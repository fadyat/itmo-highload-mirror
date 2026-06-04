package main

import (
	"context"
	"fmt"
	"os"
	"os/signal"
	"syscall"

	"github.com/caarlos0/env/v11"
	"go.mongodb.org/mongo-driver/v2/bson"
	"go.mongodb.org/mongo-driver/v2/mongo"
	"go.mongodb.org/mongo-driver/v2/mongo/options"
	"go.uber.org/zap"

	"github.com/fadyat/mlbd/hw2/consumers/internal/model"
	"github.com/fadyat/mlbd/hw2/consumers/internal/runner"
)

type config struct {
	KafkaBrokers []string `env:"KAFKA_BROKERS,required" envSeparator:","`
	KafkaTopic   string   `env:"KAFKA_TOPIC" envDefault:"users"`
	KafkaGroup   string   `env:"KAFKA_GROUP" envDefault:"mongo-writer"`

	MongoURI        string `env:"MONGO_URI,required"`
	MongoDatabase   string `env:"MONGO_DATABASE"   envDefault:"app"`
	MongoCollection string `env:"MONGO_COLLECTION" envDefault:"users"`
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

	ctx, stop := signal.NotifyContext(context.Background(), syscall.SIGINT, syscall.SIGTERM)
	defer stop()

	client, err := mongo.Connect(options.Client().ApplyURI(cfg.MongoURI))
	if err != nil {
		return fmt.Errorf("mongo.Connect: %w", err)
	}
	defer func() { _ = client.Disconnect(context.Background()) }()

	if err := client.Ping(ctx, nil); err != nil {
		return fmt.Errorf("mongo.Client.Ping: %w", err)
	}

	runCfg := runner.Config{
		Brokers: cfg.KafkaBrokers,
		Topic:   cfg.KafkaTopic,
		Group:   cfg.KafkaGroup,
	}

	return runner.Run(ctx, runCfg, log, func(ctx context.Context, u *model.User) error {
		coll := client.Database(cfg.MongoDatabase).Collection(cfg.MongoCollection)
		upsert := options.UpdateOne().SetUpsert(true)
		filter := bson.M{"user_id": u.UserID}
		update := bson.M{"$set": u}

		if _, err := coll.UpdateOne(ctx, filter, update, upsert); err != nil {
			return fmt.Errorf("mongo.Collection.UpdateOne: %w", err)
		}
		return nil
	})
}
