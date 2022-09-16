#define BOOST_UT_DISABLE_MODULE // After updgrading to VS 2022 we can use modules
#include "boost/ut.hpp"
#include "JSerializer.h"


namespace CircularDependency_Test
{
    // store a set of all pointers we already know. if it contains a pointer we already know besides nullptr then throw circular dependency error


    //struct Foo;
    //struct Foo1;
    //
    //struct Foo : JSerializable
    //{
    //    Foo()
    //    {
    //        JSER_ADD(JSERManager, foo_int, foo1_obj);
    //    }
    //
    //    int32_t foo_int = 15;
    //    Foo1 foo1_obj;
    //};
    //
    //struct Foo1 : JSerializable
    //{
    //    Foo1()
    //    {
    //        JSER_ADD(JSERManager, foo1_int, foo_obj);
    //    }
    //    
    //    int32_t foo1_int = 15;
    //    Foo foo_obj;
    //};
    //
    //
    //
    //boost::ut::suite CircularDependency_Test = [] {
    //    using namespace boost::ut;
    //
    //    "circular dependency test"_test = [] 
    //    {
    //        Foo foo;
    //        std::list<JSerError> errorList;
    //        std::string result = foo.SerializeObjectString(std::back_inserter(errorList));
    //        expect(errorList.size() == 0) << "Serialization of many object associations throws error";
    //
    //        Foo deserialized;
    //        deserialized.DeserializeObject(result, std::back_inserter(errorList));
    //        expect(errorList.size() == 0) << "Deserialization of many object associations throws error";
    //    };
    //
    //    //"object association test from string"_test = [] {
    //    //    Foo4 foo;
    //    //    std::list<JSerError> errorList;
    //    //    std::string result = foo.SerializeObjectString(std::back_inserter(errorList));
    //    //    expect(errorList.size() == 0) << "Serialization of many object associations throws error";
    //    //    const char* expectedResult = R"({"foo":{"foo":{"foo":{"foo":{"foo":{"foo_bool":false,)"
    //    //        R"("foo_short_int":9,"foo_u_int":42,"foo_u_short_int":99}}}}}})";
    //    //    expect(!result.compare(expectedResult));
    //    //
    //    //    Foo4 deserialized;
    //    //    deserialized.DeserializeObject(expectedResult, std::back_inserter(errorList));
    //    //    expect(errorList.size() == 0) << "Deserialization of many object associations throws error";
    //    //    foo.foo.foo.foo.foo.foo.compare(deserialized.foo.foo.foo.foo.foo);
    //    //};
    //
    //};
}