package join

import (
	"cmp"
	"io"
	"os"
	"sync"

	"github.com/fadyat/sort-merge-join/sort"
)

type Type string

const (
	Left  Type = "left"
	Right Type = "right"
	Inner Type = "inner"
	Outer Type = "outer"
)

// Sort-Merge Join Algorithm.
//
//  This algorithm is designed to perform a join operation between two large tables that can't fit entirely in memory.
//  The process consists of two main phases: sorting and joining.
//  It relies on principles similar to the external merge sort algorithm.
//
//  1. Sort Phase:
//     - Each table is sorted by the join key(s) using an external sorting algorithm.
//       This means that individual pages are loaded, sorted, and written back to disk.
//     - After sorting, the entire dataset for both tables is stored on disk as multiple sorted pages.
//
//  2. Join Phase:
//     - Two cursors are used to step through the sorted pages of the two tables simultaneously.
//     - Matching tuples are emitted as output.
//     - Depending on the type of join, additional logic may be required to handle cases where one
//       table has no matching row in the other.
//     - Backtracking may be needed in cases where one table has multiple rows with the
//       same join key(s), to ensure all matches are processed correctly.
//
// Cost of Sort-Merge Join.
//
//  We have two relations R (outer) with r tuples and S (inner) with s tuples.
//  With M and N as the number of pages in R and S respectively.
//  Also, we have B as the number of buffer pages available.
//
//  Sort cost (R): 2M * (1 + log_(B-1)_(M/B))
//  Sort cost (S): 2N * (1 + log_(B-1)_(N/B))
//  Merge cost: (M + N)
//  Cost = sort + merge
//
//  The worst case for the merging phase is when the join attribute for all tuples in both tables is the same.
//  Cost: (M * N) + (sort cost)

type Options struct {
	// BufferSize specifies the maximum size of the buffer used to store data in memory.
	// If the data exceeds this size, it will be spilled to disk in temporary files.
	//
	// Default: 64MiB
	BufferSize int

	// JoinType specifies the type of join to perform.
	// Default: Inner
	JoinType Type
}

func NewOptions(fn ...func(*Options)) *Options {
	o := &Options{
		BufferSize: 64 * 1024 * 1024,
		JoinType:   Inner,
	}

	for _, f := range fn {
		f(o)
	}

	return o
}

type SortMergeJoin struct {
	filepath string
	buffer   *sort.Buffer
	opts     *Options
	left     *sort.Sorter
	right    *sort.Sorter
}

func New(filepath string, left, right *sort.Sorter, opts *Options) *SortMergeJoin {
	return &SortMergeJoin{
		filepath: filepath,
		buffer:   sort.NewBuffer(opts.BufferSize),
		opts:     opts,
		left:     left,
		right:    right,
	}
}

func (s *SortMergeJoin) Join() error {
	var (
		wg                          sync.WaitGroup
		leftResponse, rightResponse *sort.Response
		leftErr, rightErr           error
	)

	wg.Add(2)
	go func() {
		defer wg.Done()

		leftResponse, leftErr = s.left.Sort()
	}()

	go func() {
		defer wg.Done()

		rightResponse, rightErr = s.right.Sort()
	}()
	wg.Wait()

	if leftErr != nil {
		return leftErr
	}

	if rightErr != nil {
		return rightErr
	}

	return s.join(leftResponse, rightResponse)
}

func (s *SortMergeJoin) writeRecordWithOptionalFlush(w io.Writer, joinedRecord []string) {
	s.buffer.WriteStrings(joinedRecord)
	if s.buffer.Len() > s.opts.BufferSize {
		_ = s.buffer.Flush(w)
	}
}

func (s *SortMergeJoin) compare(left, right []string) int {
	if len(right) == 0 {
		return -1
	}

	if len(left) == 0 {
		return 1
	}

	return cmp.Compare(left[s.left.Opts.SortColumn], right[s.right.Opts.SortColumn])
}

func (s *SortMergeJoin) join(left, right *sort.Response) error {
	out, err := os.Create(s.filepath)
	if err != nil {
		return err
	}
	defer out.Close()

	leftMeta, leftErr := sort.NewMeta(left)
	if leftErr != nil {
		return leftErr
	}

	rightMeta, rightErr := sort.NewMeta(right)
	if rightErr != nil {
		return rightErr
	}

	if leftErr = left.Open(); leftErr != nil {
		return leftErr
	}
	defer left.Close()

	if rightErr = right.Open(); rightErr != nil {
		return rightErr
	}
	defer right.Close()

	var (
		leftRecord, rightRecord []string

		// offset to backtrack to the right record in case of multiple rows with the same join key(s)
		// saving previous right offset, because when reading, we are returning the offset where start
		// to read to get the next record, not the current one.
		previousRightOffset, rightOffset int64

		// backtracking offset to read the right record from, if next left record will have the same key
		// as the current one. done to ensure all matches are processed correctly.
		backtrackOffset      *int64
		rightLastMatchingKey string
	)

	for !(leftErr == io.EOF && rightErr == io.EOF) {
		if leftRecord == nil && leftErr == nil {
			_, leftRecord, leftErr = left.ReadCsv()
			if leftErr != nil && leftErr != io.EOF {
				return leftErr
			}

			if leftErr != io.EOF && backtrackOffset != nil && rightLastMatchingKey == leftRecord[s.left.Opts.SortColumn] {
				rightOffset, rightErr = right.Seek(*backtrackOffset, io.SeekStart)
				if rightErr != nil {
					return rightErr
				}

				rightRecord = nil
			}

			backtrackOffset = nil
		}

		if rightRecord == nil && rightErr == nil {
			previousRightOffset = rightOffset
			rightOffset, rightRecord, rightErr = right.ReadCsv()
			if rightErr != nil && rightErr != io.EOF {
				return rightErr
			}
		}

		switch s.compare(leftRecord, rightRecord) {
		case -1:
			if (s.opts.JoinType == Outer || s.opts.JoinType == Left) && backtrackOffset == nil {
				s.writeRecordWithOptionalFlush(out, append(leftRecord, s.emptyRecord(rightMeta.ColumnsNumber)...))
			}
			leftRecord = nil
		case 1:
			if (s.opts.JoinType == Outer || s.opts.JoinType == Right) && backtrackOffset == nil {
				s.writeRecordWithOptionalFlush(out, append(s.emptyRecord(leftMeta.ColumnsNumber), rightRecord...))
			}
			rightRecord = nil
		case 0:
			if backtrackOffset == nil {
				// copying the offset to avoid changing the original value
				offset := previousRightOffset
				backtrackOffset = &offset
				rightLastMatchingKey = rightRecord[s.right.Opts.SortColumn]
			}

			s.writeRecordWithOptionalFlush(out, append(leftRecord, rightRecord...))
			rightRecord = nil
		}
	}

	return s.buffer.Flush(out)
}

func (s *SortMergeJoin) emptyRecord(n uint) []string {
	return make([]string, n)
}
