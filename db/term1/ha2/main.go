package main

import (
	"log"
	"os"

	"github.com/fadyat/sort-merge-join/cli"
	"github.com/fadyat/sort-merge-join/join"
	"github.com/fadyat/sort-merge-join/sort"
)

func main() {
	if err := run(); err != nil {
		log.Fatalln(err)
	}
}

func run() error {
	args, err := cli.ParseArgs(os.Args[1:])
	if err != nil {
		return err
	}

	return join.New(
		args.OutputFile,
		sort.New(args.LFilePath, sort.NewOptions(func(o *sort.Options) { o.SortColumn = args.LField })),
		sort.New(args.RFilePath, sort.NewOptions(func(o *sort.Options) { o.SortColumn = args.RField })),
		join.NewOptions(func(options *join.Options) { options.JoinType = args.JoinType }),
	).Join()
}
