#include "defs.hpp"

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

            // Пытаемся считать из кеша
            string json;
            if (Cache::get(req_login, json))
            {
                ostr << json;
                cout << "read " + req_login + " from cache" << endl;
                response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_OK);
                return;
            }

            cout << "cache missed for " + req_login  << endl;

            // Обращение к БД
            auto session_ptr = unique_ptr<SqlSession>(create_SQL_session());
            auto &session = *session_ptr;
            Person person;
            bool success = true;

            SQL_HANDLE(
                Statement SELECT(session);
                SELECT << "SELECT login, first_name, last_name, age FROM Person WHERE login=?",
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
                success = false;
            }

            try
            {
                string res = "";
                Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
                if(success)
                {
                    root->set("login", person.login);
                    root->set("first_name", person.first_name);
                    root->set("last_name", person.last_name);
                    root->set("age", person.age);
                }
                std::stringstream ss;
                Poco::JSON::Stringifier::stringify(root, ss);
                res = ss.str();

                ostr << res; // отправляем ответ клиенту
                Cache::put(req_login, res); // сохраняем ответ в кеш (сквозное чтение)
            }
            catch (...)
            {
                std::cout << "exception" << std::endl;
            }
            response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_OK);
        }
        else if(method == "GET" && form.has("first_name") && form.has("last_name"))
        {
            /* Поиск по маске не кешируем */

            string first_name_mask = form.get("first_name"),
                   last_name_mask =  form.get("last_name");

            auto session_ptr = unique_ptr<SqlSession>(create_SQL_session());
            Poco::Data::Session &session = *session_ptr;
        
            Person person;
            vector<Person> result;

            SQL_HANDLE(
                Statement SELECT(session);
                SELECT << "SELECT login, first_name, last_name, age FROM Person WHERE first_name LIKE ? AND last_name LIKE ?",
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
                        result.push_back(person);
                }
            )

            try // отправляем результаты пользователю
            {
                Poco::JSON::Array arr;
                int i;
                for(i = 0; i < result.size(); i++)
                {
                    Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
                    root->set("login", result[i].login);
                    root->set("first_name", result[i].first_name);
                    root->set("last_name", result[i].last_name);
                    root->set("age", result[i].age);
                    arr.add(root);
                }
                Poco::JSON::Stringifier::stringify(arr, ostr);
            }
            catch (...)
            {
                std::cout << "exception" << std::endl;
                response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_INTERNAL_SERVER_ERROR);
                return;
            }
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
                SELECT << "SELECT login FROM Person WHERE login=?",
                    Keywords::use(login),
                    Keywords::range(0, 1);
                SELECT.execute();

                Poco::Data::RecordSet rs(SELECT);
                if (rs.moveFirst()) success = false;
            )
            
            if(success)
            {
                // добавляем в базу
                SQL_HANDLE(
                    Statement INSERT(session);
                    INSERT << "INSERT INTO Person (login, first_name, last_name, age)"
                              "VALUES (?, ?, ?, ?)",
                        Keywords::use(login),
                        Keywords::use(first_name),
                        Keywords::use(last_name),
                        Keywords::use(age),
                        Keywords::range(0, 1);
                    INSERT.execute();
                )

                // добавляем в кеш (сквозная запись)
                Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
                root->set("login", login);
                root->set("first_name", first_name);
                root->set("last_name", last_name);
                root->set("age", age);
                std::stringstream ss;
                Poco::JSON::Stringifier::stringify(root, ss);
                string res = ss.str();
                Cache::put(login, res);
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
            Cache::init();
            Cache::remove_all();
            srv.start();

            waitForTerminationRequest();
            srv.stop();
        }
        return Application::EXIT_OK;
    }

private:
    bool _helpRequested;
};

int main(int argc, char *argv[])
{
    string DESC = get_description("server.exe");
    HTTPWebServer app;
    if(argc != ARG_COUNT + 1)
    {
        cout << "ERROR: argc must be " << ARG_COUNT + 1  << endl;
        cout << DESC << endl;
        return 0;
    }

    auto args = argv2map(argc, argv, DESC);  // разбор входных параметров

    CHECK_ARG(ip)
    CHECK_ARG(login)
    CHECK_ARG(password)
    CHECK_ARG(database)
    CHECK_ARG(sql_port, stoi)
    CHECK_ARG(cache_servers)

    return app.run(1, argv); // POCO принимает свои параметры командной строки, передаём их отсутствие
}