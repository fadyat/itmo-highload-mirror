const threshold = 60000;

db.orders.aggregate([
  { $group: { _id: "$customer_id", total_spent: { $sum: "$total_amount" } } },
  { $match: { total_spent: { $gt: threshold } } },
  { $lookup: { from: "customers", localField: "_id", foreignField: "_id", as: "customer" } },
  { $unwind: "$customer" },
  { $project: { _id: 0, full_name: "$customer.full_name", total_spent: 1 } },
]);
