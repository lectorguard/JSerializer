#define BOOST_UT_DISABLE_MODULE // After updgrading to VS 2022 we can use modules
#include "boost/ut.hpp"
#include <cassert>
#include "JSerializer.h"

namespace ErrorHandlingExample
{
	CREATE_DEFAULT_JSER_MANAGER_TYPE(JSERManager);

	// Produces error, because AddItem is not overriden and no item was added to JSerializable
	struct Foo : jser::JSerializable
	{
		int foo = 42;
	};

	boost::ut::suite Suite = [] {
		using namespace boost::ut;
		"main"_test = []
		{
			Foo foo;
			std::list<jser::JSerError> errorList;
			const std::string result = foo.SerializeObjectString(std::back_inserter(errorList));
			for (jser::JSerError err : errorList)
			{
				// Here you can handle the different errors
			}
			assert(errorList.size() == 1);
		};
	};
}
