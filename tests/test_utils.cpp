#include "test.h"

struct Person
{
    std::string name_;
    int age_;

    Person(const std::string& name, int age)
        : name_(name), age_(age)
    {
    }
};

struct Base
{
    int num;
};

struct Test0
{
    int id;
    Test0(int n = 0) : id(n) {}
};


namespace sjson
{
    template<>
    struct json_bind<Person>
    {
        void to_json(json& j, const Person& v)
        {
            j["name"] = v.name_;
            j["age"] = v.age_;
        }

        void from_json(const json& j, Person& v)
        {
            v.name_ = j["name"].get<std::string>();
            v.age_ = j["age"].get<int>();
        }

    };

    template<>
    struct json_bind<Test0>
    {
        void to_json(json& j, const Test0& t)
        {
            j = {
                {"id", t.id}
            };
        }

        void from_json(const json& j, Test0& t)
        {
            t.id = j["id"].get<int>();
        }
    };
}

void test_utils()
{
    Person p("zhangsan", 20);
    Base b{0};
    
    json obj(p);
    // json obj1(b); // error
    std::cout << color::F_GREEN << obj.dump(4) << color::CLEAR_F << "\n";


    using namespace sjson;

    Test0 t0(0);
    json j0(t0);
    j0 << t0;
    j0 >> t0;
    std::cout << color::F_GREEN << j0 << color::CLEAR_F << "\n";

    Test0 t1(1);
    std::stringstream ss;
    ss << sjson::json_wrap(t1);
    Test0 t2;
    ss >> sjson::json_wrap(t2);
    std::cout << color::F_BLUE << sjson::json_wrap(t2) << color::CLEAR_F << "\n";


    Test0 t3(2);
    json j2 = t3;                       // to_json
    auto t4 = static_cast<Test0>(j2);   // from_json    explicit type conversions
    auto t5 = (Test0)(j2);              // from_json    explicit type conversions
    // Test0 t6 = j2;                      // error     implicit type conversion
    // auto t5 = static_cast<Base>(j2);    // error
    std::cout << color::F_CYAN <<  sjson::json_wrap(t4) << color::CLEAR_F << "\n";

}





int main()
{
    test_utils();


    return 0;
}