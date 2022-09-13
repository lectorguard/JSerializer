#define BOOST_UT_DISABLE_MODULE // After updgrading to VS 2022 we can use modules
#include "boost/ut.hpp"
#include "JSerializer.h"
#include <iostream>
#include <array>
#include <vector>
#include <deque>
#include <forward_list>
#include <list>
#include <stack>
#include <queue>
#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <bitset>
#include <valarray>
#include <tuple>


namespace Stacked_Collections
{

	struct Foo0 : JSerializable
	{
		int16_t foo_int = 44;
		Foo0(int16_t _int) : foo_int(_int)
		{ 
			JSER_ADD(foo_int);
		}
		Foo0()
		{
			JSER_ADD(foo_int);
		}
	};

	struct FooSet : JSerializable
	{
		std::set<bool> foo_set;
		FooSet(bool fst, bool snd, bool trd)
		{
			foo_set.emplace(fst);
			foo_set.emplace(snd);
			foo_set.emplace(trd);
			JSER_ADD(foo_set);
		}
		FooSet()
		{
			JSER_ADD(foo_set);
		}
	};

	struct Test : JSerializable
	{
		std::map<std::string, std::vector<std::stack<Foo0>>> foo_nested_collections;
		std::array<FooSet,3> foo_set_arr;

		Test()
		{
			JSER_ADD(foo_nested_collections, foo_set_arr);
		}
	};


	boost::ut::suite Collections_Test = [] {
		using namespace boost::ut;

		"nested collections "_test = []()
		{
			Test test;
			std::stack<Foo0> foo_stack;
			foo_stack.push(Foo0(12));
			foo_stack.push(Foo0(7));
			foo_stack.push(Foo0(5));

			auto f1 = FooSet(false, true, false);
			auto f2 = FooSet(true, true, false);
			auto f3 = FooSet(false, true, true);

			//test.foo_set_arr = { f1, f2, f3 };

			test.foo_set_arr[0] = FooSet(false, true, false);
			test.foo_set_arr[1] = f2;
			test.foo_set_arr[2] = f3;

			auto copied_stack = foo_stack;
			copied_stack.pop();

			test.foo_nested_collections = { {"hello", {foo_stack, foo_stack}}, {"world", {copied_stack, copied_stack}} };


			std::list<JSerError> errorList;
			std::string result = test.SerializeObjectString(std::back_inserter(errorList));
			expect(errorList.size() == 0) << "Serialization of primitives throws error";

			std::cout << result << std::endl;
			Test deserialized;
			deserialized.DeserializeObject(result, std::back_inserter(errorList));
			expect(errorList.size() == 0) << "Deserialization of primitives throws error";



			for (const auto& [key, value] : test.foo_nested_collections)
			{
				bool result = std::equal(test.foo_nested_collections[key].begin(), test.foo_nested_collections[key].end(), deserialized.foo_nested_collections[key].begin(),
					[](auto lhs, auto rhs)
					{
						while (!lhs.empty())
						{
							if (lhs.top().foo_int != rhs.top().foo_int) return false;
							lhs.pop();
							rhs.pop();
						}
						return true;
					});
				expect(result);
			}


			bool foo_set_result = std::equal(test.foo_set_arr.begin(), test.foo_set_arr.end(), deserialized.foo_set_arr.begin(), [](FooSet Rhs, FooSet Lhs)
				{
					return Rhs.foo_set == Lhs.foo_set;
				});
			expect(foo_set_result);

			
		};

		
	};
};