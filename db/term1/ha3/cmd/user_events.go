package cmd

import (
	"fmt"
	"log/slog"

	"github.com/spf13/cobra"
)

var userEventsCmd = &cobra.Command{
	Use:   "4_user_events",
	Short: "Count user events related statistics",
	RunE: func(cmd *cobra.Command, _ []string) error {
		output := cmd.Flag("output-collection").Value.String()
		args := &mongoShellArgs{
			coreArgs: newCoreArgsFromCmd(cmd),
			filename: "cmd/scripts/04_user_events.js",
			eval: fmt.Sprintf(
				"let inputCollection = '%s'; let outputCollection = '%s';",
				cmd.Flag("collection").Value.String(), output,
			),
		}

		if err := runMongoShell(cmd.Context(), args); err != nil {
			return err
		}

		slog.Info(fmt.Sprintf("Your data is in the '%s' collection", output))
		return nil
	},
	SilenceErrors: true,
}

func init() {
	rootCmd.AddCommand(userEventsCmd)

	registerDefaultFlags(userEventsCmd)
	userEventsCmd.Flags().StringP("output-collection", "o", "user_stats", "Output collection name")
}
