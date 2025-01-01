collection.aggregate([
    {
        $group: {
            _id: "$repo.name",
            branch_creators: {
                $addToSet: {
                    $cond: [
                        { $and: [{ $eq: ["$type", "CreateEvent"] }, { $eq: ["$payload.ref_type", "branch"] }] },
                        { id: "$actor.id", login: "$actor.login" },
                        "$$REMOVE",
                    ]
                }
            },
            pushers: {
                $addToSet: {
                    $cond: [
                        { $eq: ["$type", "PushEvent"] },
                        { id: "$actor.id", login: "$actor.login" },
                        "$$REMOVE",
                    ]
                }
            },
        }
    },
    {$match: {$and: [{branch_creators: {$ne: []}}, {pushers: {$ne: []}}]}},
    {$unwind: "$branch_creators"},
    {$unwind: "$pushers"},
    {$project: {_id: 0, a: "$branch_creators", b: "$pushers"}},
    {$match: { $expr: { $ne: ["$a", "$b"] }}},
    {$group: {_id: {a: "$a", b: "$b"}, count: {$sum: 1}}},
    {$project: {_id: 0, a: {$min: ["$_id.a", "$_id.b"]}, b: {$max: ["$_id.a", "$_id.b"]}}},
    {$group: {_id: {a: "$a", b: "$b"}, count: {$sum: 1}}},
    {$match: {count: 2}},
    {$project: {_id: 0, a: "$_id.a", b: "$_id.b"}},
    {$out: outputCollection}
]);
