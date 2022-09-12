#define BOOST_UT_DISABLE_MODULE // After updgrading to VS 2022 we can use modules
#include "boost/ut.hpp"
#include "JSerializer.h"
#include <iostream>
#include <vector>
#include <functional>

namespace CustomSerialization_Test
{
    struct Foo : JSerializable
    {
        std::vector<uint64_t> foo = {15,56656565,498,48};

        Foo()
        {
            JSER_ADD_CUSTOM(
                [this](nlohmann::json& j, const std::function<void(JSerError)>& pushError)
                {
                    const std::string elementName = "foo";
                    for (std::size_t index = 0; index < foo.size(); ++index)
                    {
                        const std::string currentName = elementName + std::to_string(index);
                        j[currentName] = foo[index];
                    }
                },
                [this](nlohmann::json& j, const std::function<void(JSerError)>& pushError)
                {
                    const std::string elementName = "foo";
                    uint32_t currentIndex = 0;
                    std::function<std::string()> currentName = [&]
                    {
                        return elementName + std::to_string(currentIndex);
                    };
                    std::vector<uint64_t> temp = {};
                    while(j.contains(currentName()))
                    {
                        temp.push_back(j[currentName()]);
                        ++currentIndex;
                    };
                    foo = temp;
                });
        };

        void compare(const Foo& Rhs)
        {
            using namespace boost::ut;
            expect(foo == Rhs.foo);
        };
    };

    boost::ut::suite CustomSerialization_Test = [] {
        using namespace boost::ut;

        "custom serialization"_test = [] {
            Foo foo;
            std::list<JSerError> errorList;
            std::string result = foo.SerializeObjectString(std::back_inserter(errorList));
            expect(errorList.size() == 0) << "Serialization of many object associations throws error";

            Foo deserialized;
            deserialized.DeserializeObject(result, std::back_inserter(errorList));
            expect(errorList.size() == 0) << "Deserialization of many object associations throws error";
            foo.compare(deserialized);
        };


        "custom serialization from string"_test = [] {
            Foo foo;
            std::list<JSerError> errorList;
            std::string result = foo.SerializeObjectString(std::back_inserter(errorList));
            expect(errorList.size() == 0) << "Serialization of many object associations throws error";
            const char* expectedResult = R"({"foo0":15,"foo1":56656565,"foo2":498,"foo3":48})";
            expect(!result.compare(expectedResult));

            Foo deserialized;
            deserialized.DeserializeObject(expectedResult, std::back_inserter(errorList));
            expect(errorList.size() == 0) << "Deserialization of many object associations throws error";
            foo.compare(deserialized);
        };
    };
}