package cmd

import (
	"context"
	"fmt"
	"time"

	"github.com/spf13/cobra"
	"go.mongodb.org/mongo-driver/v2/mongo"
	"go.mongodb.org/mongo-driver/v2/mongo/options"
)

var periodsCmd = &cobra.Command{
	Use:   "7_periods",
	Short: "Non-decreasing periods calculation",
	RunE: func(cmd *cobra.Command, _ []string) error {
		args := &mongoShellArgs{
			coreArgs: newCoreArgsFromCmd(cmd),
			filename: "cmd/scripts/07_periods.js",
		}

		if cmd.Flag("prepare").Value.String() == "true" {
			if err := preparePeriodsCollection(cmd.Context(), args); err != nil {
				return err
			}
		}

		return runMongoShell(cmd.Context(), args)
	},
	SilenceErrors: true,
}

func preparePeriodsCollection(ctx context.Context, args *mongoShellArgs) error {
	opts := options.Client().
		ApplyURI(args.url).
		SetDirect(true).
		SetConnectTimeout(5 * time.Second)

	mongoClient, err := mongo.Connect(opts)
	if err != nil {
		return fmt.Errorf("failed to connect to MongoDB: %w", err)
	}

	collection := mongoClient.Database(args.database).Collection(args.collection)
	_, err = collection.InsertMany(ctx, []any{
		map[string]any{"id": 1, "n": 2, "a": 2},
		map[string]any{"id": 2, "n": 3, "a": 3},
		map[string]any{"id": 3, "n": 5, "a": 5},
		map[string]any{"id": 4, "n": 5, "a": 5},
		map[string]any{"id": 5, "n": 7, "a": 7},
		map[string]any{"id": 6, "n": 8, "a": 8},
		map[string]any{"id": 7, "n": 9, "a": 9},
		map[string]any{"id": 8, "n": 10, "a": 10},
		map[string]any{"id": 9, "n": 12, "a": 12},
		map[string]any{"id": 10, "n": 20, "a": 20},
		map[string]any{"id": 11, "n": 25, "a": 25},
		map[string]any{"id": 12, "n": 26, "a": 26},
	})
	return err
}

func init() {
	rootCmd.AddCommand(periodsCmd)

	periodsCmd.Flags().StringP("url", "u", "mongodb://admin:admin@localhost:27017", "MongoDB server URL")
	periodsCmd.Flags().StringP("database", "d", "test", "Input database name")
	periodsCmd.Flags().StringP("collection", "c", "periods", "Input collection name")
	periodsCmd.Flags().BoolP("prepare", "p", false, "Insert some data to the collection")
}
