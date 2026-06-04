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
    spark = SparkSession.builder.appName("hw1-04-postgres-star").getOrCreate()

    src = spark.read.format("avro").load(AVRO_DIR)

    dim_facility = (
        src.groupBy("facility_name", "city", "country")
        .agg(F.first("lat").alias("lat"), F.first("lng").alias("lng"))
        .select("facility_name", "city", "country", "lat", "lng")
    )
    write(dim_facility, "star.dim_facility")
    facility_db = read(spark, "star.dim_facility")

    dim_menu_item = (
        src.groupBy("menu_item")
        .agg(F.first("price").alias("price"))
        .select(
            F.col("menu_item").alias("name"),
            (F.col("price") * F.lit(100)).cast("long").alias("price"),
        )
    )
    write(dim_menu_item, "star.dim_menu_item")
    menu_db = read(spark, "star.dim_menu_item")

    d = F.to_date("datetime")
    dim_date = (
        src.select(d.alias("date_value")).distinct()
        .select(
            (F.year("date_value") * F.lit(10000) + F.month("date_value") * F.lit(100) + F.dayofmonth("date_value"))
            .cast("int").alias("date_sk"),
            "date_value",
            F.year("date_value").alias("year"),
            F.month("date_value").alias("month"),
            F.dayofmonth("date_value").alias("day"),
            F.dayofweek("date_value").alias("day_of_week"),
        )
    )
    write(dim_date, "star.dim_date")

    s = src.alias("s")
    f = facility_db.alias("f")
    m = menu_db.alias("m")

    fact = (
        s
        .join(f, (F.col("s.facility_name") == F.col("f.facility_name")) & (F.col("s.city") == F.col("f.city")))
        .join(m, F.col("s.menu_item") == F.col("m.name"))
        .select(
            F.col("s.order_id"),
            F.col("f.facility_sk"),
            F.col("m.menu_item_sk"),
            (F.year(d) * F.lit(10000) + F.month(d) * F.lit(100) + F.dayofmonth(d)).cast("int").alias("date_sk"),
            F.col("s.datetime").alias("ordered_at"),
            (F.col("s.price") * F.lit(100)).cast("long").alias("price"),
        )
    )
    write(fact, "star.fact_order_items")

    spark.stop()


if __name__ == "__main__":
    main()
