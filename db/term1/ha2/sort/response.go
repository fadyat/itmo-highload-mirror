package sort

import (
	"bytes"
	"encoding/csv"
	"io"
	"os"
	"strings"
)

// Response is the response from the Sorter and wrapper which provides the same interface
// for reading CSV data from memory or from a large file.
type Response struct {
	Filepath string
	f        *os.File
	crd      *csv.Reader
	offset   int64

	// inMemory have value only if the requested data fits in memory.
	// Otherwise, need to read from Filepath.
	inMemory *bytes.Reader
}

func (s *Response) Get() string {
	// return s.inMemory.String()
	// get all bytes from inMemory
	b := make([]byte, s.inMemory.Len())
	_, _ = s.inMemory.Read(b)
	return string(b)
}

func (s *Response) Open() error {
	if s.inMemory != nil {
		s.crd = csv.NewReader(s.inMemory)
		return nil
	}

	f, err := os.Open(s.Filepath)
	if err != nil {
		return err
	}

	s.f = f
	s.crd = csv.NewReader(f)
	return nil
}

func (s *Response) Close() error {
	if s.inMemory != nil {
		_, err := s.Seek(0, io.SeekStart)
		return err
	}

	return s.f.Close()
}

func (s *Response) ReadCsv() (int64, []string, error) {
	record, err := s.crd.Read()
	if err != nil {
		return s.offset, nil, err
	}

	// hardcoding "," as a fast way to calculate the record size
	recordSize := len(strings.Join(record, ",")) + 1
	s.offset += int64(recordSize)

	return s.offset, record, nil
}

func (s *Response) Seek(offset int64, whence int) (newOffset int64, err error) {
	if s.inMemory != nil {
		newOffset, err = s.inMemory.Seek(offset, whence)
		if err != nil {
			return 0, err
		}

		s.crd = csv.NewReader(s.inMemory)
	} else {
		newOffset, err = s.f.Seek(offset, whence)
		if err != nil {
			return 0, err
		}

		s.crd = csv.NewReader(s.f)
	}

	s.offset = newOffset
	return newOffset, nil
}
