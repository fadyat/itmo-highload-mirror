package sort

import (
	"bytes"
	"encoding/csv"
	"io"
)

type Buffer struct {
	b   bytes.Buffer
	crd *csv.Reader
	cwr *csv.Writer
}

func NewBuffer(size int) *Buffer {
	b := &Buffer{
		b: bytes.Buffer{},
	}

	b.b.Grow(size)
	b.crd = csv.NewReader(&b.b)
	b.cwr = csv.NewWriter(&b.b)
	return b
}

func (b *Buffer) Len() int {
	return b.b.Len()
}

func (b *Buffer) WriteBytes(c []byte) {
	_, _ = b.b.Write(c)
}

func (b *Buffer) WriteStrings(record []string) {
	_ = b.cwr.Write(record)
}

func (b *Buffer) Flush(w io.Writer) error {
	b.cwr.Flush()
	if err := b.cwr.Error(); err != nil {
		return err
	}
	_, err := b.b.WriteTo(w)
	return err
}

func (b *Buffer) ToCsv() ([][]string, error) {
	return b.crd.ReadAll()
}

func (b *Buffer) WriteCsv(records [][]string) error {
	return b.cwr.WriteAll(records)
}

func (b *Buffer) Reset() {
	b.cwr.Flush()
	b.b.Reset()
}
