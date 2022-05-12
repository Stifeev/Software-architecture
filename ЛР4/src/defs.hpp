#ifndef DEFS_HPP
#define DEFS_HPP

#include <stdio.h>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <exception>
#include <memory>
#include <functional>
#include <thread>
#include <sstream>
#include <map> 
#include <algorithm>

// для sleep
#ifdef _WIN32
#include <Windows.h> 
#else
#include <unistd.h>
#endif

#include "Poco/Thread.h"
#include "Poco/Runnable.h"

#include <Poco/Data/MySQL/Connector.h>
#include <Poco/Data/MySQL/MySQLException.h>
#include <Poco/Data/Session.h>
#include <Poco/Data/SessionFactory.h>
#include <Poco/Data/RecordSet.h>
#include <Poco/Data/Statement.h>

#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Net/SocketStream.h"
#include "Poco/Net/HTMLForm.h"

#include "Poco/Timestamp.h"
#include "Poco/DateTimeFormatter.h"
#include "Poco/DateTimeFormat.h"
#include "Poco/Exception.h"
#include "Poco/ThreadPool.h"
#include "Poco/StreamCopier.h"

#include "Poco/JSON/Object.h"
#include "Poco/JSON/Parser.h"

#include "Poco/Dynamic/Var.h"

#include "Poco/Util/ServerApplication.h"
#include "Poco/Util/Option.h"
#include "Poco/Util/OptionSet.h"
#include "Poco/Util/HelpFormatter.h"

#include <mutex>
#include <csignal>
#include <cppkafka/cppkafka.h>
#include <cppkafka/consumer.h>
#include <cppkafka/configuration.h>

using std::cout;
using std::endl;
using std::string;
using std::wstring;
using std::vector;
using std::unique_ptr;
using std::thread;
#define STR std::to_string

using Poco::DateTimeFormat;
using Poco::DateTimeFormatter;
using Poco::ThreadPool;
using Poco::Timestamp;
using Poco::Net::HTTPRequestHandler;
using Poco::Net::HTTPRequestHandlerFactory;
using Poco::Net::HTTPServer;
using Poco::Net::HTTPServerParams;
using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPServerResponse;
using Poco::Net::ServerSocket;

using Poco::Util::Application;
using Poco::Util::HelpFormatter;
using Poco::Util::Option;
using Poco::Util::OptionCallback;
using Poco::Util::OptionSet;
using Poco::Util::ServerApplication;

#define Keywords Poco::Data::Keywords
using Poco::Data::Statement;
using SqlSession = Poco::Data::Session;

namespace Config // глобальные переменные
{
    string host     = "127.0.0.1",
           login    = "",
           password = "",
           database = "",
           ip       = "",
           read     = "",
           write    = "",
           queue    = "",
           topic    = "";

    int port = 8080; // порт сервера
    int sql_port = -1;
    int group_id = -1;
}

#define SQL_HANDLE(...)                                   \
try                                                       \
{                                                         \
    __VA_ARGS__                                           \
}                                                         \
catch (Poco::Data::MySQL::ConnectionException &e)         \
{                                                         \
    cout << "connection ERROR:" << e.what() << endl;      \
}                                                         \
catch (Poco::Data::MySQL::StatementException &e)          \
{                                                         \
    cout << "statement ERROR:" << e.what() << endl;       \
}

struct Person
{   
    string login;
    string first_name;
    string last_name;
    int age;
};

SqlSession *create_SQL_session()  // Создаём сессию с базой данных
{
    string connection_string = "host=" + Config::host +           
                               ";user=" +  Config::login + 
                               ";db=" +  Config::database + 
                               ";password=" +  Config::password +
                               ";port=" + STR(Config::sql_port);

    Poco::Data::MySQL::Connector::registerConnector();
    SqlSession *session_ptr = NULL;
    SQL_HANDLE(
        session_ptr = new Poco::Data::Session(Poco::Data::SessionFactory::instance().create(
            Poco::Data::MySQL::Connector::KEY, connection_string));
    )
    return session_ptr;
}

/* ================= Общие функции ================= */

bool starts_with(const string &str, const string &prefix)
{
    if(prefix.size() > str.size())
        return false;

    int i;
    for(i = 0; i < prefix.size(); i++)
    {
        if(str[i] != prefix[i])
        {
            return false;
        }
    }
    return true;
}

/* ================= Разбор входных параметров ================= */

string operator *(const string &a, int n)
{
    string res(a.size() * n, ' ');
    int i;
    for(i = 0; i < n; i++)
    {
        std::copy(a.begin(), a.end(), res.begin() + i * a.size());
    }
    return res;
}

#define FORMAT "%-12s %-30s"

static string scheme_server = 
string("usage: ./" FORMAT " \\\n") +
string("         " FORMAT " \\\n") * 7 + 
string("         " FORMAT "   \n") + 
string("example: ./" FORMAT " \\\n") +
string("           " FORMAT " \\\n") * 7 +
string("           " FORMAT "   \n");

static string scheme_writer = 
string("usage: ./" FORMAT " \\\n") +
string("         " FORMAT " \\\n") * 7 + 
string("         " FORMAT "   \n") + 
string("example: ./" FORMAT " \\\n") +
string("           " FORMAT " \\\n") * 7 +
string("           " FORMAT "   \n");

static string scheme_tests = 
string("usage: ./" FORMAT " \\\n") +
string("         " FORMAT " \\\n") * 3 + 
string("         " FORMAT "   \n") + 
string("example: ./" FORMAT " \\\n") +
string("           " FORMAT " \\\n") * 3 +
string("           " FORMAT "   \n");

inline string get_description(const string &exe_name)
{
    char res[2000];

    if(starts_with(exe_name, "server"))
    {
        // 9 аргументов
        sprintf(res, scheme_server.c_str(),
                exe_name.c_str(), "--ip=<machine ip>",
                " ", "--login=<login2database>",
                " ", "--password=<password2database>",
                " ", "--sql_port=<sql port>",
                " ", "--database=<db name>",
                " ", "--read=<read server host>",
                " ", "--write=<write server host>",
                " ", "--queue=<queue host and port>",
                " ", "--topic=<topic name>",
                exe_name.c_str(), "--ip=192.168.31.63",
                " ", "--login=stud",
                " ", "--password=stud",
                " ", "--sql_port=3306",
                " ", "--database=Humanity",
                " ", "--read=127.0.0.1",
                " ", "--write=127.0.0.1",
                " ", "--queue=127.0.0.1:9092",
                " ", "--topic=event_server");
    }
    else if(starts_with(exe_name, "writer"))
    {
        // 9 аргументов
        sprintf(res, scheme_writer.c_str(),
                exe_name.c_str(), "--login=<login2database>",
                " ", "--password=<password2database>",
                " ", "--sql_port=<sql port>",
                " ", "--database=<db_name>",
                " ", "--read=<read server host>",
                " ", "--write=<write server host>",
                " ", "--queue=<queue host and port>",
                " ", "--topic=<topic name>",
                " ", "--group_id=<writer group id>",
                exe_name.c_str(), "--login=stud",
                " ", "--password=stud",
                " ", "--sql_port=3306",
                " ", "--database=Humanity",
                " ", "--read=127.0.0.1",
                " ", "--write=127.0.0.1",
                " ", "--queue=127.0.0.1:9092",
                " ", "--topic=event_server",
                " ", "--group_id=0");
    }
    else if(starts_with(exe_name, "tests"))
    {
        // 5 аргументов
        sprintf(res, scheme_tests.c_str(),
                exe_name.c_str(), "--ip=<machine ip>",
                " ", "--login=<login2database>",
                " ", "--password=<password2database>",
                " ", "--sql_port=<sql port>",
                " ", "--database=<db name>",
                exe_name.c_str(), "--ip=192.168.31.63",
                " ", "--login=stud",
                " ", "--password=stud",
                " ", "--sql_port=3306",
                " ", "--database=Humanity");
    }
    
    return string(res);
}

std::map<string, string> argv2map(int argc, char *argv[], const string &desc)
{
    int i;
    std::map<string, string> args;
    for(i = 1; i < argc; i++)
    {
        int j;
        string arg(argv[i]);
        j = std::find(arg.begin(), arg.end(), '=') - arg.begin();
        if(j == arg.size())
        {
            cout << "ERROR in argc[" + STR(i) + "]" << endl;
            cout << desc << endl;
            return args;
        }
        args[arg.substr(0, j)] = arg.substr(j + 1, arg.size() - j);
    }
    return args;
}

std::map<string, string> argv2map(int argc, char *argv[])
{
    string desc = "";
    return argv2map(argc, argv, desc);
}

#define CHECK_ARG(NAME, ...)                               \
    if(args.find("--" #NAME) == args.end())                \
    {                                                      \
        cout << "ERROR: not find --" #NAME " arg" << endl; \
        cout << DESC << endl;                              \
        return 0;                                          \
    }                                                      \
    Config::NAME = __VA_ARGS__(args["--" #NAME]);          \

#endif