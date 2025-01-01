package cmd

import (
	"fmt"
	"log/slog"

	"github.com/spf13/cobra"
)

var reverseCmd = &cobra.Command{
	Use:   "6_reverse",
	Short: "Find some interesting pairs",
	RunE: func(cmd *cobra.Command, _ []string) error {
		output := cmd.Flag("output-collection").Value.String()

		args := &mongoShellArgs{
			coreArgs: newCoreArgsFromCmd(cmd),
			filename: "cmd/scripts/06_reverse.js",
			eval:     fmt.Sprintf("let outputCollection = '%s';", output),
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
	rootCmd.AddCommand(reverseCmd)

	registerDefaultFlags(reverseCmd)
	reverseCmd.Flags().StringP("output-collection", "o", "reverse", "Output collection name")
}
