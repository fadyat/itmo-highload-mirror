package cmd

import (
	"github.com/spf13/cobra"
)

var actorsCmd = &cobra.Command{
	Use:   "3_actors",
	Short: "Count actors related statistics",
	RunE: func(cmd *cobra.Command, _ []string) error {
		args := &mongoShellArgs{
			coreArgs: newCoreArgsFromCmd(cmd),
			filename: "cmd/scripts/03_actors.js",
		}

		return runMongoShell(cmd.Context(), args)
	},
	SilenceErrors: true,
}

func init() {
	rootCmd.AddCommand(actorsCmd)

	registerDefaultFlags(actorsCmd)
}
