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
            std::string login = form.get("login"); // получаем логин

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
                    Keywords::use(login),
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
            }
            catch (...)
            {
                std::cout << "exception" << std::endl;
                response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_INTERNAL_SERVER_ERROR);
                return;
            }
            response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_OK);
        }
        else if(method == "GET" && form.has("first_name") && form.has("last_name"))
        {
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

            static cppkafka::Configuration config = {{"metadata.broker.list", Config::queue}};

            static cppkafka::Producer producer(config);
            static std::mutex mtx;
            std::lock_guard<std::mutex> lock(mtx);
            std::stringstream ss;
            Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
            root->set("login", login);
            root->set("first_name", first_name);
            root->set("last_name", last_name);
            root->set("age", age);
            Poco::JSON::Stringifier::stringify(root, ss);
            std::string message = ss.str();
            bool not_sent = true;
            while (not_sent)
            {
                try
                {
                    producer.produce(cppkafka::MessageBuilder(Config::topic).partition(0).payload(message));
                    not_sent = false;
                }
                catch (...)
                {
                }
            }

            root = new Poco::JSON::Object();
            root->set("status", "OK");
            Poco::JSON::Stringifier::stringify(root, ostr);
            response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_OK);
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

int main(int argc, char *argv[])
{
    string DESC = get_description("server.exe");
    HTTPWebServer app;
    if(argc != 9 + 1)
    {
        cout << "ERROR: argc must be " << 9 + 1  << endl;
        cout << DESC << endl;
        return 0;
    }

    auto args = argv2map(argc, argv, DESC);  // разбор входных параметров

    CHECK_ARG(ip)
    CHECK_ARG(login)
    CHECK_ARG(password)
    CHECK_ARG(database)
    CHECK_ARG(sql_port, stoi)
    CHECK_ARG(write)
    CHECK_ARG(read)
    CHECK_ARG(queue)
    CHECK_ARG(topic)

    return app.run(1, argv); // POCO принимает свои параметры командной строки, передаём их отсутствие
}