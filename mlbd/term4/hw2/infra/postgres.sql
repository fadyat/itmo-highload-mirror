create table if not exists users
(
    user_id    uuid        primary key,
    name       text        not null,
    email      text        not null unique,
    country    text        not null,
    created_at timestamptz not null default now()
);

create index if not exists users_country_idx on users (country);
