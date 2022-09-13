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

		Foo0() {};
		Foo0(int foo): foo_int(foo){}

		JserChunkAppender AddItem() override
		{
			return JSerializable::AddItem().Append(JSER_ADD(foo_int));
		}

	};

	struct FooBitSet : JSerializable
	{
		std::bitset<11> foo_bitset;

		FooBitSet() {};
		FooBitSet(std::string bits) : foo_bitset(std::bitset<11>(bits)) {};

		JserChunkAppender AddItem() override
		{
			return JSerializable::AddItem().Append(JSER_ADD(foo_bitset));
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
		}

		FooSet(){}

		JserChunkAppender AddItem() override
		{
			return JSerializable::AddItem().Append(JSER_ADD(foo_set));
		}

	};

	struct Test : JSerializable
	{
		std::map<std::string, std::vector<std::stack<Foo0>>> foo_nested_collections;
		std::array<FooSet, 3> foo_set_arr;
		std::valarray<std::forward_list<FooBitSet>> foo_valarray;
		std::deque<std::tuple<FooBitSet, Foo0>> foo_deque;
		std::queue<std::priority_queue<int32_t>> foo_queue;
		struct compare
		{
			bool operator()(Foo0 Lhs, Foo0 Rhs) const
			{
				return Lhs.foo_int < Rhs.foo_int;
			}
		};
		std::list<std::multiset<Foo0, compare>> foo_list;
	
		Test() {};

		JserChunkAppender AddItem() override
		{
			return JSerializable::AddItem().Append(JSER_ADD(foo_set_arr, foo_nested_collections, foo_valarray, foo_deque, foo_queue, foo_list));
		}

	};


	boost::ut::suite Collections_Test = [] {
		using namespace boost::ut;

		"nested collections"_test = []()
		{
			Test test;
			std::stack<Foo0> foo_stack;
			foo_stack.push(Foo0(12));
			foo_stack.push(Foo0(7));
			foo_stack.push(Foo0(5));

			auto f2 = FooSet(true, true, false);
			auto f3 = FooSet(false, true, true);

			//test.foo_set_arr = { f1, f2, f3 };

			test.foo_set_arr[0] = FooSet(false, true, false);
			test.foo_set_arr[1] = f2;
			test.foo_set_arr[2] = f3;

			auto copied_stack = foo_stack;
			copied_stack.pop();

			test.foo_nested_collections = { {"hello", {foo_stack, foo_stack}}, {"world", {copied_stack, copied_stack}} };
			test.foo_valarray = { {FooBitSet("00001111000")},{FooBitSet("00001111100")},{FooBitSet("00001111010")} };
			test.foo_deque = { {FooBitSet("00001111000"), Foo0(45)}, {FooBitSet("00001111100"), Foo0(55)}};
			std::priority_queue<int32_t> foo_prio_queue;
			foo_prio_queue.push(4);
			foo_prio_queue.push(40);
			foo_prio_queue.push(400);
			foo_prio_queue.push(4000);
			test.foo_queue.push(foo_prio_queue);
			test.foo_queue.push(foo_prio_queue);
			test.foo_queue.push(foo_prio_queue);
			test.foo_queue.push(foo_prio_queue);
			test.foo_list = { {Foo0(12),Foo0(120),Foo0(1200),Foo0(11),Foo0(10) }, {Foo0(12),Foo0(120),Foo0(1200),Foo0(11),Foo0(10)}, {Foo0(12),Foo0(120),Foo0(1200),Foo0(11),Foo0(10)} };


			std::list<JSerError> errorList;
			std::string result = test.SerializeObjectString(std::back_inserter(errorList));
			expect(errorList.size() == 0) << "Serialization of primitives throws error";

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

			bool foo_valarray_result = std::equal(std::begin(test.foo_valarray), std::end(test.foo_valarray), std::begin(deserialized.foo_valarray), [](std::forward_list<FooBitSet> Lhs, std::forward_list<FooBitSet> Rhs)
				{
					return std::equal(std::begin(Lhs), std::end(Lhs), std::begin(Rhs), [](FooBitSet Lhs, FooBitSet Rhs)
						{
							return !Rhs.foo_bitset.to_string().compare(Lhs.foo_bitset.to_string());
						});
				});
			expect(foo_valarray_result);

			bool foo_deque_result = std::equal(std::begin(test.foo_deque), std::end(test.foo_deque), std::begin(deserialized.foo_deque), [](std::tuple<FooBitSet, Foo0> Lhs, std::tuple<FooBitSet, Foo0> Rhs)
				{
					return !std::get<0>(Lhs).foo_bitset.to_string().compare(std::get<0>(Rhs).foo_bitset.to_string()) &&
						std::get<1>(Lhs).foo_int == std::get<1>(Rhs).foo_int;
				
				});
			expect(foo_deque_result);


			while (!test.foo_queue.empty())
			{
				while (!test.foo_queue.front().empty())
				{
					int32_t lhsElem = test.foo_queue.front().top();
					int32_t rhsElem = deserialized.foo_queue.front().top();
					expect(lhsElem == rhsElem);
					test.foo_queue.front().pop();
					deserialized.foo_queue.front().pop();
				}
				test.foo_queue.pop();
				deserialized.foo_queue.pop();
			}

			bool foo_list_result = std::equal(std::begin(test.foo_list), std::end(test.foo_list), std::begin(deserialized.foo_list), [](std::multiset<Foo0, Test::compare> Lhs, std::multiset<Foo0, Test::compare> Rhs)
				{
					return std::equal(std::begin(Lhs), std::end(Lhs), std::begin(Rhs), [](Foo0 Lhs, Foo0 Rhs)
						{
							return Lhs.foo_int == Rhs.foo_int;
						});
				});
			expect(foo_list_result);

		};


	};
};