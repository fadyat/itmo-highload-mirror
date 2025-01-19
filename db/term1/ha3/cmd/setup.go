package cmd

import (
	"compress/gzip"
	"context"
	"encoding/json"
	"errors"
	"fmt"
	"log/slog"
	"net/http"
	"strings"
	"sync"
	"time"

	"github.com/spf13/cobra"
	"go.mongodb.org/mongo-driver/v2/bson"
	"go.mongodb.org/mongo-driver/v2/mongo"
	"go.mongodb.org/mongo-driver/v2/mongo/options"
)

type setupArgs struct {
	coreArgs

	rangeStart time.Time
	rangeEnd   time.Time
}

func newSetupArguments(url, database, collection, drange string) (*setupArgs, error) {
	rangeSplit := strings.Split(drange, ":")
	if len(rangeSplit) != 2 {
		return nil, fmt.Errorf("invalid date range format, expected 'start:end'")
	}

	rangeStart, err := time.Parse("2006-01-02", rangeSplit[0])
	if err != nil {
		return nil, fmt.Errorf("invalid start date: %w", err)
	}

	rangeEnd, err := time.Parse("2006-01-02", rangeSplit[1])
	if err != nil {
		return nil, fmt.Errorf("invalid end date: %w", err)
	}

	return &setupArgs{
		coreArgs: coreArgs{
			url:        url,
			database:   database,
			collection: collection,
		},
		rangeStart: rangeStart,
		// making the range end exclusive
		rangeEnd: rangeEnd.AddDate(0, 0, 1),
	}, nil
}

var setupCmd = &cobra.Command{
	Use:   "setup",
	Short: "Downloads required data and stores it in the database",
	RunE: func(cmd *cobra.Command, _ []string) error {
		args, err := newSetupArguments(
			cmd.Flag("url").Value.String(),
			cmd.Flag("database").Value.String(),
			cmd.Flag("collection").Value.String(),
			cmd.Flag("range").Value.String(),
		)
		if err != nil {
			return fmt.Errorf("failed to parse arguments: %w", err)
		}

		opts := options.Client().
			ApplyURI(args.url).
			SetDirect(true).
			SetConnectTimeout(5 * time.Second)

		mongoClient, err := mongo.Connect(opts)
		if err != nil {
			return fmt.Errorf("failed to connect to MongoDB: %w", err)
		}

		collection := mongoClient.Database(args.database).Collection(args.collection)
		if err := prepareCollection(cmd.Context(), collection); err != nil {
			return fmt.Errorf("failed to prepare collection: %w", err)
		}

		slog.Debug("prepared collection for storing data")
		return initFunc(cmd.Context(), collection, args)
	},
	SilenceErrors: true,
}

func init() {
	slog.SetLogLoggerLevel(slog.LevelDebug)

	setupCmd.Flags().StringP("url", "u", "mongodb://admin:admin@localhost:27017", "MongoDB server URL")
	setupCmd.Flags().StringP("database", "d", "test", "Database name")
	setupCmd.Flags().StringP("collection", "c", "test", "Collection name")
	setupCmd.Flags().StringP("range", "r", "2015-01-01:2015-01-01", "Date range")

	rootCmd.AddCommand(setupCmd)
}

// initFunc downloads required data based on the provided arguments and stores it in the database under the specified collection.
// Dataset is a GitHub Archive dataset, which contains information about events on GitHub. The dataset is available at https://www.gharchive.org/.
// The dataset is a collection of JSON files, each containing a list of events for a specific hour, hours data downloaded in parallel.
func initFunc(ctx context.Context, collection *mongo.Collection, args *setupArgs) error {
	var (
		wg         sync.WaitGroup
		errorsChan = make(chan error)
		errs       = make([]error, 0)
	)

	go func() {
		defer close(errorsChan)

		for date := args.rangeStart; date.Before(args.rangeEnd); date = date.AddDate(0, 0, 1) {
			for hour := 0; hour < 24; hour++ {
				wg.Add(1)
				go func(date time.Time, hour int) {
					defer wg.Done()

					url := fmt.Sprintf("https://data.gharchive.org/%s-%d.json.gz", date.Format("2006-01-02"), hour)
					if err := downloadAndStore(ctx, collection, url); err != nil {
						errorsChan <- err
					}
				}(date, hour)
			}

			wg.Wait()
		}
	}()

	for errMsg := range errorsChan {
		errs = append(errs, errMsg)
	}

	return errors.Join(errs...)
}

func prepareCollection(ctx context.Context, collection *mongo.Collection) error {
	_, err := collection.Indexes().CreateOne(ctx, mongo.IndexModel{
		Keys: bson.M{"id": 1},
	})
	return err
}

func downloadAndStore(ctx context.Context, collection *mongo.Collection, url string) error {
	req, err := http.NewRequestWithContext(ctx, http.MethodGet, url, nil)
	if err != nil {
		return fmt.Errorf("failed to create request: %w", err)
	}

	resp, err := http.DefaultClient.Do(req)
	if err != nil {
		return fmt.Errorf("failed to get file: %w", err)
	}
	defer resp.Body.Close()

	slog.Debug(fmt.Sprintf("got response from %s", url))
	if resp.StatusCode != http.StatusOK {
		return fmt.Errorf("failed to download file: %s", resp.Status)
	}

	gzipReader, err := gzip.NewReader(resp.Body)
	if err != nil {
		return fmt.Errorf("failed to unzip file: %w", err)
	}

	result, err := insertWithoutDuplicates(ctx, collection, gzipReader)
	if err != nil {
		return fmt.Errorf("failed to insert documents: %w", err)
	}

	slog.Debug(fmt.Sprintf("stored %v documents from %s", result, url))
	return nil
}

// insertSimple was used in first task, but it was replaced with insertWithoutDuplicates to avoid duplicates in the collection.
//
//goland:noinspection ALL
func insertSimple(ctx context.Context, collection *mongo.Collection, r *gzip.Reader) (*mongo.InsertManyResult, error) { // nolint: unused
	var documents []interface{}
	jsonDecoder := json.NewDecoder(r)

	for jsonDecoder.More() {
		var document map[string]any
		if err := jsonDecoder.Decode(&document); err != nil {
			return nil, fmt.Errorf("failed to decode JSON: %w", err)
		}

		documents = append(documents, document)
	}

	insertOpts := options.InsertMany().SetOrdered(false).SetBypassDocumentValidation(true)
	insertResult, err := collection.InsertMany(ctx, documents, insertOpts)
	if err != nil {
		return nil, fmt.Errorf("failed to insert documents: %w", err)
	}

	return insertResult, nil
}

func insertWithoutDuplicates(ctx context.Context, collection *mongo.Collection, r *gzip.Reader) (*mongo.BulkWriteResult, error) {
	var models []mongo.WriteModel
	jsonDecoder := json.NewDecoder(r)

	for jsonDecoder.More() {
		var document map[string]any
		if err := jsonDecoder.Decode(&document); err != nil {
			return nil, fmt.Errorf("failed to decode JSON: %w", err)
		}

		m := mongo.NewUpdateOneModel().
			SetFilter(bson.M{"id": document["id"]}).
			SetUpdate(bson.M{"$set": document}).
			SetUpsert(true)

		models = append(models, m)
	}

	bulkWriteOpts := options.BulkWrite().SetOrdered(false).SetBypassDocumentValidation(true)
	bulkResult, err := collection.BulkWrite(ctx, models, bulkWriteOpts)
	if err != nil {
		return nil, fmt.Errorf("failed to insert documents: %w", err)
	}

	// removing UpsertedIDs from the result to not print them later
	bulkResult.UpsertedIDs = nil
	return bulkResult, nil
}
