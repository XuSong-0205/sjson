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

}

void test_utils()
{
    std::is_default_constructible<sjson::json_bind<Person>>::value;
    std::is_default_constructible<sjson::json_bind<Base>>::value;

    sjson::has_to_json<Person>::value;
    sjson::has_from_json<Person>::value;
    sjson::has_to_json<Person, json>::value;
    sjson::has_from_json<Person, json>::value;

    sjson::has_to_json<int, json>::value;
    sjson::has_from_json<int, json>::value;


    Person p("zhangsan", 20);
    Base b{0};
    
    json obj(p);
    //json obj1(b); // error
    std::cout << color::F_GREEN << obj.dump(4) << color::CLEAR_F << "\n";

}


int main()
{
    test_utils();


    return 0;
}