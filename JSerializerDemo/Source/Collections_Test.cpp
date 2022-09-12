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

namespace Collections_Test
{
    struct Foo2 : public JSerializable
    {
        int Kartoffel = 42;
        Foo2() { JSER_ADD(Kartoffel); };
    };

    struct Foo : JSerializable
    {
        std::array<std::string, 2> foo_array = { "hello", "world" };
        std::vector<int> foo_vector = { 2, 3 };
        std::deque<unsigned int> foo_deque = { 17, 26, 45, 78 };
        std::forward_list<bool> foo_forward_list = { true, false, true, false };
        std::list<float> foo_list = { 50.15f, 4848.52f };
        std::stack<Foo2> foo_stack; // TO DO 
        std::queue<float> foo_queue; // TO DO
        std::priority_queue<wchar_t> foo_priority_queue; // TO DO
        std::set<bool> foo_set = { true, false , false };
        std::multiset<int8_t> foo_multiset = { 7,7,7,7,7,7 };
		std::unordered_set<int64_t> foo_unordered_set = { 15,4589,516,4684,6818,568,18 };
        std::unordered_multiset<int64_t> foo_unordered_multiset = { 15,15,15,15,15,15,15,15 };
		std::map<std::string, char> foo_map = { {"Hello", 'H'}, {"World", 'W'} };
		std::unordered_map<uint16_t, int16_t> foo_unordered_map = { {6,6}, {7,7}, {8,8} };
		std::multimap<char, std::string> foo_multimap = { {'H',"Hello"}, {'H', "Hey"}, {'W', "World"}, {'W', "Whatever"} }; // TO DO
		std::unordered_multimap<char, std::string> foo_unordered_multimap = { {'H',"Hello"}, {'H', "Hey"}, {'W', "World"}, {'W', "Whatever"} }; // TO DO 
        std::tuple<int, float, char, std::string> foo_tuple = { 3, 3.0f, 'g', "hello world" };
        std::bitset<4> foo_bitset{"0011"};
        std::valarray<double> foo_valarray = { 15.1818,4564.4898,45.89 };

        Foo()
        {
            foo_stack.push(Foo2());
            foo_stack.push(Foo2());
            foo_stack.push(Foo2());
            foo_queue.push(10.3f);
            foo_queue.push(12.0f);
            foo_queue.push(45351.565130f);
            foo_priority_queue.push(24);
            foo_priority_queue.push('L');
            foo_priority_queue.push('W');

            JSER_ADD(foo_array, foo_vector, foo_list, foo_set, foo_multiset, foo_valarray, foo_deque, foo_forward_list, foo_unordered_set, foo_unordered_multiset);
            JSER_ADD(foo_map, foo_unordered_map, foo_multimap, foo_unordered_multimap, foo_tuple, foo_bitset);
            JSER_ADD(foo_stack, foo_queue, foo_priority_queue);

        };

        void compare(const Foo& Rhs)
        {
            using namespace boost::ut;

            expect(foo_array == Rhs.foo_array);
            expect(foo_vector == Rhs.foo_vector);
            expect(foo_forward_list == Rhs.foo_forward_list);
            expect(foo_list == Rhs.foo_list);
            expect(foo_deque == Rhs.foo_deque);
            //expect(foo_stack == Rhs.foo_stack);
            expect(foo_queue == Rhs.foo_queue);
            
            //while (!Rhs.foo_queue.empty())
            //{
            //    std::cout << foo_queue.back() << std::endl;
            //    foo_queue.pop();
            //}
            
            std::priority_queue<wchar_t> priority_queue_copy = Rhs.foo_priority_queue;
            while (!foo_priority_queue.empty())
            {
                expect(foo_priority_queue.top() == priority_queue_copy.top());
                foo_priority_queue.pop();
                priority_queue_copy.pop();
            }
			expect(foo_set == Rhs.foo_set);
			expect(foo_multiset == Rhs.foo_multiset);
			expect(foo_map == Rhs.foo_map);
			expect(foo_unordered_map == Rhs.foo_unordered_map);
			expect(foo_multimap == Rhs.foo_multimap);
			expect(foo_unordered_set == Rhs.foo_unordered_set);
			expect(foo_unordered_multiset == Rhs.foo_unordered_multiset);
			expect(foo_unordered_multimap == Rhs.foo_unordered_multimap);
			expect(foo_tuple == Rhs.foo_tuple);
			expect(foo_bitset == Rhs.foo_bitset);
			expect(std::equal(std::begin(foo_valarray), std::end(foo_valarray), std::begin(Rhs.foo_valarray)));
            
        };
    };

    boost::ut::suite Collections_Test = [] {
        using namespace boost::ut;

        "collections"_test = [] {
            Foo foo;
            //foo.foo_array[1] = "schmuse";
            foo.foo_list.push_back(5861.04f);
            foo.foo_vector.pop_back();
            std::list<JSerError> errorList;
            std::string result = foo.SerializeObjectString(std::back_inserter(errorList));
            expect(errorList.size() == 0) << "Serialization of many object associations throws error";

            std::cout << result << std::endl;

            Foo deserialized;
            deserialized.DeserializeObject(result, std::back_inserter(errorList));
            expect(errorList.size() == 0) << "Deserialization of many object associations throws error";
            foo.compare(deserialized);
        };

        //
        //
        //"custom serialization from string"_test = [] {
        //    Foo foo;
        //    std::list<JSerError> errorList;
        //    std::string result = foo.SerializeObjectString(std::back_inserter(errorList));
        //    expect(errorList.size() == 0) << "Serialization of many object associations throws error";
        //    const char* expectedResult = R"({"foo0":15,"foo1":56656565,"foo2":498,"foo3":48})";
        //    expect(!result.compare(expectedResult));
        //
        //    Foo deserialized;
        //    deserialized.DeserializeObject(expectedResult, std::back_inserter(errorList));
        //    expect(errorList.size() == 0) << "Deserialization of many object associations throws error";
        //    foo.compare(deserialized);
        //};

        "simple iterable types "_test = []()
        {
            struct CollectionTest : JSerializable
            {
                CollectionTest() 
                { 
                    JSER_ADD(foo, foo_list, foo_vector ,foo_forward_list ,foo_set, foo_multi_set);
                    JSER_ADD(foo_valarry, foo_deque, foo_forward_list, foo_unordered_set, foo_unordered_multiset);
                    JSER_ADD(foo_map, foo_unordered_map, foo_multimap, foo_unordered_multimap, foo_tuple, foo_bitset);
                    JSER_ADD(foo_stack, foo_queue, foo_priority_queue);
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

				std::stack<int32_t> foo_stack;  
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
			t.foo_stack.push(16);
			t.foo_stack.push(23);
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

            //std::cout << result << std::endl;

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
			std::array<std::string, 2> foo_array = { "hello", "world" };
			float my_arr[] = { 2.8f,5.4f,6.0f };
            std::list<int> foo_list = { 45,48,513,8,61,86,156 };
            std::vector<double> foo_vector = { 16561.4681,168.168,416811.68186,4648.864 };
            std::forward_list<bool> foo_forward_list = { false,true, true, false };
            std::set<int32_t> foo_set = { 5,4,5658,6 };
            std::multiset<int32_t> foo_multi_set = { 185,165,65,16,81,65,65,65 };
            std::valarray<int32_t> foo_valarry = { 11565,156,651,186,68658,616,1650 };
            float carray[3] = { 153.0f, 18965.0f, 15455.f };

            struct test : JSerializable{};
            struct test2 : test{};
            struct test3{};

            expect(is_std_array<decltype(foo_array)>());
            expect(!is_std_array<decltype(my_arr)>());
            expect(!is_std_array<decltype(foo_list)>());
            expect(!is_std_array<decltype(foo_vector)>());

            expect(is_specialization<decltype(foo_list), std::list>());
            expect(!is_specialization<decltype(my_arr), std::list>());
            expect(!is_specialization<decltype(foo_array), std::list>());
            expect(!is_specialization<decltype(foo_vector), std::list>());

			expect(is_specialization<decltype(foo_vector), std::vector>());
			expect(!is_specialization<decltype(my_arr), std::vector>());
			expect(!is_specialization<decltype(foo_array), std::vector>());
			expect(!is_specialization<decltype(foo_list), std::vector>());

            expect(is_specialization<decltype(foo_forward_list), std::forward_list>());
			expect(!is_specialization<decltype(foo_vector), std::forward_list>());
			expect(!is_specialization<decltype(my_arr), std::forward_list>());
			expect(!is_specialization<decltype(foo_array), std::forward_list>());
			expect(!is_specialization<decltype(foo_list), std::forward_list>());

			expect(is_specialization<decltype(foo_set), std::set>());
			expect(!is_specialization<decltype(foo_vector), std::set>());
			expect(!is_specialization<decltype(my_arr), std::set>());
			expect(!is_specialization<decltype(foo_array), std::set>());
			expect(!is_specialization<decltype(foo_list), std::set>());

			expect(is_specialization<decltype(foo_multi_set), std::multiset>());
			expect(!is_specialization<decltype(foo_vector), std::multiset>());
			expect(!is_specialization<decltype(my_arr), std::multiset>());
			expect(!is_specialization<decltype(foo_array), std::multiset>());
			expect(!is_specialization<decltype(foo_list), std::multiset>());
            
			expect(is_specialization<decltype(foo_valarry), std::valarray>());
			expect(!is_specialization<decltype(foo_vector), std::valarray>());
			expect(!is_specialization<decltype(my_arr), std::valarray>());
			expect(!is_specialization<decltype(foo_array), std::valarray>());
			expect(!is_specialization<decltype(foo_list), std::valarray>());

            expect(std::is_polymorphic_v<test>);
            expect(std::is_polymorphic_v<test2>);
            expect(!std::is_polymorphic_v<test3>);
            expect(!std::is_polymorphic_v<decltype(foo_array)>);
            expect(!std::is_polymorphic_v<decltype(foo_list)>);
            expect(!std::is_polymorphic_v<decltype(foo_vector)>);

        };
    };
};