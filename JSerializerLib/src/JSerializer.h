#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <assert.h>
#include <map>
#include <functional>
#include <optional>
#include "nlohmann/json.hpp"

#define EXPAND( x ) x
#define F(x, ...) X = x and VA_ARGS = __VA_ARGS__
#define G(...) EXPAND( F(__VA_ARGS__) )


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
#define JSER_ADD_ITEMS(...) AddSerializeItems(OPEN_BRACKET FOR_EACH(STRWITHCOMMA,__VA_ARGS__) CLOSE_BRACKET FOR_EACH(COMMACONTENT,__VA_ARGS__))
#define JSER_ADD_VAL(x) AddValidation([this]()x)


enum class JSerError
{
    NO_ERROR,               // OK
    JSON_ERROR,             // Error inside the json library
    SETUP_MISSING_ERROR,    // You must call SetUpSerialization before you can call DeserializeObject
    MEMBER_ERROR,           // Failed deserializing member variable
    POLYMORPHIC_ERROR,      // Object is polymorphic but it does not inherit from JSerializable

};

struct SerializeItem
{
    std::vector<std::string> ParameterNames = {};
    std::function<void(nlohmann::json&, std::vector<std::string>&, JSerError&)> SerializeCB = nullptr;
    std::function<void(nlohmann::json&, std::vector<std::string>&, JSerError&)> DeserializeCB = nullptr;
};

struct JSerializable {

    virtual ~JSerializable() {};

    std::optional<std::string> SerializeObjectString(JSerError& error);
    std::optional<nlohmann::json> SerializeObjectJson(JSerError& error);
    void DeserializeObject(nlohmann::json j, JSerError& error);
    void DeserializeObject(const char* json, JSerError& error);
    void DeserializeObject(std::string json,JSerError& error);
    void AddValidation(std::function<void()> validationFunction);

    template<typename...O>
    constexpr void AddSerializeItems(const std::vector<std::string> names, O&& ... objects)
    {
        assert(names.size() == sizeof...(objects) && "for each name there must be a parameter");

        SerializeChunks.push_back({
            names,
            [tup = std::forward_as_tuple(objects...)](nlohmann::json& j, std::vector<std::string> parameterNames, JSerError& error)
            {
                std::apply([&parameterNames,&j, &error](auto &&... args)
                    {
                        Serialize(j, parameterNames, error, args...);
                    }, tup);
            },
            [tup = std::forward_as_tuple(objects...)](nlohmann::json j, std::vector<std::string> parameterNames, JSerError& error) mutable
            {
                std::apply([&parameterNames, &j, &error](auto &&... args)
                    {
                        Deserialize(j, parameterNames, error, args...);
                    }, tup);
            }
            });
    };

private:
    template <int I, class... Ts>
    constexpr static decltype(auto) get(Ts&&... ts) {
        return std::get<I>(std::forward_as_tuple(ts...));
    }

    template<size_t index = 0, typename...O>
    static void Serialize(nlohmann::json& j, const std::vector<std::string> names, JSerError& error, O&& ... objects)
    {
        auto& elem = get<index>(objects...);

        using CurrentType = std::remove_reference<decltype(elem)>::type;
        static_assert(!std::is_pointer<CurrentType>::value, "Serialization does not support pointer types");
        
        if constexpr (std::is_polymorphic_v<CurrentType>) 
        {
            if (JSerializable* serializable = dynamic_cast<JSerializable*>(&elem)) // not constexpr
            {
                if (std::optional<nlohmann::json> subj = serializable->SerializeObjectJson(error))
                {
                    j[names[index]] = *subj;
                }
                else return;
            }
            // does not compile with "j[names[index]] = elem;", if statement above must be a constexpr but it isnt :/
            else
            {
                error = JSerError::POLYMORPHIC_ERROR;
                return;
            }
        }
        else
        {
            j[names[index]] = elem;
        }
        

        if constexpr (index + 1 < sizeof...(objects)) {
            Serialize<index + 1>(j, names, error, std::forward<O>(objects)...);
        }
    }

    template<size_t index = 0, typename...O>
    static void Deserialize(nlohmann::json j, const std::vector<std::string> names, JSerError& error, O&& ... objects)
    {
        auto& elem = get<index>(objects...);

        if (!j.contains(names[index]))
        {
            error = JSerError::MEMBER_ERROR;
            return;
        }

        using CurrentType = std::remove_reference<decltype(elem)>::type;

        if constexpr (std::is_polymorphic_v<CurrentType>)
        {
            if (JSerializable* serializable = dynamic_cast<JSerializable*>(&elem)) // not constexpr
            {
                serializable->DeserializeObject(j[names[index]], error);
            }
            // does not compile with "j[names[index]] = elem;", if statement above must be a constexpr but it isnt :/
            else 
            {
                error = JSerError::POLYMORPHIC_ERROR;
                return;
            }
        }
        else
        {
            elem = j[names[index]].get<CurrentType>();
        }
        
        if constexpr (index + 1 < sizeof...(objects))
        {
            Deserialize<index + 1>(j, names, error, std::forward<O>(objects)...);
        }
    }

    constexpr void executeValidation();

    std::vector<SerializeItem> SerializeChunks = {};
    std::vector<std::function<void()>> Validation = {};
};