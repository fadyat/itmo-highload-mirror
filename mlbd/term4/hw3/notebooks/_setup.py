"""Shared helpers for hw3 notebooks: spark factory + evaluation loop."""

from __future__ import annotations

import os
from collections.abc import Mapping
from pathlib import Path

import pandas as pd
from pyspark.ml.evaluation import Evaluator
from pyspark.sql import SparkSession


def make_spark(app_name: str, *, partitions: int = 4, memory: str = "2g") -> SparkSession:
    os.environ["SPARK_LOCAL_IP"] = "127.0.0.1"
    os.environ["SPARK_CONF_DIR"] = str((Path.cwd() / ".." / "conf").resolve())

    spark = (
        SparkSession.builder.appName(app_name)
        .master("local[*]")
        .config("spark.sql.shuffle.partitions", str(partitions))
        .config("spark.driver.memory", memory)
        .config("spark.driver.host", "127.0.0.1")
        .config("spark.driver.bindAddress", "127.0.0.1")
        .getOrCreate()
    )
    spark.sparkContext.setLogLevel("ERROR")
    return spark


def evaluate_models(
    models: Mapping[str, object],
    train,
    test,
    evaluators: Mapping[str, Evaluator],
) -> pd.DataFrame:
    """Fit each model on train, transform test, return DataFrame[model -> metrics]."""
    rows = []
    for name, est in models.items():
        fitted = est.fit(train)
        pred = fitted.transform(test)
        rows.append({"model": name, **{metric: ev.evaluate(pred) for metric, ev in evaluators.items()}})
    return pd.DataFrame(rows).set_index("model")
