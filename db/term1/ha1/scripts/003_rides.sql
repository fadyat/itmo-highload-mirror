-- Найдем для каждой поездки
--
--  * долю чаевых в расходах, понесенных пассажиром
--  * разницу между долей чаевых этой поездки и средней долей по всем поездкам в датасете
--  * разницу между долей чаевых этой поездки и средней долей по всем поездкам данного перевозчика
--  * разницу между долей чаевых этой поездки и средней долей по всем более дальним поездкам, начавшимся в тот же день
--  * ранг данной поездки по средней скорости у данного переводчика

select
    ride_id,
    vendor_id,
    tips_share_per_passenger,
    tips_share_per_passenger - avg(tips_share_per_passenger) over () as tips_share_avg_diff,
    tips_share_per_passenger - avg(tips_share_per_passenger) over ( partition by vendor_id ) as tips_share_vendor_avg_diff,
    tips_share_per_passenger - avg(tips_share_per_passenger) over (
        partition by date_trunc('day', tpep_pickup_datetime)
        order by trip_distance
        rows between current row and unbounded following
    ) as tips_share_longer_rides_avg_diff,
    rank() over (
        partition by vendor_id
        order by (trip_distance / extract(epoch from tpep_dropoff_datetime - tpep_pickup_datetime)) desc
    ) as ride_rank_by_speed_per_vendor
from (
    select
        *,
        row_number() over () as ride_id,
        tip_amount / nullif(total_amount, 0) / coalesce(nullif(passenger_count, 0), 1) as tips_share_per_passenger
    from taxi_rides
    where tpep_dropoff_datetime > tpep_pickup_datetime
) as q
order by ride_id;