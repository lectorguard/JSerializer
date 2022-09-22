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


#define SPECIALIZATION_CHECK(type, correctType, a, b, c, d) expect(is_specialization<std::decay_t<decltype(correctType)>, type>()); \
                                                            expect(!is_specialization<std::decay_t<decltype(a)>, type>()); \
                                                            expect(!is_specialization<std::decay_t<decltype(b)>, type>()); \
                                                            expect(!is_specialization<std::decay_t<decltype(c)>, type>()); \
                                                            expect(!is_specialization<std::decay_t<decltype(d)>, type>());



namespace Collections_Test
{
    CREATE_DEFAULT_JSER_MANAGER_TYPE(JSERManager);
    using namespace jser;

    boost::ut::suite Collections_Test = [] {
        using namespace boost::ut;

        "simple iterable types "_test = []()
        {
            


            struct CollectionTest : JSerializable
            {
                CollectionTest() { }

				JserChunkAppender AddItem() override
				{
					return JSerializable::AddItem()
                        .Append(JSER_ADD(JSERManager, foo, foo_list, foo_vector, foo_forward_list, foo_set, foo_multi_set))
                        .Append(JSER_ADD(JSERManager, foo_valarry, foo_deque, foo_forward_list, foo_unordered_set, foo_unordered_multiset))
                        .Append(JSER_ADD(JSERManager, foo_map, foo_unordered_map, foo_multimap, foo_unordered_multimap, foo_tuple, foo_bitset))
                        .Append(JSER_ADD(JSERManager, foo_stack, foo_queue, foo_priority_queue));
				}

                std::array<int, 3> foo = { 0,0,0 };
                std::list<int> foo_list = { 45,48,513,8,61,86,156 };
				std::vector<double> foo_vector = { 16561.4681,168.168,416811.68186,4648.864 };
				std::forward_list<bool> foo_forward_list = { false,true, true, false };
				std::set<int32_t> foo_set = { 5,4,5658,6 };
				std::multiset<int32_t> foo_multi_set = { 185,165,65,16,81,65,65,65 };
				std::valarray<int32_t> foo_valarry = { 11565,156,651,186,68658,616,1650 };   
                std::deque<unsigned int> foo_deque = { 17, 26, 45, 78 };
				std::unordered_set<int64_t> foo_unordered_set = { 15,4589,516,4684,6818,568,18 };
				std::unordered_multiset<int64_t> foo_unordered_multiset = { 15,15,15,15,15,15,15,15 };
				
                std::map<std::string, char> foo_map = { {"Hello", 'H'}, {"World", 'W'} };
		        std::unordered_map<uint16_t, int16_t> foo_unordered_map = { {6,6}, {7,7}, {8,8} };
		        std::multimap<char, std::string> foo_multimap = { {'H',"Hello"}, {'H', "Hey"}, {'W', "World"}, {'W', "Whatever"} }; 
		        std::unordered_multimap<char, std::string> foo_unordered_multimap = { {'H',"Hello"}, {'H', "Hey"}, {'W', "World"}, {'W', "Whatever"} }; 

                std::tuple<int, float, char, std::string> foo_tuple = { 3, 3.0f, 'g', "hello world" };
                std::bitset<4> foo_bitset{ "0011" };

				std::stack<int> foo_stack;  
                std::queue<float> foo_queue; 
                std::priority_queue<wchar_t> foo_priority_queue;
            };

            CollectionTest t;
            t.foo[0] = 4;
            t.foo[1] = 3;
            t.foo[2] = 24;
            t.foo_list.push_back(156);
            t.foo_vector.push_back(158551.15616);
            t.foo_forward_list.push_front(false);
            t.foo_forward_list.push_front(false);
            t.foo_set.insert(65);
            t.foo_multi_set.emplace(4557);
            t.foo_valarry[3] = 600;
            t.foo_deque.push_back(5);
            t.foo_unordered_set.insert(155);
            t.foo_unordered_multiset.insert(16);
			t.foo_map.insert({ "!", '!' });
			t.foo_unordered_map.erase(6);
			t.foo_multimap.insert({ 'W', "What" });
			t.foo_unordered_multimap.insert({ 'K', "Keep it short and simple" });
            std::get<1>(t.foo_tuple) = 15.0f;
            t.foo_bitset |= 0b1100;
			t.foo_stack.push(25);
			t.foo_stack.push(33);
			t.foo_stack.push(47);
			t.foo_queue.push(10.3f);
			t.foo_queue.push(12.0f);
			t.foo_queue.push(45351.565130f);
			t.foo_priority_queue.push(24);
			t.foo_priority_queue.push('L');
			t.foo_priority_queue.push('W');

            std::list<JSerError> errorList;
            std::string result = t.SerializeObjectString(std::back_inserter(errorList));
            expect(errorList.size() == 0) << "Serialization of many object associations throws error";

            CollectionTest t2;
            t2.DeserializeObject(result, std::back_inserter(errorList));
            expect(errorList.size() == 0) << "Serialization of many object associations throws error";

            expect(t2.foo == t.foo);
            expect(t2.foo_list == t.foo_list);
            expect(t2.foo_vector == t.foo_vector);
            expect(t2.foo_set == t.foo_set);
            expect(t2.foo_multi_set == t.foo_multi_set);
            expect(std::equal(std::begin(t2.foo_valarry), std::end(t2.foo_valarry), std::begin(t.foo_valarry)));
            expect(t2.foo_deque == t.foo_deque);
            expect(t2.foo_forward_list == t.foo_forward_list);
            expect(t2.foo_unordered_set == t.foo_unordered_set);
            expect(t2.foo_unordered_multiset == t.foo_unordered_multiset);

            bool is_map_equal = t.foo_map == t2.foo_map;
            bool is_unordered_map_equal = t2.foo_unordered_map == t.foo_unordered_map;
            bool is_multimap_equal = t.foo_multimap == t2.foo_multimap;
            bool is_unordered_multimap_equal = t.foo_unordered_multimap == t2.foo_unordered_multimap;
            expect(is_map_equal);
            expect(is_unordered_map_equal);
            expect(is_multimap_equal);
            expect(is_unordered_multimap_equal);

            expect(t2.foo_tuple == t.foo_tuple);
            expect(t2.foo_bitset == t.foo_bitset);

			expect(t2.foo_stack == t.foo_stack);
			expect(t2.foo_queue == t.foo_queue);

            bool is_same_size_priority_queues = t2.foo_priority_queue.size() == t.foo_priority_queue.size();
            expect(is_same_size_priority_queues);
            if (is_same_size_priority_queues)
            {
				while (!t2.foo_priority_queue.empty())
				{
					expect(t.foo_priority_queue.top() == t2.foo_priority_queue.top());
					t.foo_priority_queue.pop();
					t2.foo_priority_queue.pop();
				}
            }
        };

        "check correct type detection"_test = []()
        {
			const std::array<std::string, 2> foo_array = { "hello", "world" };
			float foo_carray[] = { 2.8f,5.4f,6.0f };
            const std::list<int> foo_list = { 45,48,513,8,61,86,156 };
            const std::vector<double> foo_vector = { 16561.4681,168.168,416811.68186,4648.864 };
            std::forward_list<bool> foo_forward_list = { false,true, true, false };
            std::set<int32_t> foo_set = { 5,4,5658,6 };
            const std::multiset<int32_t> foo_multi_set = { 185,165,65,16,81,65,65,65 };
            std::valarray<int32_t> foo_valarry = { 11565,156,651,186,68658,616,1650 };

			std::deque<unsigned int> foo_deque = { 17, 26, 45, 78 };
			std::unordered_set<int64_t> foo_unordered_set = { 15,4589,516,4684,6818,568,18 };
			const std::unordered_multiset<int64_t> foo_unordered_multiset = { 15,15,15,15,15,15,15,15 };
			std::map<std::string, char> foo_map = { {"Hello", 'H'}, {"World", 'W'} };
			std::unordered_map<uint16_t, int16_t> foo_unordered_map = { {6,6}, {7,7}, {8,8} };
			const std::multimap<char, std::string> foo_multimap = { {'H',"Hello"}, {'H', "Hey"}, {'W', "World"}, {'W', "Whatever"} };
			std::unordered_multimap<char, std::string> foo_unordered_multimap = { {'H',"Hello"}, {'H', "Hey"}, {'W', "World"}, {'W', "Whatever"} };
			std::tuple<int, float, char, std::string> foo_tuple = { 3, 3.0f, 'g', "hello world" };
			const std::stack<int32_t> foo_stack;
			std::queue<float> foo_queue;
			std::priority_queue<wchar_t> foo_priority_queue;
            const std::bitset<4> foo_bitset{ "0011" };

            struct test : JSerializable{};
            struct test2 : test{};
            struct test3{};

			expect(is_std_array<std::decay_t<decltype(foo_array)>>());
			expect(!is_std_array<std::decay_t<decltype(foo_carray)>>());
			expect(!is_std_array<std::decay_t<decltype(foo_list)>>());
			expect(!is_std_array<std::decay_t<decltype(foo_vector)>>());

            SPECIALIZATION_CHECK(std::list, foo_list, foo_set, foo_carray, foo_array, foo_vector);
            SPECIALIZATION_CHECK(std::vector, foo_vector, foo_set, foo_carray, foo_array, foo_list);
            SPECIALIZATION_CHECK(std::forward_list, foo_forward_list, foo_vector, foo_carray, foo_array, foo_list);
            SPECIALIZATION_CHECK(std::set, foo_set, foo_vector, foo_carray, foo_array, foo_list);
            SPECIALIZATION_CHECK(std::multiset, foo_multi_set, foo_vector, foo_carray, foo_array, foo_list);
            SPECIALIZATION_CHECK(std::valarray, foo_valarry, foo_vector, foo_carray, foo_array, foo_list);
            SPECIALIZATION_CHECK(std::deque, foo_deque, foo_vector, foo_carray, foo_array, foo_list);
            SPECIALIZATION_CHECK(std::unordered_set, foo_unordered_set, foo_vector, foo_carray, foo_array, foo_list);
            SPECIALIZATION_CHECK(std::unordered_multiset, foo_unordered_multiset, foo_vector, foo_carray, foo_array, foo_list);
            SPECIALIZATION_CHECK(std::map, foo_map, foo_vector, foo_carray, foo_array, foo_list);
            SPECIALIZATION_CHECK(std::unordered_map, foo_unordered_map, foo_vector, foo_carray, foo_array, foo_list);
            SPECIALIZATION_CHECK(std::multimap, foo_multimap, foo_vector, foo_carray, foo_array, foo_list);
            SPECIALIZATION_CHECK(std::unordered_multimap, foo_unordered_multimap, foo_vector, foo_carray, foo_array, foo_list);
            SPECIALIZATION_CHECK(std::tuple, foo_tuple, foo_vector, foo_carray, foo_array, foo_list);
            SPECIALIZATION_CHECK(std::stack, foo_stack, foo_vector, foo_carray, foo_array, foo_list);
            SPECIALIZATION_CHECK(std::queue, foo_queue, foo_vector, foo_carray, foo_array, foo_list);
            SPECIALIZATION_CHECK(std::priority_queue, foo_priority_queue, foo_vector, foo_carray, foo_array, foo_list);

			expect(is_bitset<std::decay_t<decltype(foo_bitset)>>());
			expect(!is_bitset<std::decay_t<decltype(foo_carray)>>());
			expect(!is_bitset<std::decay_t<decltype(foo_list)>>());
			expect(!is_bitset<std::decay_t<decltype(foo_vector)>>());

            expect(std::is_polymorphic_v<test>);
            expect(std::is_polymorphic_v<test2>);
            expect(!std::is_polymorphic_v<test3>);
            expect(!std::is_polymorphic_v<decltype(foo_array)>);
            expect(!std::is_polymorphic_v<decltype(foo_list)>);
            expect(!std::is_polymorphic_v<decltype(foo_vector)>);

        };
    };
};