#define BOOST_UT_DISABLE_MODULE // After updgrading to VS 2022 we can use modules
#include "boost/ut.hpp"
#include <cassert>
#include "JSerializer.h"

namespace SimleSetupExample
{
	// This manager type shows the (de)serializer 
	// how to (de)serialize an object 
	// You can give the type a custom name
	CREATE_DEFAULT_JSER_MANAGER_TYPE(JSERManager);
	
	struct TestStruct : jser::JSerializable
	{
		// All JSerializable objects must be default constructible
		// If you add a custom constructor, 
		// you have to add a default constructor as well
	
		std::array<int, 3> foo = { 0,0,0 };
		std::list<int> foo_list = { 45,48,513,8,61,86,156 };
		int foo_int = 4;
	
		// Every JSerializable object has an AddItem function,
		// there we set the variables we want to (de)serialize
		jser::JserChunkAppender AddItem() override
		{
			// With JSER_ADD macro, we can add easily variables by writing the variable name
			// You can also call the raw function call 
			// CreateSerializeItem<JSERManager>({"foo", "foo_list"}, foo, foo_list);
			return JSerializable::AddItem().Append(JSER_ADD(JSERManager, foo, foo_list, foo_int));
		}
	};

	boost::ut::suite Suite = [] {
		using namespace boost::ut;
		"main"_test = [] 
		{
			TestStruct foo;
			std::list<jser::JSerError> errorList;
			std::string result = foo.SerializeObjectString(std::back_inserter(errorList));
			assert(errorList.size() == 0);

			// Deserialization Process
			TestStruct deserialized;
			deserialized.DeserializeObject(result, std::back_inserter(errorList));
			assert(errorList.size() == 0);
		};
	};
}
