package cmd

import (
	"fmt"
	"log/slog"

	"github.com/spf13/cobra"
)

var topkCmd = &cobra.Command{
	Use:   "5_topk",
	Short: "Search top K repositories by open issues",
	RunE: func(cmd *cobra.Command, _ []string) error {
		output := cmd.Flag("output-collection").Value.String()

		args := &mongoShellArgs{
			coreArgs: newCoreArgsFromCmd(cmd),
			filename: "cmd/scripts/05_topk.js",
			eval: fmt.Sprintf(
				"let k = %s; let inputCollection = '%s'; let outputCollection = '%s';",
				cmd.Flag("k").Value.String(),
				cmd.Flag("collection").Value.String(),
				output,
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
	rootCmd.AddCommand(topkCmd)

	registerDefaultFlags(topkCmd)
	topkCmd.Flags().StringP("output-collection", "o", "topk", "Output collection name")
	topkCmd.Flags().IntP("k", "k", 10, "Number of top repositories to search")
}
