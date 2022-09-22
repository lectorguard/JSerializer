#define BOOST_UT_DISABLE_MODULE // After updgrading to VS 2022 we can use modules
#include "boost/ut.hpp"
#include <cassert>
#include "JSerializer.h"

namespace InheritanceExample
{
	CREATE_DEFAULT_JSER_MANAGER_TYPE(JSERManager);

	struct Foo : jser::JSerializable
	{
		uint32_t foo = 9;

		jser::JserChunkAppender AddItem() override
		{
			return JSerializable::AddItem().Append(JSER_ADD(JSERManager, foo));
		}
	};

	struct Foo1 : Foo
	{
		uint32_t foo1 = 76543;

		jser::JserChunkAppender AddItem() override
		{
			// We call Foo::AddItem() instead of JSerializable::AddItem() !!!
			return Foo::AddItem().Append(JSER_ADD(JSERManager, foo1));
		}
	};

	boost::ut::suite Suite = [] {
		using namespace boost::ut;
		"main"_test = []
		{
			Foo1 foo;
			std::list<jser::JSerError> errorList;
			std::string result = foo.SerializeObjectString(std::back_inserter(errorList));
			assert(errorList.size() == 0);

			// Deserialization Process
			Foo1 deserialized;
			deserialized.DeserializeObject(result, std::back_inserter(errorList));
			assert(errorList.size() == 0);
		};
	};
}
