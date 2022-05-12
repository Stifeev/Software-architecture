**Данная директория содержит исходный код ЛР4 по курсу программной архитектуры в МАИ**

Описание исходников:

- build - директория с собранными программами под Ubuntu 20.04.4 LTS;
  - server.exe - запуск сервера;
  - writer.exe - запуск обработчика сообщений от Apache Kafka;
  - tests.exe   - запуск юнит-тестов (сначала должен быть запущен server.exe и writer.exe);
- content - директория с кодом html-страниц;
- docker - сборка контейнеров с Apache Kafka;
- CMakeLists.txt - порядок компиляции, линковки и сборки;
- defs.hpp - подключение хедеров, макросы, конфиг, функции;
- server.cpp - реализация сервера;
- writer.cpp - реализация обработчика сообщений от Kafka;
- tests.cpp - реализация юнит-тестов.
- data_creation.sql - скрипт создания таблицы Person в MySql.

Порядок запуска ЛР:

Шаг 1: запуск контейнеров с Apache Kafka

```shell
$ cd docker
$ docker-compose up
```

Шаг 2: запуск сервера на Poco (с другой консоли)

```shell
$ cd build
$ ./server.exe --ip=<machine ip>              \
               --login=<login2database>       \
               --password=<password2database> \
               --sql_port=<sql port>          \
               --database=<db name>           \
               --read=<read server host>      \
               --write=<write server host>    \
               --queue=<queue host and port>  \
               --topic=<topic name>       
# Пример:
# $./server.exe   --ip=192.168.31.63             \
#                 --login=stud                   \
#                 --password=stud                \
#                 --sql_port=3306                \
#                 --database=Humanity            \
#                 --read=127.0.0.1               \
#                 --write=127.0.0.1              \
#                 --queue=127.0.0.1:9092         \
#                 --topic=event_server 
```

Шаг 3: запуск обработчика сообщений от Apache Kafka (с другой консоли)

```shell
$ cd build
$ ./writer.exe   --login=<login2database>       \
                 --password=<password2database> \
                 --sql_port=<sql port>          \
                 --database=<db_name>           \
                 --read=<read server host>      \
                 --write=<write server host>    \
                 --queue=<queue host and port>  \
                 --topic=<topic name>           \
                 --group_id=<writer group id>
# Пример:
# $./writer.exe   --login=stud                   \
#                 --password=stud                \
#				  --sql_port=3306                \
#				  --database=Humanity            \
#				  --read=127.0.0.1               \
#				  --write=127.0.0.1              \
#				  --queue=127.0.0.1:9092         \
#			      --topic=event_server           \
#				  --group_id=0 
```

Шаг 4: запуск тестов (с другой консоли)

```shell
$ cd build
$ ./tests.exe    --ip=<machine ip>              \
                 --login=<login2database>       \
                 --password=<password2database> \
                 --sql_port=<sql port>          \
                 --database=<db name>  
# Пример:
# $ ./tests.exe    --ip=192.168.31.63             \
#                  --login=stud                   \
#                  --password=stud                \
#                  --sql_port=3306                \
#                  --database=Humanity 
```

Если всё прошло хорошо, то вывод должен быть следующим:

```shell
[==========] Running 2 tests from 2 test suites.
[----------] Global test environment set-up.
[----------] 1 test from test_create
[ RUN      ] test_create.basic_test_set
[       OK ] test_create.basic_test_set (348 ms)
[----------] 1 test from test_create (348 ms total)

[----------] 1 test from test_add
[ RUN      ] test_add.basic_test_set
[       OK ] test_add.basic_test_set (4032 ms)
[----------] 1 test from test_add (4032 ms total)

[----------] Global test environment tear-down
[==========] 2 tests from 2 test suites ran. (4381 ms total)
[  PASSED  ] 2 tests.
```

Примеры запросов:

- GET http://192.168.31.83:8080/index.html
- GET http://192.168.31.83:8080/person?login=stif
- GET http://192.168.31.83:8080/person?first_name=E%25&last_name=%25v
- POST http://192.168.31.83:8080/person?login=pup&first_name=Vasya&last_name=Pupkin&age=30
