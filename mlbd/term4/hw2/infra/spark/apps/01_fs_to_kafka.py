import os

from pyspark.sql import SparkSession
from pyspark.sql import functions as F
from pyspark.sql.types import StringType, StructField, StructType, TimestampType

DATA_DIR = "/data"
CHECKPOINT_DIR = "/data/_checkpoints/fs_to_kafka"
KAFKA_BROKERS = os.environ["KAFKA_BROKERS"]
TOPIC = "users"

USER_SCHEMA = StructType(
    [
        StructField("user_id", StringType(), nullable=False),
        StructField("name", StringType(), nullable=False),
        StructField("email", StringType(), nullable=False),
        StructField("country", StringType(), nullable=False),
        StructField("created_at", TimestampType(), nullable=False),
    ]
)


def main() -> None:
    spark = SparkSession.builder.appName("fs-to-kafka").getOrCreate()
    spark.sparkContext.setLogLevel("WARN")

    stream = (
        spark.readStream.schema(USER_SCHEMA)
        .option("maxFilesPerTrigger", 50)
        .option("pathGlobFilter", "users-*.jsonl")
        .json(DATA_DIR)
    )

    payload = stream.select(
        F.col("user_id").alias("key"),
        F.to_json(F.struct("*")).alias("value"),
    )

    query = (
        payload.writeStream.format("kafka")
        .option("kafka.bootstrap.servers", KAFKA_BROKERS)
        .option("topic", TOPIC)
        .option("checkpointLocation", CHECKPOINT_DIR)
        .outputMode("append")
        .trigger(processingTime="30 seconds")
        .start()
    )

    query.awaitTermination()


if __name__ == "__main__":
    main()
