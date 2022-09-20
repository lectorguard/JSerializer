#include <cstdint>
#define BOOST_UT_DISABLE_MODULE // After updgrading to VS 2022 we can use modules
#include "boost/ut.hpp"
#include "JSerializer.h"
#include <iostream>
#include "list"

namespace SubClasses_Test 
{
    CREATE_DEFAULT_JSER_MANAGER_TYPE(JSERManager);
    using namespace jser;

    struct Foo : public JSerializable
    {
        Foo() { };
		JserChunkAppender AddItem() override
		{
			return JSerializable::AddItem().Append(JSER_ADD(JSERManager, foo, obj1, obj2, obj3));
		}


        struct Foo1 : public JSerializable
        {
            Foo1() { };

			JserChunkAppender AddItem() override
			{
				return JSerializable::AddItem().Append(JSER_ADD(JSERManager, foo));
			}

            struct Foo2 : public JSerializable
            {
                Foo2() {  };

				JserChunkAppender AddItem() override
				{
					return JSerializable::AddItem().Append(JSER_ADD(JSERManager, foo));
				}

                struct Foo3 : public JSerializable
                {
                    Foo3() {  };
					JserChunkAppender AddItem() override
					{
						return JSerializable::AddItem().Append(JSER_ADD(JSERManager, foo, obj4));
					}

                    struct Foo4 : public JSerializable
                    {
                        Foo4() { };
						JserChunkAppender AddItem() override
						{
							return JSerializable::AddItem().Append(JSER_ADD(JSERManager, foo));
						}

                        int32_t foo = 5;
                    };
                    Foo4 obj4;
                    int32_t foo = 5;
                };
                int32_t foo = 5;
            };
            int32_t foo = 5;
        };

        void compare(const Foo& Rhs)
        {
            using namespace boost::ut;
            expect(foo == Rhs.foo);
            expect(obj1.foo == Rhs.obj1.foo);
            expect(obj2.foo == Rhs.obj2.foo);
            expect(obj3.foo == Rhs.obj3.foo);
            expect(obj3.obj4.foo == Rhs.obj3.obj4.foo);
        };

    private:
        int32_t foo = 5;
        Foo1 obj1;
        Foo1::Foo2 obj2;
        Foo1::Foo2::Foo3 obj3;
    };


    boost::ut::suite SubClasses_Test = []
    {
        using namespace boost::ut;

        "sub classes"_test = [] 
        {
            Foo foo;
            std::list<JSerError> errorList;
            std::string result = foo.SerializeObjectString(std::back_inserter(errorList));
            expect(errorList.size() == 0) << "Serialization of primitives throws error";
            Foo deserialized;
            deserialized.DeserializeObject(result, std::back_inserter(errorList));
            expect(errorList.size() == 0) << "Deserialization of primitives throws error";
            foo.compare(deserialized);
        };

        "sub classes from string"_test = [] 
        {
            Foo foo;
            std::list<JSerError> errorList;
            std::string result = foo.SerializeObjectString(std::back_inserter(errorList));
            expect(errorList.size() == 0) << "Serialization of primitives throws error";
            const char* expectedResult = R"({"foo":5,"obj1":{"foo":5},"obj2":{"foo":5},"obj3":{"foo":5,"obj4":{"foo":5}}})";
            expect(!result.compare(expectedResult));

            Foo deserialized;
            deserialized.DeserializeObject(expectedResult, std::back_inserter(errorList));
            expect(errorList.size() == 0) << "Deserialization of primitives throws error";
            foo.compare(deserialized);
        };
    };
};

