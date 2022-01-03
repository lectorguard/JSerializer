// ConsoleApplication3.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include <set>
#include "JSerializer.h"


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
        JSER_ADD_ITEMS(number, testEnum);
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
        JSER_ADD_ITEMS(isTrue);
        JSER_ADD_ITEMS(vector);
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
    Inherit() { JSER_ADD_ITEMS(pc, foo); }
    
private:
    std::map<std::string, int> pc = { {"CPU", 10}, { "GPU", 15 }, { "RAM", 20 }, };
    Foo foo;
    
};

struct Outer : public JSerializable {

    Outer()
    {
        myInt = new int8_t(7);
        JSER_ADD_ITEMS(m, charSet, n, *myInt);
    }

private:

    struct Inner : public JSerializable
    {
        Inner()
        {
            JSER_ADD_ITEMS(string);
        }

        virtual ~Inner() {};

    private:
        std::string string = "I am in a class ";
    } n;
    std::map<std::string, int> m = { {"CPU", 10}, { "GPU", 15 }, { "RAM", 20 }, };
    std::set<char> charSet = { 'H','e', 'l','o' };
    int8_t* myInt = nullptr; 
};


int main()
{
    Outer outer;
    JSerError error;
    if (std::optional<std::string> result = outer.SerializeObjectString(error))
    {
        std::cout << *result << std::endl;
    }
    else
    {
        std::cout << (int)error << std::endl;
    }
    outer.DeserializeObject(R"({"*myInt":7,"charSet":[72,101,108,111],"toilette":78,"m":{"CPU":10,"GPU":15,"RAM":99},"n":{"string":"I like goint to bed every day "}})", error);
    std::cout << (int)error << std::endl;


    // {"charSet":[72,101,108,111],"m":{"CPU":10,"GPU":15,"RAM":20},"n":{"string":"I am in a class "}}
    
    
    std::cout << *outer.SerializeObjectString(error) << std::endl;

    // {"charSet":[1,11,77,121],"m":{"CPU":109,"GPU":109,"RAM":109},"n":{"string":"I am happy "}}
    return 0;
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
