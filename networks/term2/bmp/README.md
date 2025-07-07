# BMP

В этой задаче вам нужно будет реализовать мониторинг агента (в RFC monitoring station) для протокола [BMP (BGP monitoring protocol)](https://datatracker.ietf.org/doc/rfc7854/). Ваш мониторинг агент должен считать количество установленых BGP сессий у BPG спикера и реализовывать простой TCP сервер для отдачи статистики тестирующей системе.


Шаблон кода находится в файле bmp_client.py, ваша задача закончить его имплементацию.

## О среде
 
Как и в задачах про роутинг, ваш код будет запускаться в containerlab. Точная топология вам не важна, но можете прочитать `lab.yml` и `setup_agent.py`, если интересно.

## Про решение

bmp_client.py принимает следующие аргументы командной строки.

```
usage: bmp_client.py [-h] [--bmp-ip-addr BMP_IP_ADDR] [--bmp-port BMP_PORT] [--stats-server-ip STATS_SERVER_IP] [--stats-server-port STATS_SERVER_PORT] [--log-file LOG_FILE]

options:
  -h, --help            show this help message and exit
  --bmp-ip-addr BMP_IP_ADDR
                        IP address of monitored router
  --bmp-port BMP_PORT   TCP port of BMP server on monitored router
  --stats-server-ip STATS_SERVER_IP
                        IP address to bind stats server
  --stats-server-port STATS_SERVER_PORT
                        TCP port stats server should listen on
  --log-file LOG_FILE   File path for logging


```

`--bmp-ip-addr` и `--bmp-port` это ip адрес и порт с которыми нужно установить bmp сессию.
`--stats-server-ip` и `--stats-server-port` это ip и порт сервера статистики, с которым будет взаимодействовать тестирующая система.
`--log-file` это имя файла в который можно писать логи, эти логи будут доступны в выводе Gitlab CI. 

Сервер статистики должен слушать на ipv4 `--stats-server-ip` и tcp port `--stats-server-port`, и в каждое приходящее соедениние записывать одно число - количество установленных BGP пирингов у роутера в big endian.

Иначе говоря, `bmp_client.py` должен уметь обрабатывать BMP сообщения PEER UP и PEER DOWN, считать количество активных пиров и записывать это число в каждое TCP соединение установленное с сервером статистики.

## Технические детали

Чтобы сериализовать 4-х байтовое число в big endian, можно использовать `struct.pack('!I', count)`.

BMP сессия должна быть установалена при запуске bmp_client.py и переиспользоваться. Не нужно открывать новую BMP сессию на каждый запрос к stats server.
