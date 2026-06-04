MATCH (w:Warehouse)-[r:ROUTE]->(s:Store) RETURN w.name, s.name, r.distance;
