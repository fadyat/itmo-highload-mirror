package main

import (
	"context"
	"errors"
	"fmt"
	"net/http"
	"os"
	"os/signal"
	"syscall"
	"time"

	"github.com/caarlos0/env/v11"
	_ "github.com/jackc/pgx/v5/stdlib"
	"go.uber.org/zap"
	"golang.org/x/sync/errgroup"

	"github.com/fadyat/mlbd/hw2/api/internal/dumper"
	"github.com/fadyat/mlbd/hw2/api/internal/httpapi"
	"github.com/fadyat/mlbd/hw2/api/internal/store"
)

const (
	readHeaderTimeout = 5 * time.Second
	shutdownTimeout   = 10 * time.Second
)

type config struct {
	PgDSN        string        `env:"PG_DSN,required"`
	DumpDir      string        `env:"DUMP_DIR"      envDefault:"/data"`
	DumpInterval time.Duration `env:"DUMP_INTERVAL" envDefault:"60s"`
	HTTPAddr     string        `env:"HTTP_ADDR"     envDefault:":8080"`
}

func main() {
	if err := run(); err != nil {
		_, _ = fmt.Fprintln(os.Stderr, err)
		os.Exit(1)
	}
}

func run() error {
	log, err := zap.NewProduction()
	if err != nil {
		return fmt.Errorf("zap.NewProduction: %w", err)
	}
	defer func() { _ = log.Sync() }()

	var cfg config
	if err := env.Parse(&cfg); err != nil {
		return fmt.Errorf("env.Parse: %w", err)
	}

	st, err := store.NewUserStore(cfg.PgDSN)
	if err != nil {
		return fmt.Errorf("store.NewUserStore: %w", err)
	}
	defer func() { _ = st.Close() }()

	ctx, stop := signal.NotifyContext(context.Background(), syscall.SIGINT, syscall.SIGTERM)
	defer stop()

	d := dumper.New(cfg.DumpDir, cfg.DumpInterval, log)
	srv := &http.Server{
		Addr: cfg.HTTPAddr,
		Handler: httpapi.Chain(
			httpapi.NewHandler(st, d, log).Routes(),
			httpapi.Recoverer(log),
			httpapi.RequestLogger(log),
		),
		ReadHeaderTimeout: readHeaderTimeout,
	}

	g, gctx := errgroup.WithContext(ctx)

	g.Go(func() error {
		return d.Run(gctx)
	})

	g.Go(func() error {
		if err := srv.ListenAndServe(); !errors.Is(err, http.ErrServerClosed) {
			return fmt.Errorf("http.Server.ListenAndServe: %w", err)
		}
		return nil
	})

	g.Go(func() error {
		<-gctx.Done()

		sctx, cancel := context.WithTimeout(context.Background(), shutdownTimeout)
		defer cancel()

		if err := srv.Shutdown(sctx); err != nil {
			return fmt.Errorf("http.Server.Shutdown: %w", err)
		}

		return nil
	})

	return g.Wait()
}
