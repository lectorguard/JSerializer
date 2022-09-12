#define BOOST_UT_DISABLE_MODULE // After updgrading to VS 2022 we can use modules
#include "boost/ut.hpp"
#include "JSerializer.h"
#include <iostream>
#include <queue>
#include <map>

namespace Error_Test
{

	template<typename Test, template<typename...> class Ref>
	struct is_specialization_v : std::false_type {};

	template<template<typename...> class Ref, typename... Args>
	struct is_specialization_v<Ref<Args...>, Ref> : std::true_type {};
	
	boost::ut::suite Error_Test = [] 
	{
		using namespace boost::ut;
		using namespace boost::ut::bdd;

		"error"_test = []()
		{
			struct Foo : JSerializable
			{
				Foo()
				{
					JSER_ADD(foo_bool, foo_short_int, foo_u_short_int, foo_u_int);
				}

				bool foo_bool = false;
				short int foo_short_int = 9;
				unsigned short int foo_u_short_int = 99;
				unsigned int foo_u_int = 42;
				int foo_int = 47;
			};

			using namespace boost::ut;
			when("there is an invalid json") = [&]()
			{
				
				const char* invalid_json = R"({"foo_bool":false,"foo_short_)";
				then("a deserialization json error should be returned") = [&]()
				{
					Foo deserialized;
					std::list<JSerError> errorList;
					deserialized.DeserializeObject(invalid_json, std::back_inserter(errorList));
					expect(errorList.size() == 1) << "Deserialization of invalid json should throw error";
					expect(errorList.front().Error == JSerErrorTypes::JSON_ERROR);
				};
			};

			when("there is missing a JSER_ADD call") = []()
			{
				struct Foo1 : JSerializable
				{
					Foo1() {};
					int foo = 25;
				};

				then("a missing setup error should be returned on serialization") = [&]()
				{
					Foo1 foo;
					std::list<JSerError> errorList;
					const std::string result = foo.SerializeObjectString(std::back_inserter(errorList));
					expect(errorList.size() == 1) << "When a setup call is missing an error should be reutrned on serialization";
					expect(errorList.front().Error == JSerErrorTypes::SETUP_MISSING_ERROR);

				};

				then("a missing setup error should be returned on deserialization") = [&]()
				{
					Foo1 deserialized;
					std::list<JSerError> errorList;
					deserialized.DeserializeObject(R"({})", std::back_inserter(errorList));
					expect(errorList.size() == 1) << "When a setup call is missing an error should be reutrned on deserialization";
					expect(errorList.front().Error == JSerErrorTypes::SETUP_MISSING_ERROR);
				};
			};

			when("there are missing elements inside the json") = []()
			{
				const char* invalid_json = R"({"foo_bool":false})";
				then("the deserialization process should return an error for each missing element") = [&]()
				{
					Foo deserialized;
					std::list<JSerError> errorList;
					deserialized.DeserializeObject(invalid_json, std::back_inserter(errorList));

					expect(errorList.size() == 3) << "Deserialization of invalid json should throw error";
					std::for_each(errorList.begin(), errorList.end(), 
						[](const JSerError error) 
						{
							expect(error.Error == JSerErrorTypes::MEMBER_ERROR); 
						}
					);
				};
			};

// 			when("there is a referenced object which is not inheriting from JSerializable") = [&]()
// 			{
// 				struct FooX
// 				{
// 					FooX() { };
// 					char foo_char = 'c';
// 				};
// 				
// 				struct FooY : JSerializable
// 				{
// 					FooY() { JSER_ADD(foo, foo_objx); };
// 					int foo = 25;
// 					FooX foo_objx;
// 				};
// 
// 
// 				then("the code will not compile") = [&]()
// 				{
// 					// This leads automatically to a compile error 
// 				};
// 			};

			when("there is a referenced object which is not inheriting from JSerializable but inheriting from another class") = [&]()
			{
				struct FooX
				{
					bool foo_bool = false;
				};

				struct FooY : FooX
				{
					FooY() {  };
					virtual ~FooY() {};
					char foo_char = 'c';
				};
				
				struct FooZ : JSerializable
				{
					FooZ() { JSER_ADD(foo, foo_objx); };
					int foo = 25;
					FooY foo_objx;
				};

				then("a polymorphic error should be returned on serialization") = []()
				{
					FooZ foo;
					std::list<JSerError> errorList;
					const std::string result = foo.SerializeObjectString(std::back_inserter(errorList));
					expect(errorList.size() == 1) << "When a referenced object is not inheriting from JSerializer an error is returned";
					expect(errorList.front().Error == JSerErrorTypes::POLYMORPHIC_ERROR);
				};

				const char* invalid_json = R"({ "foo":25, "foo_objx" : {"foo_char":99} })";
				then("a polymorphic error should be returned on deserialization") = [invalid_json]()
				{
					FooZ deserialized;
					std::list<JSerError> errorList;
					deserialized.DeserializeObject(invalid_json, std::back_inserter(errorList));
					expect(errorList.size() == 1) << "When a referenced object is not inheriting from JSerializer an error is returned";
					expect(errorList.front().Error == JSerErrorTypes::POLYMORPHIC_ERROR);
				};
			};

			when("you want to deserialize a stack ") = []()
			{
				//std::vector<std::priority_queue<int>> help;
				//std::cout << "Value types " << is_specialization_v<decltype(help), std::vector>() << std::endl;



				//std::priority_queue<float> foo_prio_queue;
				//float kartoffel = 12.5f;
				//std::map<int, int> hello;
				//test t;

				//std::cout << "Value types " << is_single_value_type<decltype(foo_prio_queue)>() << std::endl;
				//std::queue<float> foo_queue;
				//foo_queue.push(14.0f);
				//foo_queue.push(4.0f);
				//foo_queue.push(124.0f);
				//foo_queue.push(124.0f);
				//foo_queue.push(154.0f);
				//
				//foo_queue.

			};
		};
	};
}


