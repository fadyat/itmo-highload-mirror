-- Найдем для каждого перевозчика:
--  * среднюю длину поездки
--  * дисперсию стоимости поездки
--  * количество поездок, начавшихся с 6 утра до 6 вечера
--  * сколько раз его водитель встречал Новый год в поездке
--  * максимальную стоимость поездки в среднем на пассажира

select
    vendor_id,
    avg(trip_distance) as avg_trip_distance,
    variance(total_amount) as trip_cost_variance,
    count(*) filter ( where extract(hour from tpep_pickup_datetime) between 6 and 17 ) as trips_during_work_hours,
    count(*) filter ( where extract(year from tpep_pickup_datetime) < extract(year from tpep_dropoff_datetime) ) as trips_on_new_year,
    max(fare_amount / passenger_count) filter ( where passenger_count > 0) as max_trip_cost_per_passenger
from taxi_rides
group by vendor_id
order by vendor_id;