db.products.aggregate([
  { $lookup: { from: "categories", localField: "category_id", foreignField: "_id", as: "category" } },
  { $unwind: "$category" },
  { $project: { _id: 0, name: 1, category_name: "$category.category_name" } },
]);
