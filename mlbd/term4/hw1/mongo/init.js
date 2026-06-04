db = db.getSiblingDB("orders");

db.createCollection("orders", {
  validator: {
    $jsonSchema: {
      bsonType: "object",
      required: ["_id", "facility", "ordered_at", "total", "items"],
      properties: {
        _id: { bsonType: "long" },
        facility: {
          bsonType: "object",
          required: ["name", "city", "country", "lat", "lng"],
          properties: {
            name: { bsonType: "string" },
            city: { bsonType: "string" },
            country: { bsonType: "string" },
            lat: { bsonType: "double", minimum: -90, maximum: 90 },
            lng: { bsonType: "double", minimum: -180, maximum: 180 },
          },
        },
        ordered_at: { bsonType: "date" },
        total: { bsonType: "long", minimum: 0 },
        items: {
          bsonType: "array",
          minItems: 1,
          items: {
            bsonType: "object",
            required: ["name", "price"],
            properties: {
              name: { bsonType: "string" },
              price: { bsonType: "long", minimum: 0 },
            },
          },
        },
      },
    },
  },
  validationLevel: "strict",
  validationAction: "error",
});

db.orders.createIndex({ "facility.city": 1, ordered_at: 1 });
