// ConsoleApplication3.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "JSerializer.h"


enum class TestEnum
{
    HALLO = 1,
    WORLD = 2,
    EX = 4
};

struct Foo : public Serializable
{
    Foo()
    {
        SETUP(number, text, fpNumber, vector, isTrue);
        AddValidation([this]() 
            {
                assert(number < 6);
                assert(vector.size() > 0);
            });
    }

    void toString() {
        std::cout << "number" << "  " << number << " | " << "text" << "  " << text << " | " << "fpNumber" << "  " << fpNumber << " | " << "vector" << "  ";
        std::cout << " [ ";
        for(auto elem : vector)
        {
            std::cout << elem << " , ";
        }
        std::cout << " ] ";
        std::cout << "TestEnum" << "  " << (int)testEnum;
        std::cout << std::endl;
    }

private:
    bool isTrue = false;
    int32_t number = 5;
    std::string text = "Hello World";
    float fpNumber = 12.5f;
    std::vector<double> vector = { 12.3, 15.4 };
    TestEnum testEnum = TestEnum::HALLO;
};



int main()
{
    Foo foo;
    std::cout << foo.SerializeObject() << std::endl;
    foo.DeserializeObject(R"({"fpNumber":12.5,"number":0,"text":"Something else","vector":[1.3,2.7],"isTrue":false,"testEnum":4})");
    foo.toString();
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
