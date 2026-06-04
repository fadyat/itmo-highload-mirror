from pyspark.sql import SparkSession
from pyspark.sql import functions as F

PARQUET_DIR = "/data/parquet"
MONGO_URI   = "mongodb://mongo:27017"
DB          = "orders"
COLLECTION  = "orders"


def main():
    spark = (
        SparkSession.builder.appName("hw1-05-mongo")
        .config("spark.mongodb.write.connection.uri", MONGO_URI)
        .getOrCreate()
    )

    src = spark.read.parquet(PARQUET_DIR).withColumn(
        "price", (F.col("price") * F.lit(100)).cast("long"),
    )

    facility = F.struct(
        F.col("facility_name").alias("name"),
        "city", "country", "lat", "lng",
    )
    item = F.struct(F.col("menu_item").alias("name"), "price")

    orders = (
        src.groupBy("order_id")
        .agg(
            F.first(facility).alias("facility"),
            F.first("datetime").alias("ordered_at"),
            F.collect_list(item).alias("items"),
            F.sum("price").cast("long").alias("total"),
        )
        .withColumnRenamed("order_id", "_id")
    )

    (
        orders.write.format("mongodb")
        .option("database", DB)
        .option("collection", COLLECTION)
        .mode("append")
        .save()
    )

    spark.stop()


if __name__ == "__main__":
    main()
