package main

import (
	"context"
	"fmt"
	"os"
	"os/signal"
	"syscall"

	"github.com/caarlos0/env/v11"
	"github.com/gocql/gocql"
	"go.uber.org/zap"

	"github.com/fadyat/mlbd/hw2/consumers/internal/model"
	"github.com/fadyat/mlbd/hw2/consumers/internal/runner"
)

type config struct {
	KafkaBrokers []string `env:"KAFKA_BROKERS,required" envSeparator:","`
	KafkaTopic   string   `env:"KAFKA_TOPIC"   envDefault:"users"`
	KafkaGroup   string   `env:"KAFKA_GROUP"   envDefault:"cassandra-writer"`

	CassandraHosts    []string `env:"CASSANDRA_HOSTS,required" envSeparator:","`
	CassandraKeyspace string   `env:"CASSANDRA_KEYSPACE" envDefault:"app"`
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

	cluster := gocql.NewCluster(cfg.CassandraHosts...)
	cluster.Keyspace = cfg.CassandraKeyspace
	cluster.Consistency = gocql.LocalQuorum

	session, err := cluster.CreateSession()
	if err != nil {
		return fmt.Errorf("gocql.ClusterConfig.CreateSession: %w", err)
	}
	defer session.Close()

	ctx, stop := signal.NotifyContext(context.Background(), syscall.SIGINT, syscall.SIGTERM)
	defer stop()

	runCfg := runner.Config{
		Brokers: cfg.KafkaBrokers,
		Topic:   cfg.KafkaTopic,
		Group:   cfg.KafkaGroup,
	}

	return runner.Run(ctx, runCfg, log, func(ctx context.Context, u *model.User) error {
		uid, err := gocql.ParseUUID(u.UserID)
		if err != nil {
			return fmt.Errorf("gocql.ParseUUID: %w", err)
		}

		const stmt = `insert into users (country, user_id, name, email, created_at)
	              values (?, ?, ?, ?, ?)`

		query := session.Query(stmt, u.Country, uid, u.Name, u.Email, u.CreatedAt).WithContext(ctx)
		if err := query.Exec(); err != nil {
			return fmt.Errorf("gocql.Query.Exec: %w", err)
		}

		return nil
	})
}
