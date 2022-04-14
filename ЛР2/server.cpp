#include "defs.hpp"

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

class WebPageHandler : public HTTPRequestHandler
{
public:
    WebPageHandler(const std::string &format) : _format(format)
    {
    }

    void handleRequest(HTTPServerRequest &request,
                       HTTPServerResponse &response)
    {
        response.setChunkedTransferEncoding(true);
        response.setContentType("text/html");

        std::ostream &ostr = response.send(); // выходной поток
        string url = request.getURI();
        string path2file = "../content" + url;
        FILE *fp = fopen(path2file.c_str(), "rb");
        if(!fp)
        {
            cout << "path " << path2file << " not found" << endl;
            ostr << "<html lang=\"ru\">"
                    "<head><title>Web Server</title></head>"
                    "<body><h1>Error 404: page not found</h1></body>"
                    "</html>";
            response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_NOT_FOUND);
        }
        else
        {
            vector<char> buf(400); // читаем с помощью буфера
            int len;
            while(len = fread(buf.data(), sizeof(char), buf.size() - 1, fp))
            {
                buf[len] = '\0';
                ostr << buf.data();
            }
            fclose(fp);
            response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_OK);
        }
    }

private:
    std::string _format;
};

class RequestHandler : public HTTPRequestHandler
{
public:
    RequestHandler(const std::string &format) : _format(format)
    {
    }

    void handleRequest(HTTPServerRequest &request,
                       HTTPServerResponse &response)
    {
        Poco::Net::HTMLForm form(request, request.stream());
        response.setChunkedTransferEncoding(true);
        response.setContentType("application/json");
        std::ostream &ostr = response.send();
        string method = request.getMethod();
        
        if (method == "GET" && form.has("login"))
        {
            std::string req_login = form.get("login"); // получаем логин

            auto session_ptr = unique_ptr<SqlSession>(create_SQL_session());
            auto &session = *session_ptr;
        
            Person person;
            bool success = true;

            SQL_HANDLE(
                Statement SELECT(session);
                SELECT << "SELECT login, first_name, last_name, age FROM Person WHERE login=?" <<
                          " -- sharding:" << STR(get_shard_id(req_login)),
                    Keywords::into(person.login),
                    Keywords::into(person.first_name),
                    Keywords::into(person.last_name),
                    Keywords::into(person.age),
                    Keywords::use(req_login),
                    Keywords::range(0, 1);
                SELECT.execute();

                Poco::Data::RecordSet rs(SELECT);
                if (!rs.moveFirst()) throw std::logic_error("not found");
            )
            catch(std::logic_error &e) // пользователь не найден
            {
                cout << req_login << " not found" << endl;
                success = false;
            }

            try
            {
                Poco::JSON::Array arr;
                
                if(success)
                {
                    Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
                    root->set("login", person.login);
                    root->set("first_name", person.first_name);
                    root->set("last_name", person.last_name);
                    root->set("age", person.age);
                    arr.add(root);
                }
                
                Poco::JSON::Stringifier::stringify(arr, ostr);
            }
            catch (...)
            {
                std::cout << "exception" << std::endl;
            }
            response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_OK);
        }
        else if(method == "GET" && form.has("first_name") && form.has("last_name"))
        {
            string first_name_mask = form.get("first_name"),
                   last_name_mask =  form.get("last_name");

            /* Функция, которую обрабатывает один поток */
            auto send_request = [](int shard_id,
                                   string first_name_mask, string last_name_mask, 
                                   vector<Person> *result) -> void 
            {
                auto session_ptr = unique_ptr<SqlSession>(create_SQL_session());
                Poco::Data::Session &session = *session_ptr;
            
                Person person;

                SQL_HANDLE(
                    Statement SELECT(session);
                    SELECT << "SELECT login, first_name, last_name, age FROM Person WHERE first_name LIKE ? AND last_name LIKE ?" <<
                              " -- sharding:" << STR(shard_id),
                        Keywords::into(person.login),
                        Keywords::into(person.first_name),
                        Keywords::into(person.last_name),
                        Keywords::into(person.age),
                        Keywords::use(first_name_mask),
                        Keywords::use(last_name_mask),
                        Keywords::range(0, 1);

                    while(!SELECT.done())
                    {
                        if(SELECT.execute())
                            result->push_back(person);
                    }
                )
            };

            vector<vector<Person> *> shards_result(Config::n_shards);
            vector<thread *> vec_threads(Config::n_shards);
            int i;
            for(i = 0; i < Config::n_shards; i++) // запускаем процессы ко всем шардам, результат в shards_result
            {
                shards_result[i] = new vector<Person>(0); // заказ вектора
                vec_threads[i] = new thread(send_request, i, 
                                            first_name_mask, last_name_mask,
                                            shards_result[i]);
            }
            WAIT_ALL_THREADS(vec_threads);
            
            try // отправляем результаты пользователю
            {
                Poco::JSON::Array arr;
                int i, j;
                for(i = 0; i < shards_result.size(); i++) // цикл по шардам
                    for(j = 0; j < shards_result[i]->size(); j++) // цикл по ответам внутри шарда
                    {
                        Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
                        root->set("login", shards_result[i]->at(j).login);
                        root->set("first_name", shards_result[i]->at(j).first_name);
                        root->set("last_name", shards_result[i]->at(j).last_name);
                        root->set("age", shards_result[i]->at(j).age);
                        arr.add(root);
                    }
                Poco::JSON::Stringifier::stringify(arr, ostr);
            }
            catch (...)
            {
                std::cout << "exception" << std::endl;
            }
            for(i = 0; i < shards_result.size(); i++) // очистка памяти
                delete shards_result[i];
            response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_OK);
        }
        else if(method == "POST" && form.has("login") && form.has("last_name") && form.has("first_name") && form.has("age"))
        {
            string login = form.get("login"),
                   last_name =  form.get("last_name"),
                   first_name = form.get("first_name");
            int age = atoi(form.get("age").c_str());

            auto session_ptr = unique_ptr<SqlSession>(create_SQL_session());
            auto &session = *session_ptr;

            bool success = true;

            /* Проверяем есть ли такой человек*/
            SQL_HANDLE(
                Statement SELECT(session);
                SELECT << "SELECT login FROM Person WHERE login=?" << " -- sharding:" << STR(get_shard_id(login)),
                    Keywords::use(login),
                    Keywords::range(0, 1);
                SELECT.execute();

                Poco::Data::RecordSet rs(SELECT);
                if (rs.moveFirst()) success = false;
            )
            
            if(success) // добавляем
            {
                SQL_HANDLE(
                    Statement INSERT(session);
                    INSERT << "INSERT INTO Person (login, first_name, last_name, age)"
                              "VALUES (?, ?, ?, ?)" << " -- sharding:" << STR(get_shard_id(login)),
                        Keywords::use(login),
                        Keywords::use(first_name),
                        Keywords::use(last_name),
                        Keywords::use(age),
                        Keywords::range(0, 1);
                    INSERT.execute();
                )
            }

            ostr << "<html lang=\"ru\">"
                    "<head><title>Web Server</title></head>"
                    "<body><h1>" + (success ? string("OK") : string("Already exists")) + "</h1></body>"
                    "</html>";
            response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_OK);
        }
        else
        {
            response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_BAD_REQUEST);
        }
    }

private:
    std::string _format;
};

class HTTPRequestFactory : public HTTPRequestHandlerFactory
{
public:
    HTTPRequestFactory(const std::string &format) : _format(format)
    {
    }

    HTTPRequestHandler *createRequestHandler(const HTTPServerRequest &request)
    {
        cout << "Request " + request.getMethod() + " " +  request.getURI() + " from " +
        request.clientAddress().toString() << endl;
        if (starts_with(request.getURI(), "/person"))
        {
            return new RequestHandler(_format);
        }
        else
        {
            return new WebPageHandler(_format);
        }
    }

private:
    std::string _format;
};

class HTTPWebServer : public Poco::Util::ServerApplication
{
public:
    HTTPWebServer() : _helpRequested(false)
    {
    }

    ~HTTPWebServer()
    {
    }

protected:
    void initialize(Application &self)
    {
        loadConfiguration();
        ServerApplication::initialize(self);
    }

    void uninitialize()
    {
        ServerApplication::uninitialize();
    }

    void defineOptions(OptionSet &options)
    {
        ServerApplication::defineOptions(options);
    }

    int main(const std::vector<std::string> &args)
    {
        if (!_helpRequested)
        {
            unsigned short port = (unsigned short)config().getInt("HTTPWebServer.port", Config::port);
            std::string format(
                config().getString("HTTPWebServer.format",
                                   DateTimeFormat::SORTABLE_FORMAT));

            ServerSocket svs(Poco::Net::SocketAddress("0.0.0.0", port));
            HTTPServer srv(new HTTPRequestFactory(format),
                           svs, new HTTPServerParams);

            std::cout << "Started server on " << Config::ip << ":" << STR(port) << std::endl;
            srv.start();

            waitForTerminationRequest();
            srv.stop();
        }
        return Application::EXIT_OK;
    }

private:
    bool _helpRequested;
};

static string DESC = 
"usage: ./server.exe --ip=<machine_ip>\n" \
"example: ./server.exe --ip=192.168.31.63";

int main(int argc, char *argv[])
{
    HTTPWebServer app;
    if(argc != 2)
    {
        cout << "ERROR" << endl;
        cout << DESC << endl;
        return 0;
    }

    std::map<string, string> args; // разбор входных параметров
    argv2map(argc, argv, args, DESC);

    if(args.find("--ip") == args.end())
    {
        cout << "ERROR: not find --ip arg" << endl;
        cout << DESC << endl;
        return 0;
    }
    Config::ip = args["--ip"];

    return app.run(1, argv); // POCO принимает свои параметры командной строки, передаём их отсутствие
}