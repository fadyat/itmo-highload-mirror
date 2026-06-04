create schema if not exists norm;
create schema if not exists star;

create table norm.countries
(
    country_id   serial primary key,
    country_name text not null unique
);

create table norm.cities
(
    city_id    serial primary key,
    city_name  text not null,
    country_id int  not null references norm.countries (country_id),
    unique (city_name, country_id)
);

create table norm.facilities
(
    facility_id   serial primary key,
    facility_name text             not null,
    city_id       int              not null references norm.cities (city_id),
    lat           double precision not null,
    lng           double precision not null,
    unique (facility_name, city_id)
);

create table norm.menu_items
(
    menu_item_id serial primary key,
    name         text   not null unique,
    price        bigint not null
);

create table norm.orders
(
    order_id    bigint primary key,
    facility_id int         not null references norm.facilities (facility_id),
    ordered_at  timestamptz not null
);

create table norm.order_items
(
    order_item_id bigserial primary key,
    order_id      bigint not null references norm.orders (order_id),
    menu_item_id  int    not null references norm.menu_items (menu_item_id),
    price         bigint not null
);

create table star.dim_facility
(
    facility_sk   serial primary key,
    facility_name text             not null,
    city          text             not null,
    country       text             not null,
    lat           double precision not null,
    lng           double precision not null
);

create table star.dim_menu_item
(
    menu_item_sk serial primary key,
    name         text   not null,
    price        bigint not null
);

create table star.dim_date
(
    date_sk     int primary key,
    date_value  date not null,
    year        int  not null,
    month       int  not null,
    day         int  not null,
    day_of_week int  not null
);

create table star.fact_order_items
(
    fact_id      bigserial primary key,
    order_id     bigint      not null,
    facility_sk  int         not null references star.dim_facility (facility_sk),
    menu_item_sk int         not null references star.dim_menu_item (menu_item_sk),
    date_sk      int         not null references star.dim_date (date_sk),
    ordered_at   timestamptz not null,
    price        bigint      not null
);
