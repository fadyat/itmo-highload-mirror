db = db.getSiblingDB("shop");

db.categories.insertMany([
  { _id: 1, category_name: "Электроника", parent_id: null },
  { _id: 2, category_name: "Смартфоны", parent_id: 1 },
  { _id: 3, category_name: "Ноутбуки", parent_id: 1 },
  { _id: 4, category_name: "Одежда", parent_id: null },
  { _id: 5, category_name: "Бытовая техника", parent_id: null },
]);

db.suppliers.insertMany([
  { _id: 1, name: "ТехноПоставка", country: "Россия" },
  { _id: 2, name: "GlobalSupply", country: "Китай" },
  { _id: 3, name: "EuroTrade", country: "Германия" },
]);

db.products.insertMany([
  { _id: 1, name: "iPhone 15", category_id: 2, price: 89990, stock: 50, supplier_id: 2 },
  { _id: 2, name: "Samsung Galaxy S24", category_id: 2, price: 74990, stock: 35, supplier_id: 2 },
  { _id: 3, name: "MacBook Pro 14", category_id: 3, price: 199990, stock: 20, supplier_id: 2 },
  { _id: 4, name: "ThinkPad X1", category_id: 3, price: 129990, stock: 15, supplier_id: 3 },
  { _id: 5, name: "Куртка зимняя", category_id: 4, price: 12990, stock: 100, supplier_id: 1 },
  { _id: 6, name: "Пылесос Dyson", category_id: 5, price: 45990, stock: 25, supplier_id: 3 },
  { _id: 7, name: "Pixel 8", category_id: 2, price: 59990, stock: 40, supplier_id: 2 },
]);

db.customers.insertMany([
  { _id: 1, full_name: "Иванов Иван", email: "ivanov@mail.ru", registration_date: new Date("2023-01-15") },
  { _id: 2, full_name: "Петрова Мария", email: "petrova@mail.ru", registration_date: new Date("2023-03-22") },
  { _id: 3, full_name: "Сидоров Алексей", email: "sidorov@mail.ru", registration_date: new Date("2023-06-10") },
  { _id: 4, full_name: "Козлова Анна", email: "kozlova@mail.ru", registration_date: new Date("2024-01-05") },
]);

db.orders.insertMany([
  { _id: 1, customer_id: 1, order_date: new Date("2024-10-01"), total_amount: 89990 },
  { _id: 2, customer_id: 1, order_date: new Date("2024-11-15"), total_amount: 199990 },
  { _id: 3, customer_id: 2, order_date: new Date("2024-10-20"), total_amount: 74990 },
  { _id: 4, customer_id: 3, order_date: new Date("2024-12-01"), total_amount: 12990 },
  { _id: 5, customer_id: 3, order_date: new Date("2024-12-05"), total_amount: 45990 },
  { _id: 6, customer_id: 4, order_date: new Date("2024-12-10"), total_amount: 59990 },
]);

db.order_items.insertMany([
  { _id: 1, order_id: 1, product_id: 1, quantity: 1, unit_price: 89990 },
  { _id: 2, order_id: 2, product_id: 3, quantity: 1, unit_price: 199990 },
  { _id: 3, order_id: 3, product_id: 2, quantity: 1, unit_price: 74990 },
  { _id: 4, order_id: 4, product_id: 5, quantity: 1, unit_price: 12990 },
  { _id: 5, order_id: 5, product_id: 6, quantity: 1, unit_price: 45990 },
  { _id: 6, order_id: 6, product_id: 7, quantity: 1, unit_price: 59990 },
]);

db.shipments.insertMany([
  { _id: 1, order_id: 1, shipment_date: new Date("2024-10-03"), status: "delivered" },
  { _id: 2, order_id: 2, shipment_date: new Date("2024-11-18"), status: "delivered" },
  { _id: 3, order_id: 3, shipment_date: new Date("2024-10-23"), status: "delivered" },
  { _id: 4, order_id: 4, shipment_date: new Date("2024-12-04"), status: "in_transit" },
  { _id: 5, order_id: 5, shipment_date: new Date("2024-12-08"), status: "pending" },
  { _id: 6, order_id: 6, shipment_date: new Date("2024-12-13"), status: "pending" },
]);
