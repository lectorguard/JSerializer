#define BOOST_UT_DISABLE_MODULE // After updgrading to VS 2022 we can use modules
#include "boost/ut.hpp"
#include "JSerializer.h"


namespace Association_Test
{
    CREATE_DEFAULT_JSER_MANAGER_TYPE(JSERManager);
    using namespace jser;

    struct Foo : JSerializable
    {
        Foo(){}

		JserChunkAppender AddItem() override
		{
			return JSerializable::AddItem().Append(JSER_ADD(JSERManager, foo_bool, foo_short_int, foo_u_short_int, foo_u_int));
		}
    
        bool foo_bool = false;
        short int foo_short_int = 9;
        unsigned short int foo_u_short_int = 99;
        unsigned int foo_u_int = 42;
    
        void compare(const Foo& Rhs)
        {
            using namespace boost::ut;
            expect(foo_bool == Rhs.foo_bool);
            expect(foo_short_int == Rhs.foo_short_int);
            expect(foo_u_short_int == Rhs.foo_u_short_int);
            expect(foo_u_int == Rhs.foo_u_int);
        };
    };
    
    struct Foo0 : JSerializable
    {
        Foo0()
        {
        }
		JserChunkAppender AddItem() override
		{
			return JSerializable::AddItem().Append(JSER_ADD(JSERManager, foo));
		}

        Foo foo;
    };
    
    struct Foo1 : JSerializable
    {
        Foo1()
        {
        }
		JserChunkAppender AddItem() override
		{
			return JSerializable::AddItem().Append(JSER_ADD(JSERManager, foo));
		}
        
        Foo0 foo;
    };
    
    struct Foo2 : JSerializable
    {
        Foo2()
        {
        }

		JserChunkAppender AddItem() override
		{
			return JSerializable::AddItem().Append(JSER_ADD(JSERManager, foo));
		}
        Foo1 foo;
    };
    
    struct Foo3 : JSerializable
    {
        Foo3()
        {
        }
		JserChunkAppender AddItem() override
		{
			return JSerializable::AddItem().Append(JSER_ADD(JSERManager,  foo));
		}
        Foo2 foo;
    };
    
    struct Foo4 : JSerializable
    {
        Foo4()
        {
        }
		JserChunkAppender AddItem() override
		{
			return JSerializable::AddItem().Append(JSER_ADD(JSERManager, foo));
		}
        Foo3 foo;
    };

    boost::ut::suite Association_Test = [] {
        using namespace boost::ut;

        "object association"_test = [] {
            Foo4 foo;
            std::list<JSerError> errorList;
            std::string result = foo.SerializeObjectString(std::back_inserter(errorList));
            expect(errorList.size() == 0) << "Serialization of many object associations throws error";
            
            Foo4 deserialized;
            deserialized.DeserializeObject(result, std::back_inserter(errorList));
            expect(errorList.size() == 0) << "Deserialization of many object associations throws error";
            foo.foo.foo.foo.foo.foo.compare(deserialized.foo.foo.foo.foo.foo);
        };

        "object association test from string"_test = [] {
            Foo4 foo;
            std::list<JSerError> errorList;
            std::string result = foo.SerializeObjectString(std::back_inserter(errorList));
            expect(errorList.size() == 0) << "Serialization of many object associations throws error";
            const char* expectedResult = R"({"foo":{"foo":{"foo":{"foo":{"foo":{"foo_bool":false,)"
                                         R"("foo_short_int":9,"foo_u_int":42,"foo_u_short_int":99}}}}}})";
            expect(!result.compare(expectedResult));

            Foo4 deserialized;
            deserialized.DeserializeObject(expectedResult, std::back_inserter(errorList));
            expect(errorList.size() == 0) << "Deserialization of many object associations throws error";
            foo.foo.foo.foo.foo.foo.compare(deserialized.foo.foo.foo.foo.foo);
        };
    };
}

