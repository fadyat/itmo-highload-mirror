from datetime import datetime, timezone

from pyspark.sql import DataFrame, SparkSession
from pyspark.sql import functions as F
from pyspark.sql.column import Column

CITIES = [
    ("Moscow", "Russia", 55.7558, 37.6173),
    ("Saint Petersburg", "Russia", 59.9311, 30.3609),
    ("Novosibirsk", "Russia", 55.0084, 82.9357),
    ("Yekaterinburg", "Russia", 56.8389, 60.6057),
    ("Kazan", "Russia", 55.7887, 49.1221),
    ("Nizhny Novgorod", "Russia", 56.2965, 43.9361),
    ("Krasnodar", "Russia", 45.0355, 38.9753),
    ("Voronezh", "Russia", 51.6608, 39.2003),
    ("Samara", "Russia", 53.2415, 50.2212),
    ("Rostov-on-Don", "Russia", 47.2357, 39.7015),
]

FACILITY_NAMES = [
    "Теремок", "Чайхана", "Му-му", "Кофе Хауз", "Хлеб насущный",
    "Грабли", "Строганов", "Хачапури и вино", "Шашлычная", "Пельменная",
]

MENU = [
    ("Борщ", 350), ("Пельмени", 420), ("Блины с икрой", 450), ("Окрошка", 320), ("Солянка", 450),
    ("Винегрет", 250), ("Бефстроганов", 650), ("Сырники", 300), ("Ватрушка", 150), ("Квас", 120),
    ("Морс", 140), ("Компот", 130), ("Пирожки", 180), ("Уха", 520), ("Гречка с грибами", 380),
]

START_TS = int(datetime(2024, 1, 1, 0, 0, 0, tzinfo=timezone.utc).timestamp())
END_TS = int(datetime(2024, 12, 31, 23, 59, 59, tzinfo=timezone.utc).timestamp())

N_ORDERS = 300_000
OUTPUT_DIR = "/data/csv"

CORRUPTION_RATE = 0.10


def lit_array(values) -> Column:
    return F.array(*[F.lit(v) for v in values])


def corrupt(df: DataFrame, rate: float) -> DataFrame:
    """
    Corrupt approximately `rate` fraction of rows.

    Draws a uniform random `_c` in [0, 1) per row and splits the interval
    [0, rate) into four equal disjoint buckets, one per failure mode:
        bucket 0 -> city  := NULL
        bucket 1 -> price := NULL
        bucket 2 -> lat   := -999.999
        bucket 3 -> lng   := -999.999
    Each row falls into at most one bucket, so it gets at most one
    corruption. Rows with _c >= rate pass through unchanged.
    """

    bucket = rate / 4
    c = F.col("_c")

    def in_bucket(i: int) -> Column:
        return (c >= bucket * i) & (c < bucket * (i + 1))

    return (
        df
        .withColumn("_c", F.rand())
        .withColumn("city", F.when(in_bucket(0), F.lit(None).cast("string")).otherwise(F.col("city")))
        .withColumn("price", F.when(in_bucket(1), F.lit(None).cast("int")).otherwise(F.col("price")))
        .withColumn("lat", F.when(in_bucket(2), F.lit(-999.999)).otherwise(F.col("lat")))
        .withColumn("lng", F.when(in_bucket(3), F.lit(-999.999)).otherwise(F.col("lng")))
        .drop("_c")
    )


def main():
    spark = SparkSession.builder.appName("hw1-01-generate-csv").getOrCreate()

    orders = (
        spark
        .range(N_ORDERS)
        .withColumnRenamed("id", "order_id")
        .withColumn("city_idx", (F.rand() * len(CITIES)).cast("int"))
        .withColumn("name_idx", (F.rand() * len(FACILITY_NAMES)).cast("int"))
        .withColumn("lat_j", (F.rand() - F.lit(0.5)) * F.lit(0.05))
        .withColumn("lng_j", (F.rand() - F.lit(0.5)) * F.lit(0.05))
        .withColumn("datetime", F.from_unixtime(F.lit(START_TS) + (F.rand() * F.lit(END_TS - START_TS)).cast("long")))
        .withColumn("n_items", (F.rand() * F.lit(5)).cast("int") + F.lit(1))
    )

    items = (
        orders
        .withColumn("seq", F.explode(F.sequence(F.lit(1), F.col("n_items"))))
        .withColumn("menu_idx", (F.rand() * len(MENU)).cast("int"))
    )

    def at(arr: Column, idx: str) -> Column:
        return F.element_at(arr, F.col(idx) + 1)

    items = (
        items
        .withColumn("city", at(lit_array([c[0] for c in CITIES]), "city_idx"))
        .withColumn("country", at(lit_array([c[1] for c in CITIES]), "city_idx"))
        .withColumn("facility_name", at(lit_array(FACILITY_NAMES), "name_idx"))
        .withColumn("menu_item", at(lit_array([m[0] for m in MENU]), "menu_idx"))
        .withColumn("price", at(lit_array([m[1] for m in MENU]), "menu_idx"))
        .withColumn("lat", F.round(at(lit_array([c[2] for c in CITIES]), "city_idx") + F.col("lat_j"), 6))
        .withColumn("lng", F.round(at(lit_array([c[3] for c in CITIES]), "city_idx") + F.col("lng_j"), 6))
    )

    items = corrupt(items, CORRUPTION_RATE)

    result = items.select(
        "order_id", "city", "country", "facility_name",
        "lat", "lng", "menu_item", "price", "datetime",
    )

    result.write.mode("overwrite").option("header", "true").csv(OUTPUT_DIR)

    spark.stop()


if __name__ == "__main__":
    main()
