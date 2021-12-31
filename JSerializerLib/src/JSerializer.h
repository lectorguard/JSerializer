#pragma once

#include <string>
#include <vector>
#include <assert.h>
#include <map>
#include <functional>
#include "nlohmann/json.hpp"

#define EXPAND( x ) x
#define F(x, ...) X = x and VA_ARGS = __VA_ARGS__
#define G(...) EXPAND( F(__VA_ARGS__) )

//#define TEST(code, v1, v2, v3) std::invoke(code,v1); std::invoke(code,v2); std::invoke(code,v3);


// Make a FOREACH macro
#define FE_0(WHAT)
#define FE_1(WHAT, X) WHAT(X)
#define FE_2(WHAT, X, ...) WHAT(X)EXPAND(FE_1(WHAT, __VA_ARGS__))
#define FE_3(WHAT, X, ...) WHAT(X)EXPAND(FE_2(WHAT, __VA_ARGS__))
#define FE_4(WHAT, X, ...) WHAT(X)EXPAND(FE_3(WHAT, __VA_ARGS__))
#define FE_5(WHAT, X, ...) WHAT(X)EXPAND(FE_4(WHAT, __VA_ARGS__))
#define FE_6(WHAT, X, ...) WHAT(X)EXPAND(FE_5(WHAT, __VA_ARGS__))
#define FE_7(WHAT, X, ...) WHAT(X)EXPAND(FE_6(WHAT, __VA_ARGS__))
#define FE_8(WHAT, X, ...) WHAT(X)EXPAND(FE_7(WHAT, __VA_ARGS__))
#define FE_9(WHAT, X, ...) WHAT(X)EXPAND(FE_8(WHAT, __VA_ARGS__))
#define FE_10(WHAT, X, ...) WHAT(X)EXPAND(FE_9(WHAT, __VA_ARGS__))
#define FE_11(WHAT, X, ...) WHAT(X)EXPAND(FE_10(WHAT, __VA_ARGS__))
#define FE_12(WHAT, X, ...) WHAT(X)EXPAND(FE_11(WHAT, __VA_ARGS__))
#define FE_13(WHAT, X, ...) WHAT(X)EXPAND(FE_12(WHAT, __VA_ARGS__))
#define FE_14(WHAT, X, ...) WHAT(X)EXPAND(FE_13(WHAT, __VA_ARGS__))
#define FE_15(WHAT, X, ...) WHAT(X)EXPAND(FE_14(WHAT, __VA_ARGS__))
#define FE_16(WHAT, X, ...) WHAT(X)EXPAND(FE_15(WHAT, __VA_ARGS__))
#define FE_17(WHAT, X, ...) WHAT(X)EXPAND(FE_16(WHAT, __VA_ARGS__))
//... repeat as needed

#define GET_MACRO(_0,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,NAME,...) NAME 
#define FOR_EACH(action,...) \
    EXPAND(GET_MACRO(_0,__VA_ARGS__,FE_17,FE_16,FE_15,FE_14,FE_13,FE_12,FE_11,FE_10,FE_9,FE_8,FE_7,FE_6,FE_5,FE_4,FE_3,FE_2,FE_1,FE_0,)(action,__VA_ARGS__))

// Actions
#define COMMA ,
#define STRWITHCOMMA(X) #X COMMA
#define COMMACONTENT(X) COMMA X
#define OPEN_BRACKET  {
#define CLOSE_BRACKET }
// Helper function
#define SETUP(...) SetUpSerialization(OPEN_BRACKET FOR_EACH(STRWITHCOMMA,__VA_ARGS__) CLOSE_BRACKET FOR_EACH(COMMACONTENT,__VA_ARGS__))



struct Serializable {

    std::string SerializeObject();
    void DeserializeObject(nlohmann::json j);
    void DeserializeObject(const char* json);
    void DeserializeObject(std::string json);
    void AddValidation(std::function<void()> validationFunction);

    template<typename...O>
    constexpr void SetUpSerialization(const std::vector<std::string> names, O&& ... objects)
    {
        assert(names.size() == sizeof...(objects) && "for each name there must be a parameter");

        ParameterNames = names;

        SerializeCB = [this, tup = std::forward_as_tuple(objects...)]()
        {
            return std::apply([this](auto &&... args)
                {
                    nlohmann::json j;
                    serialize(j, ParameterNames, args...);
                    return j.dump();
                }, tup);
        };

        DeserializeCB = [this, tup = std::forward_as_tuple(objects...)](nlohmann::json j) mutable
        {
            std::apply([this, j](auto &&... args)
                {
                    deserialize(j, ParameterNames, args...);
                }, tup);
        };
    };

private:
    template <int I, class... Ts>
    decltype(auto) get(Ts&&... ts) {
        return std::get<I>(std::forward_as_tuple(ts...));
    }

    template<size_t index = 0, typename...O>
    constexpr void serialize(nlohmann::json& j, const std::vector<std::string> names, O&& ... objects)
    {
        using CurrentType = std::remove_reference<decltype(get<index>(objects...))>::type;
        static_assert(!std::is_pointer<CurrentType>::value, "Serialization does not support pointer types");

        j[names[index]] = get<index>(objects...);

        if constexpr (index + 1 < sizeof...(objects)) {
            serialize<index + 1>(j, names, std::forward<O>(objects)...);
        }
    }

    template<size_t index = 0, typename...O>
    constexpr void deserialize(nlohmann::json j, const std::vector<std::string> names, O&& ... objects)
    {
        assert(j.contains(names[index]) && "parsed json contains non existing member variable");

        using CurrentType = std::remove_reference<decltype(get<index>(objects...))>::type;
        get<index>(objects...) = j[names[index]].get<CurrentType>();

        if constexpr (index + 1 < sizeof...(objects))
        {
            deserialize<index + 1>(j, names, std::forward<O>(objects)...);
        }
    }

    constexpr void executeValidation();

    std::function<std::string()> SerializeCB = nullptr;
    std::function<void(nlohmann::json)> DeserializeCB = nullptr;
    std::vector<std::string> ParameterNames = {};
    std::vector<std::function<void()>> Validation = {};
};