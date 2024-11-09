-- ```
-- CREATE TABLE nums(a INT CHECK (a > 0), b INT CHECK(b > 0));
-- ```
--
-- Напишем CTE-запрос, который реализует массовое применение алгоритма Евклида и вернет ответ с тремя полями:
-- a, b, gcd.
--
-- Если одно из чисел 0, то gcd должно содержать другое число.

create table if not exists nums
(
    a int check (a > 0),
    b int check (b > 0)
);

insert into nums
select
    random() * 100 + 1,
    random() * 100 + 1
from generate_series(1, 10);

with recursive gcd as (
    select a as a0, b as b0, a, b
    from nums
    union all
    select a0, b0, b, a % b
    from gcd
    where b > 0
)
select
    a0 as a,
    b0 as b,
    a as gcd
from gcd
where b = 0;

