package runner

import (
	"context"
	"encoding/json"
	"fmt"
	"strings"

	"github.com/segmentio/kafka-go"
	"go.uber.org/zap"

	"github.com/fadyat/mlbd/hw2/consumers/internal/model"
)

type HandleFunc func(ctx context.Context, u *model.User) error

type Config struct {
	Brokers []string
	Topic   string
	Group   string
}

func Run(ctx context.Context, cfg Config, log *zap.Logger, handle HandleFunc) error {
	r := kafka.NewReader(kafka.ReaderConfig{
		Brokers: cfg.Brokers,
		Topic:   cfg.Topic,
		GroupID: cfg.Group,
	})
	defer func() { _ = r.Close() }()

	for {
		msg, err := r.FetchMessage(ctx)
		if err != nil {
			if ctx.Err() != nil {
				return nil
			}
			return fmt.Errorf("kafka.Reader.FetchMessage: %w", err)
		}

		var u model.User
		if err := json.Unmarshal(msg.Value, &u); err != nil {
			log.Warn(
				"skip invalid message",
				zap.Int64("offset", msg.Offset),
				zap.String("preview", preview(msg.Value)),
				zap.Error(err),
			)
			if err := r.CommitMessages(ctx, msg); err != nil {
				return fmt.Errorf("kafka.Reader.CommitMessages: %w", err)
			}
			continue
		}

		if err := handle(ctx, &u); err != nil {
			return fmt.Errorf("handle: %w", err)
		}

		if err := r.CommitMessages(ctx, msg); err != nil {
			return fmt.Errorf("kafka.Reader.CommitMessages: %w", err)
		}
	}
}

func preview(b []byte) string {
	const previewLen = 80
	s := strings.ReplaceAll(string(b), "\n", " ")
	if len(s) > previewLen {
		s = s[:previewLen] + "..."
	}
	return s
}
