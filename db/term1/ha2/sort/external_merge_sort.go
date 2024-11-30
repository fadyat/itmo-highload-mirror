package sort

import (
	"bufio"
	"bytes"
	"encoding/csv"
	"fmt"
	"io"
	"os"
	"sort"
	"time"

	"github.com/gammazero/deque"
)

type Options struct {

	// WorkDir specifies the working directory, which is used to store temporary files
	// during the sorting process.
	//
	// Default: os.TempDir()
	WorkDir string

	// BufferSize specifies the maximum size of the buffer used to store data in memory.
	// If the data exceeds this size, it will be spilled to disk in temporary files.
	//
	// Default: 64MiB
	BufferSize int

	// SortColumn specifies the column number to sort the data on.
	//
	// Default: 0
	SortColumn uint
}

func NewOptions(fn ...func(*Options)) *Options {
	o := &Options{
		WorkDir:    os.TempDir(),
		BufferSize: 64 * 1024 * 1024,
		SortColumn: 0,
	}

	for _, f := range fn {
		f(o)
	}

	return o
}

type Sorter struct {
	filepath string
	Opts     *Options
	chunks   *deque.Deque[string]

	buffer *Buffer
}

func New(filepath string, opts *Options) *Sorter {
	return &Sorter{
		filepath: filepath,
		Opts:     opts,
		chunks:   &deque.Deque[string]{},
		buffer:   NewBuffer(opts.BufferSize),
	}
}

func (s *Sorter) Sort() (*Response, error) {
	inMemory, err := s.initialSort()
	if err != nil {
		return nil, err
	}

	if inMemory {
		return &Response{inMemory: bytes.NewReader(s.buffer.b.Bytes())}, nil
	}

	if err := s.mergeSort(); err != nil {
		return nil, err
	}

	return &Response{Filepath: s.chunks.PopBack()}, nil
}

func (s *Sorter) initialSort() (inMemory bool, err error) {
	f, err := os.Open(s.filepath)
	if err != nil {
		return false, err
	}
	defer f.Close()

	scanner := bufio.NewScanner(f)
	for scanner.Scan() {
		s.buffer.WriteBytes(append(scanner.Bytes(), '\n'))
		if s.buffer.Len() > s.Opts.BufferSize {
			if err := s.sortInMemory(); err != nil {
				return false, err
			}

			if err := s.createNewChunk(s.buffer.Flush); err != nil {
				return false, err
			}
		}
	}

	if err := scanner.Err(); err != nil {
		return false, err
	}

	if err := s.sortInMemory(); err != nil {
		return false, err
	}

	if s.chunks.Len() == 0 {
		return true, nil
	}

	if s.buffer.Len() == 0 {
		s.buffer.cwr.Flush()
		return false, nil
	}

	return false, s.createNewChunk(s.buffer.Flush)
}

func (s *Sorter) sortInMemory() error {
	records, err := s.buffer.ToCsv()
	if err != nil {
		return err
	}

	sort.Slice(records, func(i, j int) bool {
		return records[i][s.Opts.SortColumn] < records[j][s.Opts.SortColumn]
	})

	s.buffer.Reset()
	return s.buffer.WriteCsv(records)
}

func (s *Sorter) createNewChunk(fn func(w io.Writer) error) error {
	chunkFileName := s.generateChunkFileName()
	f, err := os.Create(chunkFileName)
	if err != nil {
		return err
	}
	defer f.Close()

	if err := fn(f); err != nil {
		_ = os.Remove(chunkFileName)
		return err
	}

	s.chunks.PushBack(chunkFileName)
	return nil
}

func (s *Sorter) mergeSort() error {
	for s.chunks.Len() > 1 {
		c1 := s.chunks.PopFront()
		c2 := s.chunks.PopFront()

		if err := s.mergeChunks(c1, c2); err != nil {
			return err
		}

		if err := os.Remove(c1); err != nil {
			return err
		}
		if err := os.Remove(c2); err != nil {
			return err
		}
	}

	return nil
}

func (s *Sorter) mergeChunks(p1, p2 string) error {
	c1, err := os.Open(p1)
	if err != nil {
		return err
	}
	defer c1.Close()

	c2, err := os.Open(p2)
	if err != nil {
		return err
	}
	defer c2.Close()

	return s.createNewChunk(func(w io.Writer) error {
		return s.mergeCSV(csv.NewReader(c1), csv.NewReader(c2), w)
	})
}

func (s *Sorter) writeRecordWithOptionalFlush(w io.Writer, record []string) {
	s.buffer.WriteStrings(record)
	if s.buffer.Len() > s.Opts.BufferSize {
		_ = s.buffer.Flush(w)
	}
}

func (s *Sorter) mergeCSV(r1, r2 *csv.Reader, w io.Writer) error {
	var (
		row1, row2 []string
		err1, err2 error
	)

	readRow := func(r *csv.Reader, row *[]string, err *error) {
		if *row == nil && *err == nil {
			*row, *err = r.Read()
		}
	}

	for !(err1 == io.EOF && err2 == io.EOF) {
		readRow(r1, &row1, &err1)
		if err1 != nil && err1 != io.EOF {
			return err1
		}

		readRow(r2, &row2, &err2)
		if err2 != nil && err2 != io.EOF {
			return err2
		}

		switch {
		case row1 != nil && (row2 == nil || row1[s.Opts.SortColumn] < row2[s.Opts.SortColumn]):
			s.writeRecordWithOptionalFlush(w, row1)
			row1 = nil
		case row2 != nil:
			s.writeRecordWithOptionalFlush(w, row2)
			row2 = nil
		}
	}

	return s.buffer.Flush(w)
}

func (s *Sorter) generateChunkFileName() string {
	return fmt.Sprintf("%s/%s_%d", s.Opts.WorkDir, "chunk", time.Now().UnixNano())
}
