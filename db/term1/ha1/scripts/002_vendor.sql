-- Найдем для каждого перевозчика:
--  * среднюю длину поездки
--  * дисперсию стоимости поездки
--  * количество поездок, начавшихся с 6 утра до 6 вечера
--  * сколько раз его водитель встречал Новый год в поездке
--  * максимальную стоимость поездки в среднем на пассажира

with
avg_ride_duration as (
    select vendor_id,
        avg(extract(epoch from taxi_rides.tpep_dropoff_datetime - taxi_rides.tpep_pickup_datetime)) as avg_trip_duration
    from taxi_rides
    group by vendor_id
),
ride_cost_variance as (
    select vendor_id,
        variance(total_amount) as trip_cost_variance
    from taxi_rides
    group by vendor_id
),
rides_during_work_hours as (
    select vendor_id,
        count(*) as trips_during_work_hours
    from taxi_rides
    where extract(hour from tpep_pickup_datetime) between 6 and 17
    group by vendor_id
),
rides_on_new_year as (
    select vendor_id,
        count(*) as trips_on_new_year
    from taxi_rides
    where extract(year from tpep_pickup_datetime) <
        extract(year from tpep_dropoff_datetime)
    group by vendor_id
),
max_ride_cost_per_passenger as (
    select vendor_id,
        max(fare_amount / passenger_count) as max_trip_cost_per_passenger
    from taxi_rides
    where passenger_count > 0
    group by vendor_id
)
select
    vendor_id,
    avg_trip_duration,
    trip_cost_variance,
    coalesce(trips_during_work_hours, 0) as trips_during_work_hours,
    coalesce(trips_on_new_year, 0)       as trips_on_new_year,
    max_trip_cost_per_passenger
from
    avg_ride_duration
    full join ride_cost_variance using (vendor_id)
    full join rides_during_work_hours using (vendor_id)
    full join rides_on_new_year using (vendor_id)
    full join max_ride_cost_per_passenger using (vendor_id)

