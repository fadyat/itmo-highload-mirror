package dumper

import (
	"context"
	"encoding/json"
	"fmt"
	"os"
	"path/filepath"
	"sync"
	"time"

	"github.com/google/renameio/v2"
	"go.uber.org/zap"

	"github.com/fadyat/mlbd/hw2/api/internal/model"
)

const (
	dumpDirPerm  = 0o750
	dumpFilePerm = 0o644
)

// Dumper buffers users in memory and flushes them as JSONL to disk every interval.
type Dumper struct {
	dir      string
	interval time.Duration
	log      *zap.Logger

	mu  sync.Mutex
	buf []model.User
}

func New(dir string, interval time.Duration, log *zap.Logger) *Dumper {
	return &Dumper{dir: dir, interval: interval, log: log}
}

func (d *Dumper) Add(u model.User) {
	d.mu.Lock()
	defer d.mu.Unlock()
	d.buf = append(d.buf, u)
}

func (d *Dumper) Run(ctx context.Context) error {
	if err := os.MkdirAll(d.dir, dumpDirPerm); err != nil {
		return fmt.Errorf("os.MkdirAll: %w", err)
	}

	t := time.NewTicker(d.interval)
	defer t.Stop()

	for {
		select {
		case <-ctx.Done():
			d.flush()
			return nil
		case <-t.C:
			d.flush()
		}
	}
}

func (d *Dumper) takeBatch() []model.User {
	d.mu.Lock()
	defer d.mu.Unlock()
	batch := d.buf
	d.buf = nil
	return batch
}

func (d *Dumper) flush() {
	batch := d.takeBatch()
	if len(batch) == 0 {
		return
	}

	path := filepath.Join(d.dir, fmt.Sprintf("users-%d.jsonl", time.Now().UnixMilli()))
	if err := writeJSONL(path, batch); err != nil {
		d.log.Error("flush batch", zap.Error(err))
		return
	}

	d.log.Info("flushed batch", zap.String("file", path), zap.Int("count", len(batch)))
}

func writeJSONL(path string, records []model.User) error {
	pf, err := renameio.NewPendingFile(path, renameio.WithPermissions(dumpFilePerm))
	if err != nil {
		return fmt.Errorf("renameio.NewPendingFile: %w", err)
	}
	defer func() { _ = pf.Cleanup() }()

	enc := json.NewEncoder(pf)
	for i := range records {
		if err := enc.Encode(&records[i]); err != nil {
			return fmt.Errorf("json.Encoder.Encode: %w", err)
		}
	}

	if err := pf.CloseAtomicallyReplace(); err != nil {
		return fmt.Errorf("renameio.PendingFile.CloseAtomicallyReplace: %w", err)
	}

	return nil
}
