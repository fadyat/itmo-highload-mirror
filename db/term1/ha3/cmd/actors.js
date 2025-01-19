const result = collection.aggregate([
  {
    $facet: {
      "n_no_id": [
        { $match: { "actor.id": { $exists: false } } },
        { $count: "n" }
      ],
      "n_no_login": [
        { $match: { "actor.login": { $exists: false } } },
        { $count: "n" }
      ],
      "n_one_login_many_id": [
        { $group: { "_id": "$actor.id", "uniqueLogins": { $addToSet: "$actor.login" } } },
        { $match: { "uniqueLogins.1": { $exists: true } } },
        { $count: "n" }
      ],
      "n_one_id_many_login": [
        { $group: { "_id": "$actor.login", "uniqueIds": { $addToSet: "$actor.id" } } },
        { $match: { "uniqueIds.1": { $exists: true } } },
        { $count: "n" }
      ]
    }
  },
  {
    $project: {
      n_no_id: { $arrayElemAt: ["$n_no_id.n", 0] },
      n_no_login: { $arrayElemAt: ["$n_no_login.n", 0] },
      n_one_login_many_id: { $arrayElemAt: ["$n_one_login_many_id.n", 0] },
      n_one_id_many_login: { $arrayElemAt: ["$n_one_id_many_login.n", 0] }
    }
  }
])

printjson(result)