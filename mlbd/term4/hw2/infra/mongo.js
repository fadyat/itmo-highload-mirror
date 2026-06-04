db = db.getSiblingDB("app");

db.createCollection("users", {
  validator: {
    $jsonSchema: {
      bsonType: "object",
      required: ["user_id", "name", "email", "country", "created_at"],
      properties: {
        user_id: { bsonType: "string" },
        name: { bsonType: "string" },
        email: { bsonType: "string" },
        country: { bsonType: "string" },
        created_at: { bsonType: ["date", "string"] },
      },
    },
  },
});

db.users.createIndex({ user_id: 1 }, { unique: true });
db.users.createIndex({ country: 1 });
