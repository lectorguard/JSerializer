#include <cstdint>
#define BOOST_UT_DISABLE_MODULE // After updgrading to VS 2022 we can use modules
#include "boost/ut.hpp"
#include "JSerializer.h"
#include "list"

namespace PrimitiveDatatypes_Test 
{
    CREATE_DEFAULT_JSER_MANAGER_TYPE(JSERManager);

    using namespace jser;
    enum class FooEnum : uint8_t
    {
        FOO, FOO1, FOO2, FOO3, FOO4, FOO5, FOO6, FOO7, FOO8, FOO9,
    };
    
    
    struct Foo : JSerializable
    {
    
        Foo()
        {
        }

		JserChunkAppender AddItem() override
		{
			return JSerializable::AddItem()
                .Append(JSER_ADD(JSERManager, foo_bool, foo_short_int, foo_u_short_int, foo_u_int, foo_int))
                .Append(JSER_ADD(JSERManager, foo_long_int, foo_u_long_int, foo_long_long_int, foo_u_long_long_int, foo_signed_char))
                .Append(JSER_ADD(JSERManager, foo_unsigned_char, foo_float, foo_double, foo_long_double, foo_wchar, foo_enum));
		}

    
        bool foo_bool = false;
        short int foo_short_int = 9;
        unsigned short int foo_u_short_int = 99;
        unsigned int foo_u_int = 42;
        int foo_int = 47; 
        long int foo_long_int = 194846;
        unsigned long int foo_u_long_int = 1;
        long long int foo_long_long_int = 94486584611;
        unsigned long long int  foo_u_long_long_int = 45681631553156;
        signed char foo_signed_char = ' ';
        unsigned char foo_unsigned_char = 'r';
        float foo_float = 15.54f;
        double foo_double = 186156.56538;
        long double foo_long_double = 15651365.516851561;
        wchar_t foo_wchar = 'g';
        FooEnum foo_enum = FooEnum::FOO3;
    
        void compare(const Foo& Rhs)
        {
            using namespace boost::ut;
            expect(foo_bool == Rhs.foo_bool);
            expect(foo_short_int == Rhs.foo_short_int);
            expect(foo_u_short_int == Rhs.foo_u_short_int);
            expect(foo_u_int == Rhs.foo_u_int);
            expect(foo_int == Rhs.foo_int);
            expect(foo_long_int == Rhs.foo_long_int);
            expect(foo_u_long_int == Rhs.foo_u_long_int);
            expect(foo_long_long_int == Rhs.foo_long_long_int);
            expect(foo_u_long_long_int == Rhs.foo_u_long_long_int);
            expect(foo_signed_char == Rhs.foo_signed_char);
            expect(foo_unsigned_char == Rhs.foo_unsigned_char);
            expect(foo_float == Rhs.foo_float);
            expect(foo_double == Rhs.foo_double);
            expect(foo_long_double == Rhs.foo_long_double);
            expect(foo_wchar == Rhs.foo_wchar);
            expect(foo_enum == Rhs.foo_enum);
        };
    };

    boost::ut::suite PrimitiveDatatypes_Test = [] {
        using namespace boost::ut;

        "primitive data types"_test = [] {
            Foo foo;
            foo.foo_bool = true;
            std::list<JSerError> errorList;
            std::string result = foo.SerializeObjectString(std::back_inserter(errorList));
            expect(errorList.size() == 0) << "Serialization of primitives throws error";
            Foo deserialized;
            deserialized.DeserializeObject(result, std::back_inserter(errorList));
            expect(errorList.size() == 0) << "Deserialization of primitives throws error";
            foo.compare(deserialized);
        };


        "primitive data types from string"_test = [] 
        {
            Foo foo;
            std::list<JSerError> errorList;
            std::string result = foo.SerializeObjectString(std::back_inserter(errorList));
            const char* expectedResult = R"({"foo_bool":false,"foo_double":186156.56537999999,)"
                                         R"("foo_enum":3,"foo_float":15.539999961853027,)"
                                         R"("foo_int":47,"foo_long_double":15651365.516851561,)"
                                         R"("foo_long_int":194846,"foo_long_long_int":94486584611,)"
                                         R"("foo_short_int":9,"foo_signed_char":32,"foo_u_int":42,)"
                                         R"("foo_u_long_int":1,"foo_u_long_long_int":45681631553156,)"
                                         R"("foo_u_short_int":99,"foo_unsigned_char":114,"foo_wchar":103})";
            expect(!result.compare(expectedResult));

            Foo deserialized;
            deserialized.DeserializeObject(expectedResult, std::back_inserter(errorList));
            foo.compare(deserialized);
        };
        
    };
};

