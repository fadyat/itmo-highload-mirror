CREATE (w1:Warehouse {id: 1, name: "Склад-Москва",    city: "Москва"});
CREATE (w2:Warehouse {id: 2, name: "Склад-Петербург", city: "Санкт-Петербург"});
CREATE (w3:Warehouse {id: 3, name: "Склад-Казань",    city: "Казань"});

CREATE (s1:Store {id: 1, name: "Магазин-Центр",    city: "Москва"});
CREATE (s2:Store {id: 2, name: "Магазин-Север",    city: "Санкт-Петербург"});
CREATE (s3:Store {id: 3, name: "Магазин-Восток",   city: "Казань"});
CREATE (s4:Store {id: 4, name: "Магазин-Юг",       city: "Краснодар"});

CREATE (sup1:Supplier {id: 1, name: "ТехноПром",    country: "Россия"});
CREATE (sup2:Supplier {id: 2, name: "ChinaGoods",   country: "Китай"});
CREATE (sup3:Supplier {id: 3, name: "EuroParts",    country: "Германия"});

CREATE (p1:Product {id: 1, name: "Ноутбук",     category: "Электроника", price: 75000});
CREATE (p2:Product {id: 2, name: "Монитор",     category: "Электроника", price: 32000});
CREATE (p3:Product {id: 3, name: "Клавиатура",  category: "Периферия",   price: 5000});
CREATE (p4:Product {id: 4, name: "Принтер",     category: "Оргтехника",  price: 18000});

MATCH (w:Warehouse {id: 1}), (s:Store {id: 1}) CREATE (w)-[:ROUTE {distance: 15,  time: 30}]->(s);
MATCH (w:Warehouse {id: 1}), (s:Store {id: 4}) CREATE (w)-[:ROUTE {distance: 1350, time: 960}]->(s);
MATCH (w:Warehouse {id: 2}), (s:Store {id: 2}) CREATE (w)-[:ROUTE {distance: 10,  time: 20}]->(s);
MATCH (w:Warehouse {id: 2}), (s:Store {id: 1}) CREATE (w)-[:ROUTE {distance: 710, time: 480}]->(s);
MATCH (w:Warehouse {id: 3}), (s:Store {id: 3}) CREATE (w)-[:ROUTE {distance: 12,  time: 25}]->(s);
MATCH (w:Warehouse {id: 3}), (s:Store {id: 4}) CREATE (w)-[:ROUTE {distance: 1500, time: 1080}]->(s);

MATCH (sup:Supplier {id: 1}), (w:Warehouse {id: 1}) CREATE (sup)-[:SUPPLIES]->(w);
MATCH (sup:Supplier {id: 2}), (w:Warehouse {id: 2}) CREATE (sup)-[:SUPPLIES]->(w);
MATCH (sup:Supplier {id: 2}), (w:Warehouse {id: 3}) CREATE (sup)-[:SUPPLIES]->(w);
MATCH (sup:Supplier {id: 3}), (w:Warehouse {id: 1}) CREATE (sup)-[:SUPPLIES]->(w);

MATCH (w:Warehouse {id: 1}), (p:Product {id: 1}) CREATE (w)-[:STOCKS {quantity: 100}]->(p);
MATCH (w:Warehouse {id: 1}), (p:Product {id: 2}) CREATE (w)-[:STOCKS {quantity: 50}]->(p);
MATCH (w:Warehouse {id: 2}), (p:Product {id: 1}) CREATE (w)-[:STOCKS {quantity: 75}]->(p);
MATCH (w:Warehouse {id: 2}), (p:Product {id: 3}) CREATE (w)-[:STOCKS {quantity: 200}]->(p);
MATCH (w:Warehouse {id: 3}), (p:Product {id: 4}) CREATE (w)-[:STOCKS {quantity: 60}]->(p);
MATCH (w:Warehouse {id: 3}), (p:Product {id: 2}) CREATE (w)-[:STOCKS {quantity: 30}]->(p);

MATCH (s:Store {id: 1}), (p:Product {id: 1}) CREATE (s)-[:ORDERS {quantity: 10}]->(p);
MATCH (s:Store {id: 1}), (p:Product {id: 2}) CREATE (s)-[:ORDERS {quantity: 5}]->(p);
MATCH (s:Store {id: 2}), (p:Product {id: 1}) CREATE (s)-[:ORDERS {quantity: 8}]->(p);
MATCH (s:Store {id: 2}), (p:Product {id: 3}) CREATE (s)-[:ORDERS {quantity: 20}]->(p);
MATCH (s:Store {id: 3}), (p:Product {id: 4}) CREATE (s)-[:ORDERS {quantity: 15}]->(p);
MATCH (s:Store {id: 4}), (p:Product {id: 1}) CREATE (s)-[:ORDERS {quantity: 12}]->(p);
MATCH (s:Store {id: 4}), (p:Product {id: 4}) CREATE (s)-[:ORDERS {quantity: 7}]->(p);
