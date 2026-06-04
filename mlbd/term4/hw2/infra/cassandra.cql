create keyspace if not exists app
  with replication = {'class': 'SimpleStrategy', 'replication_factor': 1};

create table if not exists app.users (
  country    text,
  user_id    uuid,
  name       text,
  email      text,
  created_at timestamp,
  primary key (country, user_id)
);
