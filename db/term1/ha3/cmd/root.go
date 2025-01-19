package cmd

import (
	"context"
	"fmt"
	"log/slog"
	"os"
	"os/exec"

	"github.com/spf13/cobra"
)

type coreArgs struct {
	url        string
	database   string
	collection string
}

type mongoShellArgs struct {
	coreArgs

	filename string
	eval     string
}

var rootCmd = &cobra.Command{
	Use: "avfadeev",
	CompletionOptions: cobra.CompletionOptions{
		DisableDefaultCmd: true,
	},
	SilenceErrors: true,
}

func Execute() {
	if err := rootCmd.Execute(); err != nil {
		slog.Error(err.Error())
		os.Exit(1)
	}
}

// runMongoShell runs the mongosh process with the given arguments.
func runMongoShell(ctx context.Context, args *mongoShellArgs) error {
	cmdArgs := []string{
		args.url,
		"--eval", fmt.Sprintf("db = db.getSiblingDB('%s'); collection = db.getCollection('%s'); %s", args.database, args.collection, args.eval),
		"--file", args.filename,
	}

	c := exec.CommandContext(ctx, "mongosh", cmdArgs...)
	c.Stdout = os.Stdout
	c.Stderr = os.Stderr

	slog.Debug("Running command: mongosh ", slog.Any("args", cmdArgs))
	if err := c.Run(); err != nil {
		return fmt.Errorf("failed to run mongosh: %w", err)
	}

	return nil
}
