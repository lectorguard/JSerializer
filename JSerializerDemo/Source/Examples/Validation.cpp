#define BOOST_UT_DISABLE_MODULE // After updgrading to VS 2022 we can use modules
#include "boost/ut.hpp"
#include <cassert>
#include "JSerializer.h"

namespace ValidationExample
{
	CREATE_DEFAULT_JSER_MANAGER_TYPE(JSERManager);

	struct Foo : jser::JSerializable
	{
		std::stack<float> foo_stack;
		const std::vector<float> stack_elements = { 12.5f , 9.5f , 7.5f , 23.5f , 15.5f, 16.5f, 5.0f };
		Foo()
		{
			for (const float& elem : stack_elements)
			{
				foo_stack.emplace(elem);
			}
		}

		void Validate(jser::JSerEvent jser_event, jser::PushErrorType push_error) override
		{
			if (foo_stack.size() != stack_elements.size())
			{
				push_error({ jser::JSerErrorTypes::VALIDATION_ERROR, "Check if validation errors ar working" });
				return;
			}
		}

		jser::JserChunkAppender AddItem() override
		{
			return JSerializable::AddItem().Append(JSER_ADD(JSERManager, foo_stack));
		}
	};

	boost::ut::suite Suite = [] {
		using namespace boost::ut;
		"main"_test = []
		{
			Foo foo;
			foo.foo_stack.push(156.0f);
			std::list<jser::JSerError> errorList;
			std::string result = foo.SerializeObjectString(std::back_inserter(errorList));
			assert(errorList.size() == 1);
			assert(errorList.front().Error == jser::JSerErrorTypes::VALIDATION_ERROR);
			errorList.clear();

			Foo deserialized;
			deserialized.DeserializeObject(result, std::back_inserter(errorList));
			assert(errorList.size() == 1);
			assert(errorList.front().Error == jser::JSerErrorTypes::VALIDATION_ERROR);
		};
	};
}
