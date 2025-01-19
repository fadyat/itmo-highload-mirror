package cmd

import (
	"fmt"
	"log/slog"

	"github.com/spf13/cobra"
)

var domainsCmd = &cobra.Command{
	Use:   "8_domains",
	Short: "Calculate number of repositories per 2nd level domain",
	RunE: func(cmd *cobra.Command, _ []string) error {
		output := cmd.Flag("output-collection").Value.String()

		args := &mongoShellArgs{
			coreArgs: newCoreArgsFromCmd(cmd),
			filename: "cmd/scripts/08_domains.js",
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
	rootCmd.AddCommand(domainsCmd)

	registerDefaultFlags(domainsCmd)
	domainsCmd.Flags().StringP("output-collection", "o", "domains", "Output collection name")
}
