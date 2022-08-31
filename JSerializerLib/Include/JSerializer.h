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
#define JSER_ADD(...) AddDefaultSerializeItem(OPEN_BRACKET FOR_EACH(STRWITHCOMMA,__VA_ARGS__) CLOSE_BRACKET FOR_EACH(COMMACONTENT,__VA_ARGS__))
#define JSER_ADD_CUSTOM(SerializeCB,DeserializeCB)AddCustomSerializeItem(SerializeCB, DeserializeCB)
#define JSER_ADD_VAL(x) AddValidation([this]()x)


enum class JSerErrorTypes
{
    NO_ERROR,               // OK
    JSON_ERROR,             // Error inside the json library
    SETUP_MISSING_ERROR,    // You must call SetUpSerialization before you can call DeserializeObject
    MEMBER_ERROR,           // Failed deserializing member variable
    POLYMORPHIC_ERROR,      // Object is polymorphic but it does not inherit from JSerializable

};


struct JSerError
{
    const JSerErrorTypes Error = JSerErrorTypes::NO_ERROR;
    const std::string Message = "";
};

template<class T>
concept JSerErrorCompatible = std::is_same_v<typename T::value_type, JSerError>;


struct CustomSerializeItem
{
    std::function<void(nlohmann::json&, std::function<void(JSerError)>&)> SerializeCB = nullptr;
    std::function<void(nlohmann::json&, std::function<void(JSerError)>&)> DeserializeCB = nullptr;
};

struct DefaultSerializeItem
{
    std::vector<std::string> ParameterNames = {};
    std::function<void(nlohmann::json&, std::vector<std::string>&, std::function<void(JSerError)>&)> SerializeCB = nullptr;
    std::function<void(nlohmann::json&, std::vector<std::string>&, std::function<void(JSerError)>&)> DeserializeCB = nullptr;
};

struct JSerializable {

    using CustomCB = std::function<void(nlohmann::json&, std::function<void(JSerError)>&)>;

    virtual ~JSerializable() = default;

    template<JSerErrorCompatible T>
    std::string SerializeObjectString(std::back_insert_iterator<T> error)
    {
        nlohmann::json j = SerializeObjectJson(error);
        return j.dump(-1, (char)32, false, nlohmann::detail::error_handler_t::ignore);
    }

    template<JSerErrorCompatible T>
    nlohmann::json SerializeObjectJson(std::back_insert_iterator<T> error)
    {
        std::function<void(JSerError)> pushError = [&error](JSerError newError) { error = newError; };
        return SerializeObject_Internal(pushError);
    }

    template<JSerErrorCompatible T>
    void DeserializeObject(nlohmann::json j, std::back_insert_iterator<T> error)
    {
        std::function<void(JSerError)> pushError = [&error](JSerError newError) { error = newError; };
        DeserializeObject_Internal(j, pushError);
    }
    
    template<JSerErrorCompatible T>
    void DeserializeObject(const char* json, std::back_insert_iterator<T> error)
    {
        DeserializeObject(std::string(json), error);
    }

    template<JSerErrorCompatible T>
    void DeserializeObject(std::string json, std::back_insert_iterator<T> error)
    {
        nlohmann::json j = nlohmann::json::parse(json, nullptr, false);
        if (j.is_discarded())
        {
            error = { JSerErrorTypes::JSON_ERROR, "The following json " + json + " could not be parsed and got discarded" };
            return;
        }
        DeserializeObject(j, error);
    }

    void AddValidation(std::function<void()> validationFunction)
    {
        Validation.push_back(validationFunction);
    }

    template<typename...O>
    constexpr void AddDefaultSerializeItem(const std::vector<std::string> names, O&& ... objects)
    {
        assert(names.size() == sizeof...(objects) && " for each name there must be a parameter");

        DefaultSerializeChunks.push_back({
            names,
            [tup = std::forward_as_tuple(objects...)](nlohmann::json& j, std::vector<std::string> parameterNames, std::function<void(JSerError)>& pushError)
            {
                std::apply([&parameterNames,&j, &pushError](auto &&... args)
                    {
                        Serialize(j, parameterNames, pushError, args...);
                    }, tup);
            },
            [tup = std::forward_as_tuple(objects...)](nlohmann::json j, std::vector<std::string> parameterNames, std::function<void(JSerError)>& pushError) mutable
            {
                std::apply([&parameterNames, &j, &pushError](auto &&... args)
                    {
                        Deserialize(j, parameterNames, pushError, args...);
                    }, tup);
            }
            });
    };

    void AddCustomSerializeItem(const CustomCB& SerializeCB, const CustomCB& DeserializeCB)
    {
        assert(SerializeCB && DeserializeCB && "Valid function pointers must be provided (de)serialization");

        CustomSerializeChunks.push_back({ SerializeCB, DeserializeCB });
    }

private:
    constexpr void executeValidation()
    {
        for (std::function<void()>& func : Validation)
        {
            func();
        }
    }

    nlohmann::json SerializeObject_Internal(std::function<void(JSerError)>& pushError)
    {
        executeValidation();
        if (CustomSerializeChunks.size() == 0 && DefaultSerializeChunks.size() == 0)
        {
            pushError({ JSerErrorTypes::SETUP_MISSING_ERROR, "You need to call JSER_ADD_ITEMS(...) or similar inside the constructor of " + std::string(typeid(*this).name())+ ", before calling SerializeObject. "});
        }
        nlohmann::json j;
        for (DefaultSerializeItem& item : DefaultSerializeChunks)
        {
            item.SerializeCB(j,item.ParameterNames, pushError);
        }
        for (CustomSerializeItem& item : CustomSerializeChunks)
        {
            item.SerializeCB(j, pushError);
        }
        return j;
    }

    void DeserializeObject_Internal(nlohmann::json j, std::function<void(JSerError)>& pushError)
    {
        if (CustomSerializeChunks.size() == 0 && DefaultSerializeChunks.size() == 0)
        {
            pushError({ JSerErrorTypes::SETUP_MISSING_ERROR, "You need to call JSER_ADD_ITEMS(...) or similar inside the constructor of " + std::string(typeid(*this).name()) + ", before calling DeserializeObject."});
        }
        for (DefaultSerializeItem& item : DefaultSerializeChunks)
        {
            item.DeserializeCB(j, item.ParameterNames, pushError);
        }
        for (CustomSerializeItem& item : CustomSerializeChunks)
        {
            item.DeserializeCB(j, pushError);
        }
        executeValidation();
    }

    template <int I, class... Ts>
    constexpr static decltype(auto) get(Ts&&... ts) {
        return std::get<I>(std::forward_as_tuple(ts...));
    }

    template<size_t index = 0, typename...O>
    static void Serialize(nlohmann::json& j, const std::vector<std::string> names, std::function<void(JSerError)>& pushError, O&& ... objects)
    {
        auto& elem = get<index>(objects...);

        using CurrentType = std::remove_reference<decltype(elem)>::type;
        static_assert(!std::is_pointer_v<CurrentType>, "Serialization does not support pointer types");
        
        if constexpr (std::is_polymorphic_v<CurrentType>) 
        {
            if (JSerializable* serializable = dynamic_cast<JSerializable*>(&elem)) // not constexpr
            {
                if (std::optional<nlohmann::json> subj = serializable->SerializeObject_Internal(pushError))
                {
                    j[names[index]] = *subj;
                }
                else return;
            }
            // does not compile with "j[names[index]] = elem;", if statement above must be a constexpr but it isnt :/
            else pushError({ JSerErrorTypes::POLYMORPHIC_ERROR, " " + std::string(typeid(CurrentType).name()) + " must inherit from JSerializable in order to be Serializable. "});
        }
        else
        {
            j[names[index]] = elem;
        }
        

        if constexpr (index + 1 < sizeof...(objects)) {
            Serialize<index + 1>(j, names, pushError, std::forward<O>(objects)...);
        }
    }

    template<size_t index = 0, typename...O>
    static void Deserialize(nlohmann::json j, const std::vector<std::string> names, std::function<void(JSerError)>& pushError, O&& ... objects)
    {
        auto& elem = get<index>(objects...);

        using CurrentType = std::remove_reference<decltype(elem)>::type;

        if (!j.contains(names[index]))
        {
            pushError({ JSerErrorTypes::MEMBER_ERROR, "Json file is missing member --> " + names[index] + " <-- of type " + std::string(typeid(elem).name()) + ". Field can not be deserialized."});
        }
        else
        {
            if constexpr (std::is_polymorphic_v<CurrentType>)
            {
                if (JSerializable* serializable = dynamic_cast<JSerializable*>(&elem)) // not constexpr
                {
                    serializable->DeserializeObject_Internal(j[names[index]], pushError);
                }
                // does not compile with "j[names[index]] = elem;", if statement above must be a constexpr but it isnt :/
                else pushError({ JSerErrorTypes::POLYMORPHIC_ERROR, " " + std::string(typeid(CurrentType).name()) + " must inherit from JSerializable in order to be Serializable. " });
            }
            else
            {
                elem = j[names[index]].get<CurrentType>();
            }
        }

        if constexpr (index + 1 < sizeof...(objects))
        {
            Deserialize<index + 1>(j, names, pushError, std::forward<O>(objects)...);
        }
    }

    std::vector<CustomSerializeItem> CustomSerializeChunks = {};
    std::vector<DefaultSerializeItem> DefaultSerializeChunks = {};
    std::vector<std::function<void()>> Validation = {};
};