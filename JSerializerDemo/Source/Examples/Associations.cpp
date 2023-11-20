#define BOOST_UT_DISABLE_MODULE // After updgrading to VS 2022 we can use modules
#include "boost/ut.hpp"
#include <memory>
#include <cassert>
#include "JSerializer.h"


namespace AssociationsExample
{
	CREATE_DEFAULT_JSER_MANAGER_TYPE(JSERManager);

	struct TestStruct1 : jser::JSerializable
	{
		std::string foo_name = "Hello C++";
		jser::JserChunkAppender AddItem() override
		{
			return JSerializable::AddItem().Append(JSER_ADD(JSERManager, foo_name));
		}
	};

	struct TestStruct2 : jser::JSerializable
	{
		TestStruct1 foo_testStruct1;
		std::unique_ptr<TestStruct1> foo_uniquePtr;
		jser::JserChunkAppender AddItem() override
		{
			return JSerializable::AddItem().Append(JSER_ADD(JSERManager, foo_testStruct1, foo_uniquePtr));
		}
	};

	boost::ut::suite Suite = [] {
		using namespace boost::ut;
		"main"_test = []
		{
			// Set values
			TestStruct2 foo;
			foo.foo_uniquePtr = std::make_unique<TestStruct1>();
			foo.foo_testStruct1.foo_name = "This string has changed";
			std::list<jser::JSerError> errorList;
			
			std::string result = foo.SerializeObjectString(std::back_inserter(errorList));
			assert(errorList.size() == 0);

			// TestStruct2 will also deserialize TestStruct1 inside foo
			TestStruct2 deserialized;
			deserialized.DeserializeObject(result, std::back_inserter(errorList));
			assert(errorList.size() == 0);


			// Also testing binary json logic
			std::vector<uint8_t> bson_result = foo.SerializeObjectBson(std::back_inserter(errorList));
			assert(errorList.size() == 0);

			TestStruct2 bson_deserialized;
			bson_deserialized.DeserializeObject(bson_result, std::back_inserter(errorList));
			assert(errorList.size() == 0);
		};
	};
}