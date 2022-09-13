#define BOOST_UT_DISABLE_MODULE // After updgrading to VS 2022 we can use modules
#include "boost/ut.hpp"
#include "JSerializer.h"
#include <iostream>
#include <stack>

namespace Validation_Test
{

    struct Foo : JSerializable
    {
        std::stack<float> foo_stack;
        const std::vector<float> pushed_elements = {12.5f , 9.5f , 7.5f , 23.5f , 15.5f, 16.5f, 4.5f};
        Foo()
        {
            for (const float& elem : pushed_elements)
            {
                foo_stack.emplace(elem);
            }

            JSER_ADD_CUSTOM(
                [this](nlohmann::json& j, PushErrorType pushError)
                {
                   std::deque<float> deque_casted = foo_stack._Get_container();
                   std::vector<float> serializable(deque_casted.begin(), deque_casted.end());
                   j["foo"] = serializable;
                },
                [this](nlohmann::json& j, PushErrorType pushError)
                {
					if (j.contains("foo"))
					{
						std::stack<float> foo_temp;
						const std::vector<float> deserialized = j["foo"];
                        std::for_each(deserialized.begin(), deserialized.end(), 
                            [&foo_temp](float f)
                            {
                                foo_temp.push(f);
                            }
                        );
                        foo_stack = foo_temp;
					}
					else std::cout << "foo does not exist ";

                }
                );
            JSER_ADD_VAL(
                {
                    using namespace boost::ut;
                    expect(foo_stack.size() == pushed_elements.size()) << "Sizes are not equal";
                    const std::deque<float> deque_casted = foo_stack._Get_container();
                    for (size_t i = 0; i < deque_casted.size(); ++i)
                    {
                        expect(deque_casted[i] == pushed_elements[i]) << "Elements in deque and vector are not identical";
                    }
                }
            );
        }
    };

    boost::ut::suite Association_Test = [] {
        using namespace boost::ut;
        "validation"_test = [] 
        {
            Foo foo;
            std::list<JSerError> errorList;
            std::string result = foo.SerializeObjectString(std::back_inserter(errorList));
            expect(errorList.size() == 0) << "Serialization of many object associations throws error";

            Foo deserialized;
            deserialized.DeserializeObject(result, std::back_inserter(errorList));
            expect(errorList.size() == 0) << "Deserialization of many object associations throws error";
        };

		"validation from string"_test = []
		{
            const char* expected = R"({"foo":[12.5,9.5,7.5,23.5,15.5,16.5,4.5]})";
			Foo foo;
			std::list<JSerError> errorList;
			std::string result = foo.SerializeObjectString(std::back_inserter(errorList));
			expect(errorList.size() == 0) << "Serialization of many object associations throws error";
            expect(!result.compare(expected));

			Foo deserialized;
			deserialized.DeserializeObject(expected, std::back_inserter(errorList));
			expect(errorList.size() == 0) << "Deserialization of many object associations throws error";
		};

    };
}

