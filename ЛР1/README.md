**Данная директория содержит исходный код ЛР1 по курсу программной архитектуры в МАИ**

Описание исходников:

- build - директория с собранными программами под Ubuntu 20.04.4 LTS;
  - server.exe - запуск сервера (конфиг захардкоден в defs.hpp);
  - tests.exe - запуск юнит-тестов (сначала должен быть запущен server.exe);
- content - директория с кодом html-страниц;
- CMakeLists.txt - порядок компиляции, линковки и сборки;
- defs.hpp - подключение хедеров, макросы, конфиг;
- server.cpp - реализация сервера;
- tests.cpp - реализация юнит-тестов.
- data_creation.sql - скрипт создания таблицы Person в MySql.

Чтобы запуск прошёл успешно, необходима предварительно созданная mysql-база данных с именем Humanity на вашей системе с таблицей Person в ней:

$ cd Код

$ mysql -u <ЛОГИН> -p

$ create database Humanity;

$ use Humanity;

$ source data_creation.sql;

$ quit;

Также не забудьте настроить конфиг под себя в файле defs.hpp (namespace Config).

Примеры запросов:

- GET http://192.168.31.83:8080/index.html
- GET http://192.168.31.83:8080/person?login=stif
- GET http://192.168.31.83:8080/person?first_name=E%25&last_name=%25v
- POST http://192.168.31.83:8080/person?add&login=pup&first_name=Vasya&last_name=Pupkin&age=30

