// ConsoleApplication3.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include <list>
#include <set>
#include "JSerializer.h"
#define BOOST_UT_DISABLE_MODULE // After updgrading to VS 2022 we can use modules
#include "boost/ut.hpp"


enum class TestEnum
{
    HALLO = 1,
    WORLD = 2,
    EX = 4
};



struct Foo : public JSerializable
{
    Foo()
    {
        JSER_ADD(number, testEnum);
        JSER_ADD_VAL(
            {
                assert(number < INT32_MAX);
            }
        );
    }

private:
    int32_t number = 5;
    TestEnum testEnum = TestEnum::HALLO;
};

struct Poo : public JSerializable
{
    Poo() {
        JSER_ADD(isTrue);
        JSER_ADD(vector);
        JSER_ADD_VAL(
            {
                assert(vector.size() > 0);
            }
        );
    }

private:
    std::vector<double> vector = { 12.3, 15.4 }; 
    bool isTrue = false;
};


struct Inherit : public Poo
{
    Inherit() { JSER_ADD(pc, foo); }
    
private:
    std::map<std::string, int> pc = { {"CPU", 10}, { "GPU", 15 }, { "RAM", 20 }, };
    Foo foo;
    
};

struct Outer : public JSerializable {
    Outer()
    {
        myInt = new int8_t(7);
        JSER_ADD(m, charSet, n, *myInt);
    }

    struct Inner : public JSerializable
    {
        Inner()
        {
            JSER_ADD_CUSTOM(
                [this](nlohmann::json& j, PushErrorType pushError) {
                    j["string"] = string; 
                }, 
                [this](nlohmann::json& j, PushErrorType pushError) {
                    if (j.contains("string"))
                        string = j["string"];
                    else
                        pushError({JSerErrorTypes::JSON_ERROR, "Key string does not exist in json"});
                });
        }
    private:
        std::string string = "I am in a class ";
    } n;

    std::map<std::string, int> m = { {"CPU", 10}, { "GPU", 15 }, { "RAM", 20 }, };
    std::set<char> charSet = { 'H','e', 'l','o' };
    int8_t* myInt = nullptr; 
};

int main()
{

    //return 0;
    //using namespace  boost::ut;
    // 
    //"simple"_test = [] {
    //    Foo* foo = new Foo();
    //    boost::ut::expect(dynamic_cast<JSerializable*>(foo) == nullptr);
    //    delete foo;
    //};
    //
    //
    //Outer outer;
    //std::list<JSerError> errorList;
    //
    //std::cout << outer.SerializeObjectString(std::back_inserter(errorList)) << std::endl;
    //for (JSerError error : errorList)
    //    std::cout << (int)error.Error << "  msg : " << error.Message  << std::endl;
    //errorList.clear();
    //
    //outer.DeserializeObject(R"({"*myInt":7,"charSet":[72,101,108,111],"m":{"CPU":10,"GPU":15,"RAM":20},"n":{"string":"NOOOOOO"}})", std::back_inserter(errorList));
    //for(JSerError error : errorList)
    //    std::cout << (int)error.Error << "  msg : " << error.Message << std::endl;
    //errorList.clear();
    //
    //std::cout << outer.SerializeObjectString(std::back_inserter(errorList)) << std::endl;
    //return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
