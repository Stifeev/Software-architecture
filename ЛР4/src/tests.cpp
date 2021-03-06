#include <gtest/gtest.h>
#include "defs.hpp"

/*
 * Запуск тестов: $ build/tests.exe <args>
 * Внимание!
 * Прежде, чем запускать тесты, запустите server командой 
 * $ build/server.exe <args>
 * и writer командой 
 * $ build/writer.exe <args>
*/

void add_person(Person person)
{
    Poco::Net::SocketAddress sa(Config::ip, Config::port);
    Poco::Net::StreamSocket socket(sa);

    Poco::Net::SocketStream str(socket);
    str << "POST /person HTTP/1.1\n" <<
            "content-type: application/url-encoded\n\n" <<
            "login=" << person.login << 
            "&first_name=" << person.first_name <<
            "&last_name=" << person.last_name <<
            "&age=" << person.age << "\n";
    str.flush();
    socket.shutdownSend();
}

bool check_person(Person person)
{
    Poco::Net::SocketAddress sa(Config::ip, Config::port);
    Poco::Net::StreamSocket socket(sa);

    Poco::Net::SocketStream str(socket);
    str << "GET /person?login=" << person.login << "\nHTTP/1.1\n";
            
    str.flush();
    socket.shutdownSend();

    std::stringstream ss;
    Poco::StreamCopier::copyStream(str, ss);
    string ans = ss.str();
    int i = ans.find('{'); // пока так
    int j = ans.find('}');
    string json = ans.substr(i, j - i + 1);

    Poco::JSON::Parser parser;

    bool res = true;
    try
    {
        Poco::Dynamic::Var result = parser.parse(json);
        Poco::JSON::Object::Ptr object = result.extract<Poco::JSON::Object::Ptr>();
        string login = object->get("login").toString();
        string first_name = object->get("first_name").toString();
        string last_name = object->get("last_name").toString();
        int age;
        object->get("age").convert(age);
        res = login == person.login && first_name == person.first_name && last_name == person.last_name && age == person.age;
    }
    catch(...)
    {
        res = false;
    }
   
    return res;
}

TEST(test_create, basic_test_set) // создание базы
{
    testing::internal::CaptureStdout();

    auto session_ptr = unique_ptr<SqlSession>(create_SQL_session());
    auto &session = *session_ptr;

    SQL_HANDLE(
        Statement DROP(session);
        DROP << "DROP TABLE IF EXISTS Person"; 
        DROP.execute();
    )

    SQL_HANDLE(
        Statement CREATE(session);
        CREATE << "CREATE TABLE IF NOT EXISTS Person"
                  "("
                  "    login VARCHAR(30) CHARACTER SET utf8 COLLATE utf8_unicode_ci NOT NULL PRIMARY KEY,"
                  "    first_name VARCHAR(50) CHARACTER SET utf8 COLLATE utf8_unicode_ci  NULL,"
                  "    last_name VARCHAR(50) CHARACTER SET utf8 COLLATE utf8_unicode_ci  NOT NULL,"
                  "    age INT NULL CHECK(age >= 0)"
                  ")";
        
        CREATE.execute();
    )

    ASSERT_TRUE(testing::internal::GetCapturedStdout() == "");
}

TEST(test_add, basic_test_set) // добавление записей с последующей проверкой
{
    vector<Person> persons = {{string("stif"), string("Evgeniy"), string("Stifeev"), 22},
                              {string("mimimi"), string("Liza"), string("Solnceva"), 21},
                              {string("pup"), string("Vasya"), string("Pupkin"), 20},
                              {string("vano"), string("Vasiliy"), string("Ivanov"), 42}};

    int i;
    testing::internal::CaptureStdout();

    for(i = 0; i < persons.size(); i++) 
    {
        add_person(persons[i]);
    }
    
    sleep(8); // ждём пока не запишется в базу

    for(i = 0; i < persons.size(); i++) // запросы на поиск
    {
        if(!check_person(persons[i]))
        {
            cout << "ERROR in check persons[" << i << "]\n";
        }
    }

    ASSERT_TRUE(testing::internal::GetCapturedStdout() == "");
}

int main(int argc, char *argv[])
{
    string DESC = get_description(string("tests.exe"));
    if(argc != 5 + 1)
    {
        cout << "ERROR: argc must be " << 5 + 1  << endl;
        cout << DESC << endl;
        return 0;
    }

    auto args = argv2map(argc, argv, DESC);  // разбор входных параметров

    CHECK_ARG(ip)
    CHECK_ARG(login)
    CHECK_ARG(password)
    CHECK_ARG(database)
    CHECK_ARG(sql_port, stoi)

    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}