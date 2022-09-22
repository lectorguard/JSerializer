#define BOOST_UT_DISABLE_MODULE // After updgrading to VS 2022 we can use modules
#include "boost/ut.hpp"
#include <cassert>
#include "JSerializer.h"

namespace SubclassExample
{
	CREATE_DEFAULT_JSER_MANAGER_TYPE(JSERManager);

	struct Foo : public jser::JSerializable
	{
		jser::JserChunkAppender AddItem() override
		{
			return JSerializable::AddItem().Append(JSER_ADD(JSERManager, foo, obj1));
		}

		struct Foo1 : public jser::JSerializable
		{
			int32_t foo = 9;

			jser::JserChunkAppender AddItem() override
			{
				return JSerializable::AddItem().Append(JSER_ADD(JSERManager, foo));
			}
		};

		Foo1 obj1;
		int32_t foo = 5;
	};
	boost::ut::suite Suite = [] {
		using namespace boost::ut;
		"main"_test = []
		{
			Foo foo;
			std::list<jser::JSerError> errorList;
			std::string result = foo.SerializeObjectString(std::back_inserter(errorList));
			assert(errorList.size() == 0);

			// Deserialization Process
			Foo deserialized;
			deserialized.DeserializeObject(result, std::back_inserter(errorList));
			assert(errorList.size() == 0);
		};
	};
}
