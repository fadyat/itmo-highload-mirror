# Лабораторная работа 2

[Google Drive](https://drive.google.com/drive/folders/16Xk6DTgTgCqjzua5a2spJca_vsZ7RRT0)

В рамках лабораторной работы необходимо реализовать платформу из приложений как изображено на картинке ниже

- Web API компонент, принимает 2 типа запросов: на чтение и запись
    - Чтение: Читает из PostgreSQL базы информацию, переданную в запросе, например поиск пользователя по id
    - Запись: Запись в PostgreSQL новых сущностей в соответствии с вариантом, и агрегирует некоторый набор записей и
      каждую минуту выгружает в новый файл в файловой системе
- PostgreSQL база данных, где создана таблица сущностей предметной области
- Локальная файловая система, сюда веб приложение будет 1 раз в мин выгружать сущности которые ей пришли на запрос
  записи
- Потоковое приложение Spark, реагирует на появление новых файлов в файловой системе и записывает их в Kafka topic (
  replication factor 3, partitions 10)
- Kafka Broker, хранит сообщения поступившие из spark streaming
- Kafka consumer group, читает данные из Kafka topic и записывает в Apache Cassandra (в качестве PRIMARY KEY выбрать
  что-то подходящее, например страну пользователя)
- Kafka consumer group, читает данные из Kafka topic и записывает в MongoDB
- Потоковое приложение Spark, реагирует на появление новых файлов в файловой системе и записывает их в Elasticsearch

## Архитектура

```
HTTP ─► Web API ─┬─► PostgreSQL
   (Go)          │
                 └─► /data/users-*.jsonl  (atomic write раз в 60s)
                              │
                ┌─────────────┴─────────────┐
                ▼                           ▼
        Spark Streaming           Spark Streaming
          (FS → Kafka)              (FS → ES)
                │                           │
                ▼                           ▼
        Kafka topic `users`           Elasticsearch
       (RF=3, partitions=10)            index `users`
                │
       ┌────────┴────────┐
       ▼ group=          ▼ group=
  cassandra-writer  mongo-writer
       │                 │
       ▼                 ▼
   Cassandra          MongoDB
 (PK=country,    (upsert по user_id)
  cluster=user_id)
```

---

Контрольные вопросы:

- Pub/Sub (Publish/Subscribe) и очереди (Queue)
- Семантики доставки (delivery semantics)
- Apache Kafka, Производитель, Потребитель, Тема, Брокеры
- Apache Kafka, Фактор репликации, Лидер раздела, Потребители и группы потребителей
- Kafka Connect
- Kafka Streams, KTables, KStream, SerDes
- Архитектура Kafka Streams
- Spark Structured Streaming
- NoSQL, типы баз данных: Ключ-значение, Документ, Широкий столбец, Графовые базы данных
- Каковы преимущества баз данных NoSQL?
- Теорема CAP
- Теорема PACELC
- ACID и BASE
- Сassandra, Модель данных Cassandra
- Сassandra, Распределение данных
- Сassandra, Согласованность данных
- Сassandra, Восстановление данных
- Сassandra, Запись на диск
- Сassandra, Уплотнение данных
- Сassandra, Операции удаления и Транзакционность
- Сassandra, Подсказки (Hints)

