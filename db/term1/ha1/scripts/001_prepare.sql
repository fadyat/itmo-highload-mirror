-- * Возьмем датасет такси Нью-Йорка:
-- * Поэтапно превратим его в SQL-таблицу.
-- * Первым делом создадим таблицу с текстовыми полями. По полю на каждый столбец.
-- * Прочитаем с помощью запроса COPY.
-- * Создадим новые поля с максимально адекватными типами, старые строковые удалим.
-- * Оформим последовательность действий в виде процедуры на PL/pgSQL.

create or replace function prepare_taxi_rides(
    file_name text,
    table_name text,
    columns_typed text[]
)
    returns void as
$$
declare
    column_name text;
    column_type text;
begin
    -- creating empty table without any columns
    execute format('create table %I()', table_name);

    -- reading column names and creating with text type
    for i in 1..array_length(columns_typed, 1) by 2
        loop
            column_name := columns_typed[i];
            execute format('alter table %I add column %I %s', table_name, column_name, 'text');
        end loop;

    -- copying data from file to table
    execute format('copy %I from %L delimiter '','' csv header', table_name, file_name);

    -- casting columns to correct types with removing old columns
    for i in 1..array_length(columns_typed, 1) by 2
        loop
            column_name := columns_typed[i];
            column_type := columns_typed[i + 1];
            execute format('alter table %I add column %I_v2 %s', table_name, column_name, column_type);
            execute format('update %I set %I_v2 = %I::%s', table_name, column_name, column_name, column_type);
            execute format('alter table %I drop column %I', table_name, column_name);
            execute format('alter table %I rename column %I_v2 to %I', table_name, column_name, column_name);
        end loop;
end;
$$
    language plpgsql;

select prepare_taxi_rides(
               '/tmp/head.csv',
               'taxi_rides',
               array [
                   'vendor_id', 'smallint',
                   'tpep_pickup_datetime', 'timestamp',
                   'tpep_dropoff_datetime', 'timestamp',
                   'passenger_count', 'smallint',
                   'trip_distance', 'real',
                   'rate_code_id', 'smallint',
                   'store_and_fwd_flag', 'char(1)',
                   'pu_location_id', 'smallint',
                   'do_location_id', 'smallint',
                   'payment_type', 'smallint',
                   'fare_amount', 'real',
                   'extra', 'real',
                   'mta_tax', 'real',
                   'tip_amount', 'real',
                   'tolls_amount', 'real',
                   'improvement_surcharge', 'real',
                   'total_amount', 'real'
                   ]
       );
