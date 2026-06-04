MATCH (s:Store)-[:ORDERS]->(p:Product {name: 'Ноутбук'}) RETURN s.name, s.city;
