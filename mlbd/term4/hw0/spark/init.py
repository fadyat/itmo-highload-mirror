from pyspark.sql import SparkSession

spark = SparkSession.builder.appName("hw0-init").getOrCreate()

base = "/opt/spark/delta"


def save(name, schema, rows):
    df = spark.createDataFrame(rows, schema)
    df.write.format("delta").mode("overwrite").save(f"{base}/{name}")


save(
    "customers",
    ["id", "full_name", "city", "registration_date"],
    [
        (1, "Иванов Иван",     "Москва",          "2021-03-10"),
        (2, "Петрова Мария",   "Санкт-Петербург", "2021-06-15"),
        (3, "Сидоров Алексей", "Казань",          "2022-01-20"),
        (4, "Козлова Анна",    "Москва",          "2022-05-11"),
        (5, "Новиков Дмитрий", "Новосибирск",     "2023-02-28"),
        (6, "Морозова Елена",  "Санкт-Петербург", "2023-07-04"),
    ],
)

save(
    "branches",
    ["id", "branch_name", "city"],
    [
        (1, "Центральный", "Москва"),
        (2, "Северный",    "Санкт-Петербург"),
        (3, "Восточный",   "Казань"),
    ],
)

save(
    "accounts",
    ["id", "customer_id", "account_type", "balance", "branch_id"],
    [
        (1, 1, "checking", 150000.0, 1),
        (2, 1, "savings",  500000.0, 1),
        (3, 2, "checking",  80000.0, 2),
        (4, 3, "savings",  200000.0, 3),
        (5, 3, "checking",  45000.0, 3),
        (6, 4, "savings",  320000.0, 1),
        (7, 5, "checking",  60000.0, 2),
        (8, 6, "savings",  110000.0, 2),
        (9, 6, "checking",  25000.0, 2),
    ],
)

save(
    "transactions",
    ["id", "account_id", "amount", "transaction_date", "transaction_type"],
    [
        (1, 1,  50000.0, "2024-10-01", "deposit"),
        (2, 1, -20000.0, "2024-10-05", "withdrawal"),
        (3, 2, 100000.0, "2024-10-10", "deposit"),
        (4, 3, -15000.0, "2024-11-01", "withdrawal"),
        (5, 4,  30000.0, "2024-11-15", "deposit"),
        (6, 5,  -5000.0, "2024-12-01", "withdrawal"),
        (7, 6,  80000.0, "2024-12-10", "deposit"),
    ],
)

save(
    "loans",
    ["id", "customer_id", "loan_amount", "interest_rate", "start_date", "status"],
    [
        (1, 1, 1000000.0, 12.5, "2023-01-15", "active"),
        (2, 2,  500000.0, 10.0, "2023-06-20", "active"),
        (3, 3,  300000.0, 14.0, "2024-02-10", "closed"),
        (4, 5,  750000.0, 11.5, "2024-05-01", "active"),
    ],
)

save(
    "employees",
    ["id", "full_name", "branch_id", "position", "salary"],
    [
        (1, "Кузнецов Сергей",  1, "manager", 120000.0),
        (2, "Смирнова Ольга",   1, "teller",   65000.0),
        (3, "Попов Виктор",     2, "manager", 115000.0),
        (4, "Федорова Наталья", 2, "teller",   60000.0),
        (5, "Орлов Андрей",     3, "manager", 110000.0),
    ],
)

spark.stop()
