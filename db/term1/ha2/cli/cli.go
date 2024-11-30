package cli

import (
	"fmt"
	"slices"
	"strconv"

	"github.com/fadyat/sort-merge-join/join"
)

type Args struct {
	LFilePath  string
	RFilePath  string
	OutputFile string
	JoinType   join.Type
	LField     uint
	RField     uint
}

func ParseArgs(args []string) (*Args, error) {
	if len(args) < 5 {
		return nil, fmt.Errorf("not enough arguments")
	}

	lfield, err := strconv.Atoi(args[1])
	if err != nil {
		return nil, fmt.Errorf("invalid left field: %w", err)
	}

	rfield, err := strconv.Atoi(args[3])
	if err != nil {
		return nil, fmt.Errorf("invalid right field: %w", err)
	}

	joinType := join.Type(args[4])
	if !slices.Contains([]join.Type{join.Left, join.Right, join.Inner, join.Outer}, joinType) {
		return nil, fmt.Errorf("invalid join type: %s", joinType)
	}

	var outputFile = "/dev/stdout"
	if len(args) >= 6 {
		outputFile = args[5]
	}

	return &Args{
		LFilePath:  args[0],
		RFilePath:  args[2],
		LField:     uint(lfield),
		RField:     uint(rfield),
		JoinType:   joinType,
		OutputFile: outputFile,
	}, nil
}
