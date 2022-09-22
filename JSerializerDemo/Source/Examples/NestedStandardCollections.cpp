#define BOOST_UT_DISABLE_MODULE // After updgrading to VS 2022 we can use modules
#include "boost/ut.hpp"
#include <vector>
#include <map>
#include <string>
#include <stack>
#include <cassert>
#include "JSerializer.h"


namespace NestedStandardCollectionsExample
{

	CREATE_DEFAULT_JSER_MANAGER_TYPE(JSERManager);

	struct Foo : jser::JSerializable
	{
		int16_t foo_int = 44;
		Foo() {};
		Foo(int foo) : foo_int(foo) {};
		jser::JserChunkAppender AddItem() override
		{
			return JSerializable::AddItem().Append(JSER_ADD(JSERManager, foo_int));
		}
	};

	struct TestStruct : jser::JSerializable
	{
		// Complex type will be (de)serialized correctly
		std::map<std::string, std::vector<std::stack<Foo>>> foo_complex_type;
		jser::JserChunkAppender AddItem() override
		{
			return JSerializable::AddItem().Append(JSER_ADD(JSERManager, foo_complex_type));
		}
	};

	boost::ut::suite Suite = [] {
		using namespace boost::ut;
		"main"_test = []
		{
			TestStruct foo;
			std::stack<Foo> foo_stack;
			foo_stack.push(Foo(12));
			foo_stack.push(Foo(7));
			foo_stack.push(Foo(5));

			auto copied_stack = foo_stack;
			copied_stack.pop();

			foo.foo_complex_type = { {"hello", {foo_stack, foo_stack}}, {"world", {copied_stack, copied_stack}} };
			std::list<jser::JSerError> errorList;
			std::string result = foo.SerializeObjectString(std::back_inserter(errorList));
			assert(errorList.size() == 0);

			// TestStruct2 will also deserialize TestStruct1 inside foo
			TestStruct deserialized;
			deserialized.DeserializeObject(result, std::back_inserter(errorList));
			assert(errorList.size() == 0);
		};
	};
}