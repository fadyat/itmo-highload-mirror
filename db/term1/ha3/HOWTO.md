В решении используется комбинация кода на `Go` и `mongosh`.
`mongosh` запускается отдельным процессом из кода на `Go` и используется для read-only операций.
Выбрал `mongosh`, так как на JS писать запросы к `MongoDB` удобнее и читабельнее.

### Запуск

Зависимости можно поставить при помощи `go mod tidy`.

```md
$ go run main.go

Usage:
  avfadeev [command]

Available Commands:
  1_setup       Downloads required data and stores it in the database
  3_actors      Count actors related statistics
  4_user_events Count user events related statistics
  5_topk        Search top K repositories by open issues
  6_reverse     Find some interesting pairs
  7_periods     Non-decreasing periods calculation
  8_domains     Calculate number of repositories per 2nd level domain
```

Список запускаемых скриптов можно посмотреть в [директории](./cmd/scripts).
Параметры пользователя прокидываются при помощи `eval` при вызове `mongosh`.

Запускать обязательно в корне проекта, так как используется относительный путь к скриптам.

При запуске любой из команд, можно переопределять значения коллекций, параметров подключения и запросов:

```md
$ go run main.go 7_periods --help
Non-decreasing periods calculation

Usage:
  avfadeev 7_periods [flags]

Flags:
  -c, --collection string   Input collection name (default "periods")
  -d, --database string     Input database name (default "test")
  -u, --url string          MongoDB server URL (default "mongodb://admin:admin@localhost:27017")
```

### Пример

```md
$ go run main.go 7_periods
test.periods
[
  {
    id: 1,
    a: 2,
    period: 1,
    with_next_avg_a: 3.75,
    a_diff: -1.75
  },
  {
    id: 2,
    a: 3,
    period: 1,
    with_next_avg_a: 3.75,
    a_diff: -0.75
  },
  {
    id: 3,
    a: 5,
    period: 2,
    with_next_avg_a: 7.333333333333333,
    a_diff: -2.333333333333333
  },
  {
    a: 5,
    id: 4,
    period: 2,
    with_next_avg_a: 7.333333333333333,
    a_diff: -2.333333333333333
  },
  {
    id: 5,
    a: 7,
    period: 3,
    with_next_avg_a: 9.2,
    a_diff: -2.1999999999999993
  },
  {
    id: 6,
    a: 8,
    period: 3,
    with_next_avg_a: 9.2,
    a_diff: -1.1999999999999993
  },
  {
    a: 9,
    id: 7,
    period: 3,
    with_next_avg_a: 9.2,
    a_diff: -0.1999999999999993
  },
  {
    id: 8,
    a: 10,
    period: 3,
    with_next_avg_a: 9.2,
    a_diff: 0.8000000000000007
  },
  {
    a: 12,
    id: 9,
    period: 4,
    with_next_avg_a: 16,
    a_diff: -4
  },
  {
    a: 20,
    id: 10,
    period: 5,
    with_next_avg_a: 23.666666666666668,
    a_diff: -3.666666666666668
  },
  {
    id: 11,
    a: 25,
    period: 6,
    with_next_avg_a: 25.5,
    a_diff: -0.5
  },
  {
    id: 12,
    a: 26,
    period: 6,
    with_next_avg_a: 25.5,
    a_diff: 0.5
  }
] 
```