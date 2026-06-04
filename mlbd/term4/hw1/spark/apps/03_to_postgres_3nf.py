from pyspark.sql import DataFrame, SparkSession
from pyspark.sql import functions as F

JDBC_URL = "jdbc:postgresql://postgres:5432/orders"
JDBC_PROPS = {
    "user": "admin",
    "password": "admin",
    "driver": "org.postgresql.Driver",
}

AVRO_DIR = "/data/avro"


def write(df: DataFrame, table: str) -> None:
    df.write.jdbc(url=JDBC_URL, table=table, mode="append", properties=JDBC_PROPS)


def read(spark: SparkSession, table: str) -> DataFrame:
    return spark.read.jdbc(url=JDBC_URL, table=table, properties=JDBC_PROPS)


def main():
    spark = SparkSession.builder.appName("hw1-03-postgres-3nf").getOrCreate()

    src = spark.read.format("avro").load(AVRO_DIR)

    countries = src.select(F.col("country").alias("country_name")).distinct()
    write(countries, "norm.countries")
    countries_db = read(spark, "norm.countries")

    cities = (
        src.select("city", "country").distinct()
        .join(countries_db, F.col("country") == F.col("country_name"))
        .select(F.col("city").alias("city_name"), "country_id")
    )
    write(cities, "norm.cities")
    cities_db = read(spark, "norm.cities")

    menu = (
        src.groupBy("menu_item")
        .agg(F.first("price").alias("price"))
        .select(
            F.col("menu_item").alias("name"),
            (F.col("price") * F.lit(100)).cast("long").alias("price"),
        )
    )
    write(menu, "norm.menu_items")
    menu_db = read(spark, "norm.menu_items")

    facilities = (
        src.groupBy("facility_name", "city")
        .agg(F.first("lat").alias("lat"), F.first("lng").alias("lng"))
        .join(cities_db, F.col("city") == F.col("city_name"))
        .select("facility_name", "city_id", "lat", "lng")
    )
    write(facilities, "norm.facilities")
    facilities_db = read(spark, "norm.facilities")

    facilities_with_city = facilities_db.join(cities_db, "city_id").select("facility_id", "facility_name", "city_name")

    orders = (
        src.select(
            "order_id",
            "facility_name",
            F.col("city").alias("city_name"),
            F.col("datetime").alias("ordered_at"),
        )
        .dropDuplicates(["order_id"])
        .join(facilities_with_city, ["facility_name", "city_name"])
        .select("order_id", "facility_id", "ordered_at")
    )
    write(orders, "norm.orders")

    s, m = src.alias("s"), menu_db.alias("m")
    order_items = (
        s.join(m, F.col("s.menu_item") == F.col("m.name"))
        .select(
            F.col("s.order_id"),
            F.col("m.menu_item_id"),
            (F.col("s.price") * F.lit(100)).cast("long").alias("price"),
        )
    )
    write(order_items, "norm.order_items")

    spark.stop()


if __name__ == "__main__":
    main()
