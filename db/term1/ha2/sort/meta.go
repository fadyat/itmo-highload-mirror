package sort

type Meta struct {
	ColumnsNumber uint
}

// NewMeta returns details about the table, here it's done by single
// table scan. In the real world, it's store in some kind of metadata store.
func NewMeta(r *Response) (*Meta, error) {
	meta := &Meta{}

	if err := r.Open(); err != nil {
		return nil, err
	}
	defer r.Close()

	_, record, err := r.ReadCsv()
	if err != nil {
		return nil, err
	}

	meta.ColumnsNumber = uint(len(record))
	return meta, nil
}
