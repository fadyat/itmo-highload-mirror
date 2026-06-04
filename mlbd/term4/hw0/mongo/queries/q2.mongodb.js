db.products.aggregate([
  { $group: { _id: "$category_id", product_count: { $sum: 1 } } },
  { $lookup: { from: "categories", localField: "_id", foreignField: "_id", as: "category" } },
  { $unwind: "$category" },
  { $project: { _id: 0, category_name: "$category.category_name", product_count: 1 } },
]);
