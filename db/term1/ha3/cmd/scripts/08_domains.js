collection.aggregate([
    {$match: {type: "PushEvent"}},
    {
        $project: {
            _id: 0,
            repo_name: "$repo.name",
            domains: {
                $filter: {
                    input: {
                        $map: {
                            input: "$payload.commits",
                            as: "commit",
                            in: {
                                $regexFind: {
                                    input: "$$commit.author.email",
                                    regex: /@([a-zA-Z0-9-]+)\.[a-zA-Z0-9-]+/,
                                }
                            }
                        },
                    },
                    as: "domain",
                    cond: {$ne: ["$$domain.match", null]},
                }
            }
        }
    },
    {
        $project: {
            _id: 0,
            repo_name: 1,
            domains: {
                $map: {
                    input: "$domains",
                    as: "domain",
                    in: { $arrayElemAt: ["$$domain.captures", 0]}
                }
            }
        }
    },
    {$unwind: "$domains"},
    {$group: {_id: {domain: "$domains", repo_name: "$repo_name"}, count: {$sum: 1}}},
    {$group: {_id: "$_id.domain", repo_count: {$sum: 1}}},
    {$match: {_id: {$ne: null}}},
    {$sort: {repo_count: -1}},
    {$out: outputCollection}
])