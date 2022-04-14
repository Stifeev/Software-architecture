**Данная директория содержит исходный код ЛР2 по курсу программной архитектуры в МАИ**

Описание исходников:

- benchmark - результаты тестирования утилитой wrk и python-скрипт, строящий графики;

- build - директория с собранными программами под Ubuntu 20.04.4 LTS;
  - server.exe - запуск сервера
    - ./server.exe --ip=<ip вашей машины>
  - tests.exe - запуск юнит-тестов (сначала должен быть запущен server.exe);
    - ./tests.exe --ip=<ip вашей машины>
- content - директория с кодом html-страниц;
- docker - сборка контейнеров и кластера с proxysql;
- CMakeLists.txt - порядок компиляции, линковки и сборки;
- defs.hpp - подключение хедеров, макросы, конфиг, функции;
- server.cpp - реализация сервера;
- tests.cpp - реализация юнит-тестов.
- data_creation.sql - скрипт создания таблицы Person в MySql.

Порядок запуска ЛР:

Шаг 1: запуск контейнерного-кластера с тремя базами и proxysql

```shell
$ cd docker
$ docker-compose up
```

Шаг 2: запуск сервера на Poco (с другой консоли)

```shell
$ cd build
$ ./server.exe --ip=<ip вашей машины>
```

Шаг 3: запуск тестов (с другой консоли)

```shell
$ cd build
$ ./tests.exe --ip=<ip вашей машины>
```

Если всё прошло хорошо, то вывод должен быть следующим:

```shell
[==========] Running 2 tests from 2 test suites.
[----------] Global test environment set-up.
[----------] 1 test from test_create
[ RUN      ] test_create.basic_test_set
[       OK ] test_create.basic_test_set (154 ms)
[----------] 1 test from test_create (154 ms total)

[----------] 1 test from test_add
[ RUN      ] test_add.basic_test_set
[       OK ] test_add.basic_test_set (4011 ms)
[----------] 1 test from test_add (4011 ms total)

[----------] Global test environment tear-down
[==========] 2 tests from 2 test suites ran. (4165 ms total)
[  PASSED  ] 2 tests.
```

Примеры запросов:

- GET http://192.168.31.83:8080/index.html
- GET http://192.168.31.83:8080/person?login=stif
- GET http://192.168.31.83:8080/person?first_name=E%25&last_name=%25v
- POST http://192.168.31.83:8080/person?add&login=pup&first_name=Vasya&last_name=Pupkin&age=30

Графики производительности:

![12](https://github.com/Stifeev/Software-architecture/blob/main/ЛР2/benchmark/im1.png?raw=true)

![](benchmark\im2.png)
