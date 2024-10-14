-- Если мысленно упорядочить записи по неубывнию n, то можно выделить периоды плавного неубывания.
-- Период плавного неубывания - это участок, на котором n не растет или растет на 1.
-- Иногда период плавного неубывания может иметь длину 1.
--
-- Напишем запрос, который для каждого id покажет разницу между его показателем `a` и средним `a`
-- по его и следующему периоду плавного неубывания.

create table if not exists data (
    id int primary key,
    n int not null,
    a int not null
);

insert into data
values
    (1, 2, 2), (2, 3, 3),
    (3, 5, 5), (4, 5, 5),
    (5, 7, 7), (6, 8, 8), (7, 9, 9), (8, 10, 10),
    (9, 12, 12),
    (10, 20, 20),
    (11, 25, 25), (12, 26, 26);

with __period_markers as (
    select
        id,
        n,
        a,
        case when n - lag(n) over (order by n) in (0, 1)
            then 0 else 1
        end as period_marker
    from data
),
periods as (
    select
        id,
        n,
        a,
        sum(period_marker) over (order by n) as period
    from __period_markers
),
periods_avg_with_next as (
    select
        id,
        avg(a) over current_and_next_periods as with_next_avg_a
    from periods
    window current_and_next_periods as (
        order by period
        range between current row and 1 following
    )
)
select
    p.id,
    p.period,
    p.a,
    pa.with_next_avg_a,
    p.a - pa.with_next_avg_a as a_diff
from periods as p
join periods_avg_with_next as pa using (id);