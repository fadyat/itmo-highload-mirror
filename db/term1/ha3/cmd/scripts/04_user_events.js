collection.aggregate([
    {
        $addFields: {
            year: {$year: {$dateFromString: {dateString: "$created_at"}}},
            month: {$month: {$dateFromString: {dateString: "$created_at"}}}
        }
    },
    {
        $group: {
            _id: {user_id: "$actor.id", year: "$year", month: "$month", type: "$type"},
            count: {$sum: 1}
        }
    },
    {
        $group: {
            _id: {user_id: "$_id.user_id", year: "$_id.year", month: "$_id.month"},
            events: {$push: {type: "$_id.type", count: "$count"}}
        }
    },
    {
        $group: {
            _id: "$_id.user_id",
            events_by_ym: {$push: {year: "$_id.year", month: "$_id.month", events: "$events"}},
            events: {$push: "$events"}
        }
    },
    {
        $addFields: {
            events: {
                $reduce: {
                    input: "$events",
                    initialValue: [],
                    in: {$concatArrays: ["$$value", "$$this"]}
                }
            }
        }
    },
    {
        $addFields: {
            events: {
                $map: {
                    input: { $setUnion: ["$events.type", []] },
                    as: "eventType",
                    in: {
                        type: "$$eventType",
                        count: {
                            $sum: {
                                $map: {
                                    input: "$events",
                                    as: "event",
                                    in: {$cond: [{$eq: ["$$event.type", "$$eventType"]}, "$$event.count", 0]}
                                }
                            }
                        }
                    }
                }
            }
        }
    },
    {
        $lookup: {
            from: inputCollection,
            let: {user_id: "$_id"},
            pipeline: [
                {$match: {$expr: {$and: [{$eq: ["$type", "PushEvent"]}, {$eq: ["$actor.id", "$$user_id"]}]}}},
                {$group: {_id: "$repo.name"}},
            ],
            as: "pushed_to"
        }
    },
    {
        $project: {
            _id: 0,
            user_id: "$_id",
            events: 1,
            events_by_ym: 1,
            pushed_to: "$pushed_to._id"
        }
    },
    {$out: outputCollection}
], {allowDiskUse: true})