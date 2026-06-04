from pyspark.sql import DataFrame, SparkSession
from pyspark.sql import functions as F
from pyspark.sql.types import (
    DoubleType,
    IntegerType,
    LongType,
    StringType,
    StructField,
    StructType,
    TimestampType,
)

INPUT_DIR       = "/data/csv"
DEADLETTER_DIR  = "/data/deadletter"
AVRO_DIR        = "/data/avro"
PARQUET_DIR     = "/data/parquet"

SCHEMA = StructType([
    StructField("order_id",      LongType(),      True),
    StructField("city",          StringType(),    True),
    StructField("country",       StringType(),    True),
    StructField("facility_name", StringType(),    True),
    StructField("lat",           DoubleType(),    True),
    StructField("lng",           DoubleType(),    True),
    StructField("menu_item",     StringType(),    True),
    StructField("price",         IntegerType(),   True),
    StructField("datetime",      TimestampType(), True),
])

# Russia is roughly between these bounds; anything outside (incl. -999.999)
# is treated as corrupted.
LAT_MIN, LAT_MAX = 41.0, 82.0
LNG_MIN, LNG_MAX = 19.0, 180.0


def is_corrupted() -> "F.Column":
    """Row is corrupted if any required field is NULL or coords are out of range."""
    null_any = (
        F.col("order_id").isNull()
        | F.col("city").isNull()
        | F.col("country").isNull()
        | F.col("facility_name").isNull()
        | F.col("lat").isNull()
        | F.col("lng").isNull()
        | F.col("menu_item").isNull()
        | F.col("price").isNull()
        | F.col("datetime").isNull()
    )
    bad_coords = (
        (F.col("lat") < LAT_MIN) | (F.col("lat") > LAT_MAX)
        | (F.col("lng") < LNG_MIN) | (F.col("lng") > LNG_MAX)
    )
    bad_price = F.col("price") <= 0
    return null_any | bad_coords | bad_price


def write_partitioned(df: DataFrame, fmt: str, path: str) -> None:
    (
        df.write
        .mode("overwrite")
        .partitionBy("date", "city")
        .format(fmt)
        .save(path)
    )


def main():
    spark = SparkSession.builder.appName("hw1-02-clean-partition").getOrCreate()

    raw = (
        spark.read
        .option("header", "true")
        .schema(SCHEMA)
        .csv(INPUT_DIR)
    )

    bad = raw.filter(is_corrupted())
    good = raw.filter(~is_corrupted()).withColumn("date", F.to_date("datetime"))

    bad.write.mode("overwrite").option("header", "true").csv(DEADLETTER_DIR)
    write_partitioned(good, "avro",    AVRO_DIR)
    write_partitioned(good, "parquet", PARQUET_DIR)

    spark.stop()


if __name__ == "__main__":
    main()
