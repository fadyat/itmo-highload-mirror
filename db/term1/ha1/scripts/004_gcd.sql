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

truncate nums;

insert into nums
select
    random() * 100 + 1,
    random() * 100 + 1
from generate_series(1, 10);

create or replace function gcd(a int, b int)
    returns int as
$$
    with recursive gcd_recursive(a, b) as (
        select a, b
        union all
        select b, a % b
        from gcd_recursive
        where b != 0
    )
    select a
    from gcd_recursive
    order by b desc
    limit 1;
$$
language sql;

do $$
begin
    assert 1 = gcd(1, 1), 'gcd(1, 1) = 1';
    assert 5 = gcd(25, 5), 'gcd(25, 5) = 5';
    assert 1 = gcd(11, 13), 'gcd(11, 13) = 1';

    assert (select count(*) from nums where gcd(a, b) != gcd(b, a)) = 0, 'gcd(a, b) != gcd(b, a)';
end
$$
language plpgsql;

with gcds as (
    select
        a,
        b,
        gcd(a, b) as gcd
    from nums
)
select *
from gcds;
