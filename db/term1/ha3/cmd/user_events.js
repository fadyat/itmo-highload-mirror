// Сформируем коллекцию, в которой каждому GitHub-пользователю будет соответствовать документ с полями:
//
// - user_id: id GitHub-пользователя
// - events: список документов, в каждом из которых хранятся имена событий и количество таких событий, которые совершил
//   данный пользователь
// - events_by_ym: список документов, в каждом из которых хранится гистограмма событий данного пользователя по паре
//   год-месяц,
//   То есть в каждом документе есть поле с годом, поле с месяцем (число в диапазоне 1-12), и поле events - по структуре
//   как в предыдущем пункте, только с данными по отдельному месяцу. И этот список должен быть упорядочен по времени (более
//   ранние месяцы в начале)
// - pushed_to: список имен репозиториев, по которые делал push данный пользователь
//
// В гистограммах событий должны отсутствовать типы событий, которые в данные период не встречались.

let result = collection.aggregate([
  {
    $addFields: {
      "year": { $year: { $dateFromString: { dateString: "$created_at" } } },
      "month": { $month: { $dateFromString: { dateString: "$created_at" } } },
    },
  },
  {
    $group: {
      _id: {
        actor_id: "$actor.id",
        event_type: "$type",
        year: "$year",
        month: "$month",
      },
      monthly_count: { $sum: 1 },
      repos: {
        $addToSet: {
          $cond: {
            if: { $eq: ["$type", "PushEvent"] },
            then: "$repo.name",
            else: "$$REMOVE",
          },
        }
      }
    },
  },
  {
    $group: {
      _id: {
        actor_id: "$_id.actor_id",
        year: "$_id.year",
        month: "$_id.month",
      },
      events_by_ym: {
        $push: {
          event_type: "$_id.event_type",
          count: "$monthly_count",
        }
      },
      // list of lists, but we need a list of unique values
      pushed_to: { $addToSet: "$repos" }
    }
  },
//  {
//    $sort: { "_id.year": 1, "_id.month": 1 },
//  },
  {
    $group: {
      _id: "$_id.actor_id",
      events_by_ym: { $push: { year: "$_id.year", month: "$_id.month", events: "$events_by_ym" } },
      pushed_to: {
        $reduce: {
          input: "$pushed_to",
          initialValue: [],
          in: { $setUnion: ["$$value", "$$this"] }
        }
      }
    }
  },
  {
    $match: {
      $expr: { $gt: [{ $size: "$events_by_ym" }, 1] }
    }
  }
], { allowDiskUse: true })

printjson(result)
