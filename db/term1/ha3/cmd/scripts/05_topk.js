collection.aggregate([
    {$match: {type: "IssuesEvent"}},
    {
        $setWindowFields: {
            partitionBy: "$payload.issue.id",
            sortBy: {created_at: 1},
            output: {last_action: {$last: "$payload.action"}}
        }
    },
    {$match: {last_event_action: {$ne: "closed"}}},
    {
        $group: {
            _id: "$repo.name",
            issue_count: {$addToSet: "$payload.issue.id"},
        }
    },
    {
        $project: {
            _id: 1,
            org_name: {$arrayElemAt: [{$split: ["$_id", "/"]}, 0]},
            repo_name: {$arrayElemAt: [{$split: ["$_id", "/"]}, 1]},
            issue_count: {$size: "$issue_count"}
        }
    },
    {$sort: {issue_count: -1}},
    {$limit: k},
    {
        $lookup: {
            from: inputCollection,
            let: {repo_name: "$_id"},
            pipeline: [
                {
                    $match: {
                        $expr: {
                            $and: [{$eq: ["$type", "PushEvent"]}, {$eq: ["$repo.name", "$$repo_name"]}]
                        }
                    }
                },
                {$group: {_id: "$actor.id", push_count: {$sum: 1}}},
                {$count: "n"}
            ],
            as: "pushers",
        }
    },
    {
        $group: {
            _id: "$org_name",
            repos: {
                $push: {
                    repo_name: "$repo_name",
                    issue_count: "$issue_count",
                    active_pushers_count: {$arrayElemAt: ["$pushers.n", 0]}
                }
            }
        }
    },
    {$project: {_id: 0, org_name: "$_id", repos: 1}},
    {$out: outputCollection}
])
