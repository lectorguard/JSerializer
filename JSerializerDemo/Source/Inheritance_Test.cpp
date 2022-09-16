#define BOOST_UT_DISABLE_MODULE // After updgrading to VS 2022 we can use modules
#include "boost/ut.hpp"
#include "JSerializer.h"
#include <iostream>
#include <cstdint>
#include <string>


namespace Inheritance_Test 
{
    CREATE_DEFAULT_JSER_MANAGER_TYPE(JSERManager);
    struct Foo : JSerializable
    {
        Foo()
        {
        };

		JserChunkAppender AddItem() override
		{
			return JSerializable::AddItem().Append(JSER_ADD(JSERManager, foo));
		}
        uint32_t foo = 9;
    };
    
    struct Foo1 : Foo
    {
        Foo1()
        {
        }

		JserChunkAppender AddItem() override
		{
			return Foo::AddItem().Append(JSER_ADD(JSERManager, foo1));
		}
        uint32_t foo1 = 76543;
    };
    
    struct Foo2 : Foo1
    {
        Foo2()
        {
        }

		JserChunkAppender AddItem() override
		{
			return Foo1::AddItem().Append(JSER_ADD(JSERManager, foo2));
		}

        uint32_t foo2 = 5678;
    };
    
    struct Foo3 : Foo2
    {
        Foo3()
        {
        }

		JserChunkAppender AddItem() override
		{
			return Foo2::AddItem().Append(JSER_ADD(JSERManager, foo3));
		}
        uint32_t foo3 = 782;
    }; 
    
    struct Foo4 : Foo3
    {
        Foo4()
        {
        }
		JserChunkAppender AddItem() override
		{
			return Foo3::AddItem().Append(JSER_ADD(JSERManager, foo4));
		}

        uint32_t foo4 = 4526;
    }; 
    
    struct Foo5 : Foo4
    {
        Foo5()
        {
        }

		JserChunkAppender AddItem() override
		{
			return Foo4::AddItem().Append(JSER_ADD(JSERManager, foo5));
		}
        uint32_t foo5 = 78978;
    };
    
    struct Foo6 : Foo5
    {
        Foo6()
        {
        }

		JserChunkAppender AddItem() override
		{
			return Foo5::AddItem().Append(JSER_ADD(JSERManager, foo6));
		}
        uint32_t foo6 = 6584;
    };
    
    struct Foo7 : Foo6
    {
        Foo7()
        {
        }
		JserChunkAppender AddItem() override
		{
			return Foo6::AddItem().Append(JSER_ADD(JSERManager, foo7));
		}

        uint32_t foo7 = 99;
    }; 
    
    struct Foo8 : Foo7
    {
        Foo8(){}
		JserChunkAppender AddItem() override
		{
			return Foo7::AddItem().Append(JSER_ADD(JSERManager, foo8));
		}

        uint32_t foo8 = 11;
    }; 
    
    struct Foo9 : Foo8
    {
        Foo9(){}
		JserChunkAppender AddItem() override
		{
			return Foo8::AddItem().Append(JSER_ADD(JSERManager, foo9));
		}
        uint32_t foo9 = 47;
    }; 
    
    struct Foo10 : Foo9
    {
        Foo10()
        {
        }
		JserChunkAppender AddItem() override
		{
			return Foo9::AddItem().Append(JSER_ADD(JSERManager, foo10));
		}
        uint32_t foo10 = 52;
    
        void compare(const Foo10& Rhs)
        {
            using namespace boost::ut;
            expect(foo10 == Rhs.foo10);
            expect(foo9 == Rhs.foo9);
            expect(foo8 == Rhs.foo8);
            expect(foo7 == Rhs.foo7);
            expect(foo6 == Rhs.foo6);
            expect(foo5 == Rhs.foo5);
            expect(foo4 == Rhs.foo4);
            expect(foo3 == Rhs.foo3);
            expect(foo2 == Rhs.foo2);
            expect(foo1 == Rhs.foo1);
            expect(foo == Rhs.foo);       
        }
    };

    struct Boo : JSerializable
    {
        uint32_t boo = 52;
    };

    struct Boo1 : Boo
    {
        uint32_t boo1 = 59;
    };

    struct Boo2 : Boo1
    {
        Boo2()
        {
        }
		JserChunkAppender AddItem() override
		{
			return Boo1::AddItem().Append(JSER_ADD(JSERManager, boo, boo1, boo2));
		}

        uint32_t boo2 = 7;


        void compare(const Boo2& Rhs)
        {
            using namespace boost::ut;
            expect(boo2 == Rhs.boo2);
            expect(boo1 == Rhs.boo1);
            expect(boo == Rhs.boo);
        }
    };

    boost::ut::suite Inheritance_Test = [] {

        using namespace boost::ut;
        
        "inheritance"_test = [] 
        {
            Foo10 foo;
            foo.foo += 4;
            std::list<JSerError> errorList;
            std::string result = foo.SerializeObjectString(std::back_inserter(errorList));
            expect(errorList.size() == 0) << "Serialization for inherited object throws error";

            Foo10 deserialized;
            deserialized.DeserializeObject(result, std::back_inserter(errorList));
            expect(errorList.size() == 0) << "Deserialization for inherited object throws error";
            foo.compare(deserialized);
        };

        "Add Jser values from parent"_test = []
        {
            Boo2 boo;
            std::list<JSerError> errorList;
            std::string result = boo.SerializeObjectString(std::back_inserter(errorList));
            expect(errorList.size() == 0) << "Serialization for inherited object throws error";
            
            Boo2 deserialized;
            deserialized.DeserializeObject(result, std::back_inserter(errorList));
            expect(errorList.size() == 0) << "Deserialization for inherited object throws error";
            boo.compare(deserialized);
        };

        "Compare strings inheritance"_test = []
        {
            Foo10 foo;
            std::list<JSerError> errorList;
            std::string result = foo.SerializeObjectString(std::back_inserter(errorList));
            const char* expectedResult = R"({"foo":9,"foo1":76543,"foo10":52,"foo2":5678,"foo3":782,"foo4":4526,"foo5":78978,"foo6":6584,"foo7":99,"foo8":11,"foo9":47})";
            expect(!result.compare(expectedResult));
           
            Foo10 deserialized;
            deserialized.DeserializeObject(expectedResult, std::back_inserter(errorList));
            deserialized.compare(foo);
        };


        "Compare strings Jser values from parent"_test = []
        {
            Boo2 foo;
            std::list<JSerError> errorList;
            std::string result = foo.SerializeObjectString(std::back_inserter(errorList));
            const char* expectedResult = R"({"boo":52,"boo1":59,"boo2":7})";
            expect(!result.compare(expectedResult));
            
            Boo2 deserialized;
            deserialized.DeserializeObject(expectedResult, std::back_inserter(errorList));
            deserialized.compare(foo);
        };


    };
};

