#define BOOST_UT_DISABLE_MODULE // After updgrading to VS 2022 we can use modules
#include "boost/ut.hpp"
#include "JSerializer.h"
#include <iostream>
#include <stack>

namespace Validation_Test
{
    CREATE_DEFAULT_JSER_MANAGER_TYPE(JSERManager);
    using namespace jser;

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
        }

        void Validate(JSerEvent jser_event, PushErrorType push_error) override
        {
            using namespace boost::ut;
            if (foo_stack.size() != pushed_elements.size())
            {
				push_error({ JSerErrorTypes::VALIDATION_ERROR, "Check if validation errors are working" });
				return;
            }
			const std::deque<float> deque_casted = foo_stack._Get_container();
			for (size_t i = 0; i < deque_casted.size(); ++i)
			{
				expect(deque_casted[i] == pushed_elements[i]) << "Elements in deque and vector are not identical";
			}
        }

        JserChunkAppender AddItem() override
        {
            return JSerializable::AddItem().Append(JSER_ADD(JSERManager, foo_stack));
        }

    };

    boost::ut::suite Association_Test = [] {
        using namespace boost::ut;
        "validation"_test = [] 
        {
            Foo foo;
            foo.foo_stack.push(156.0f);
            std::list<JSerError> errorList;
            std::string result = foo.SerializeObjectString(std::back_inserter(errorList));
            expect(errorList.size() == 1) << "There should be one validation error";
            expect(errorList.front().Error == JSerErrorTypes::VALIDATION_ERROR);

            errorList.clear();
            Foo deserialized;
            deserialized.DeserializeObject(result, std::back_inserter(errorList));
			expect(errorList.size() == 1) << "There should be one validation error";
			expect(errorList.front().Error == JSerErrorTypes::VALIDATION_ERROR);
        };

		"validation from string"_test = []
		{
            const char* expected = R"({"foo_stack":[12.5,9.5,7.5,23.5,15.5,16.5,4.5]})";
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

