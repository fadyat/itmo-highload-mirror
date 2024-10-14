-- Найдем для каждой поездки
--
--  * долю чаевых в расходах, понесенных пассажиром
--  * разницу между долей чаевых этой поездки и средней долей по всем поездкам в датасете
--  * разницу между долей чаевых этой поездки и средней долей по всем поездкам данного перевозчика
--  * разницу между долей чаевых этой поездки и средней долей по всем более дальним поездкам, начавшимся в тот же день
--  * ранг данной поездки по средней скорости у данного переводчика

with
__ride_tips_share as (
    select
        *,
        row_number() over ()                                                           as ride_id,
        tip_amount / nullif(total_amount, 0) / coalesce(nullif(passenger_count, 0), 1) as tips_share_per_passenger
    from taxi_rides
),
tips_share as (
    select
        ride_id,
        tips_share_per_passenger
    from __ride_tips_share
),
tips_share_avg_diff as (
    select
        ride_id,
        tips_share_per_passenger,
        tips_share_per_passenger - avg(tips_share_per_passenger) over () as tips_share_avg_diff
    from __ride_tips_share
),
tips_share_vendor_avg_diff as (
    select
        ride_id,
        tips_share_per_passenger,
        tips_share_per_passenger - avg(tips_share_per_passenger) over (partition by vendor_id) as tips_share_vendor_avg_diff
    from __ride_tips_share
),
tips_share_longer_rides_avg_diff as (
    select
        ride_id,
        tips_share_per_passenger,
        avg(tips_share_per_passenger) over rows_with_longer_rides                            as tips_share_with_longer_rides_avg,
        tips_share_per_passenger - avg(tips_share_per_passenger) over rows_with_longer_rides as tips_share_longer_rides_avg_diff
    from (
        select *,
            date_trunc('day', tpep_pickup_datetime) as tpep_pickup_down_to_day
        from __ride_tips_share
    ) as q
    window rows_with_longer_rides as (
        partition by tpep_pickup_down_to_day
        order by trip_distance
        rows between current row and unbounded following
    )
),
ride_rank_by_speed_per_vendor as (
    select
        row_number() over ()                                                             as ride_id,
        rank() over rank_by_speed                                                        as ride_rank_by_speed_per_vendor,
        trip_distance / extract(epoch from tpep_dropoff_datetime - tpep_pickup_datetime) as speed
    from taxi_rides
    where tpep_dropoff_datetime > tpep_pickup_datetime
    window rank_by_speed as (
        partition by vendor_id
        order by trip_distance / extract(epoch from tpep_dropoff_datetime - tpep_pickup_datetime) desc
    )
)
select
    ride_id,
    tips_share.tips_share_per_passenger,
    tips_share_avg_diff,
    tips_share_vendor_avg_diff,
    tips_share_longer_rides_avg_diff,
    ride_rank_by_speed_per_vendor
from
    tips_share
    full join tips_share_avg_diff using (ride_id)
    full join tips_share_vendor_avg_diff using (ride_id)
    full join tips_share_longer_rides_avg_diff using (ride_id)
    full join ride_rank_by_speed_per_vendor using (ride_id)
