import os

from pyspark.sql import SparkSession
from pyspark.sql.types import StringType, StructField, StructType, TimestampType

DATA_DIR = "/data"
CHECKPOINT_DIR = "/data/_checkpoints/fs_to_es"
ES_NODES = os.environ["ES_NODES"]
ES_PORT = os.environ["ES_PORT"]
ES_INDEX = "users"

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
    spark = SparkSession.builder.appName("fs-to-es").getOrCreate()
    spark.sparkContext.setLogLevel("WARN")

    stream = (
        spark.readStream.schema(USER_SCHEMA)
        .option("maxFilesPerTrigger", 50)
        .option("pathGlobFilter", "users-*.jsonl")
        .json(DATA_DIR)
    )

    query = (
        stream.writeStream.format("org.elasticsearch.spark.sql")
        .option("es.nodes", ES_NODES)
        .option("es.port", ES_PORT)
        .option("es.resource", ES_INDEX)
        .option("es.mapping.id", "user_id")
        .option("checkpointLocation", CHECKPOINT_DIR)
        .outputMode("append")
        .trigger(processingTime="30 seconds")
        .start()
    )

    query.awaitTermination()


if __name__ == "__main__":
    main()
