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
    struct Foo : JSerializable
    {
        std::array<std::string, 2> foo_array = { "hello", "world" };
        std::vector<unsigned char> foo_vector = { 'h', 'w' };
        std::deque<unsigned int> foo_deque = { 17, 26, 45, 78 };
        std::forward_list<bool> foo_forward_list = { true, false, true, false };
        std::list<float> foo_list = { 50.15f, 4848.52f };
        //std::stack<double> foo_stack; // TO DO 
        //std::queue<float> foo_queue; // TO DO
        //std::priority_queue<wchar_t> foo_priority_queue; // TO DO
        std::set<bool> foo_set = { true, false , false };
        std::multiset<int8_t> foo_multiset = { 7,7,7,7,7,7 };
        std::map<std::string, char> foo_map = { {"Hello", 'H'}, {"World", 'W'} };
        std::unordered_map<uint16_t, int16_t> foo_unordered_map = { {6,6}, {7,7}, {8,8} };
        //std::multimap<char, std::string> foo_multimap = { {'H',"Hello"}, {'H', "Hey"}, {'W', "World"}, {'W', "Whatever"} }; // TO DO
        std::unordered_set<int64_t> foo_unordered_set = { 15,4589,516,4684,6818,568,18 };
        std::unordered_multiset<int64_t> foo_unordered_multiset = { 15,15,15,15,15,15,15,15 };
        //std::unordered_multimap<char, std::string> foo_unordered_multimap = { {'H',"Hello"}, {'H', "Hey"}, {'W', "World"}, {'W', "Whatever"} }; // TO DO 
        std::tuple<int, float, char, std::string> foo_tuple = { 3, 3.0f, 'g', "hello world" };
        //std::bitset<4> foo_bitset{"0011"};
        std::valarray<double> foo_valarray = { 15.1818,4564.4898,45.89 };

        Foo()
        {
            //foo_stack.push(10.3);
            //foo_stack.push(12.0);
            //foo_stack.push(45351.565130);
            //foo_queue.push(10.3f);
            //foo_queue.push(12.0f);
            //foo_queue.push(45351.565130f);
            //foo_priority_queue.push(24);
            //foo_priority_queue.push('L');
            //foo_priority_queue.push('W');

            JSER_ADD(foo_array, foo_vector, foo_deque, foo_forward_list, foo_list, foo_set);//, /*foo_stack,*/ foo_queue, foo_priority_queue, foo_set);
            JSER_ADD(foo_multiset, foo_map, foo_unordered_map, foo_unordered_set, foo_unordered_multiset, foo_tuple, foo_valarray);// , foo_multimap);// , foo_unordered_set, foo_unordered_multiset, foo_unordered_multimap, foo_tuple);
        };

        void compare(const Foo& Rhs)
        {
            using namespace boost::ut;
            expect(foo_array == Rhs.foo_array);
            expect(foo_vector == Rhs.foo_vector);
            expect(foo_forward_list == Rhs.foo_forward_list);
            expect(foo_list == Rhs.foo_list);
            //expect(foo_stack == Rhs.foo_stack);
            //expect(foo_queue == Rhs.foo_queue);
            
            //std::priority_queue<wchar_t> priority_queue_copy = Rhs.foo_priority_queue;
            //while (!foo_priority_queue.empty())
            //{
            //    expect(foo_priority_queue.top() == priority_queue_copy.top());
            //    foo_priority_queue.pop();
            //    priority_queue_copy.pop();
            //}
            expect(foo_set == Rhs.foo_set);
            expect(foo_multiset == Rhs.foo_multiset);
            expect(foo_map == Rhs.foo_map);
            expect(foo_unordered_map == Rhs.foo_unordered_map);
            //expect(foo_multimap == Rhs.foo_multimap);
            expect(foo_unordered_set == Rhs.foo_unordered_set);
            expect(foo_unordered_multiset == Rhs.foo_unordered_multiset);
            //expect(foo_unordered_multimap == Rhs.foo_unordered_multimap);
            expect(foo_tuple == Rhs.foo_tuple);
            //expect(foo_bitset == Rhs.foo_bitset);
            expect(std::equal(std::begin(foo_valarray), std::end(foo_valarray), std::begin(Rhs.foo_valarray)));
            
        };
    };

    boost::ut::suite Collections_Test = [] {
        using namespace boost::ut;

        "collections"_test = [] {
            Foo foo;
            std::list<JSerError> errorList;
            std::string result = foo.SerializeObjectString(std::back_inserter(errorList));
            expect(errorList.size() == 0) << "Serialization of many object associations throws error";

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
    };
};