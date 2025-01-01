const result = collection.aggregate([
    {
        $setWindowFields: {
            sortBy: {n: 1},
            output: {lag_n: {$shift: {output: "$n", by: -1}}},
        },
    },
    {
        $set: {
            period_marker: {
                $cond: [{$in: [{$subtract: ["$n", "$lag_n"]}, [0, 1]]}, 0, 1],
            },
        },
    },
    {
        $setWindowFields: {
            sortBy: {n: 1},
            output: {
                period: {
                    $sum: "$period_marker",
                    window: {documents: ["unbounded", "current"]}
                },
            },
        },
    },
    {
        $setWindowFields: {
            sortBy: {period: 1},
            output: {
                with_next_avg_a: {
                    $avg: "$a",
                    window: {range: [0, 1]},
                },
            },
        },
    },
    {
        $project: {
            _id: 0,
            id: 1,
            period: 1,
            a: 1,
            with_next_avg_a: 1,
            a_diff: {$subtract: ["$a", "$with_next_avg_a"]},
        },
    },
]);

printjson(result);