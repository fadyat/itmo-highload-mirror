package cmd

import (
	"github.com/spf13/cobra"
)

var actorsCmd = &cobra.Command{
	Use:   "shell",
	Short: "Counts some actor-related statistics",
	RunE: func(cmd *cobra.Command, _ []string) error {
		args := &mongoShellArgs{
			coreArgs: coreArgs{
				url:        cmd.Flag("url").Value.String(),
				database:   cmd.Flag("database").Value.String(),
				collection: cmd.Flag("collection").Value.String(),
			},
			filename: cmd.Flag("filename").Value.String(),
		}

		return runMongoShell(cmd.Context(), args)
	},
}

func init() {
	rootCmd.AddCommand(actorsCmd)

	actorsCmd.Flags().StringP("url", "u", "mongodb://admin:admin@localhost:27017", "MongoDB server URL")
	actorsCmd.Flags().StringP("database", "d", "test", "Database name")
	actorsCmd.Flags().StringP("collection", "c", "test", "Collection name")
	actorsCmd.Flags().StringP("filename", "f", "./cmd/actors.js", "JavaScript file to run")
}
