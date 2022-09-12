#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <assert.h>
#include <map>
#include <functional>
#include <optional>
#include "nlohmann/json.hpp"
#include "Utils/Macros.h"
#include "Utils/Utils.h"
#include "Serializer/SerializationManager.h"


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

    // After Deserialization is finished, validation function is automatically called
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
            [tup = std::forward_as_tuple(objects...),this](nlohmann::json j, std::vector<std::string> parameterNames, std::function<void(JSerError)>& pushError) mutable
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
		// Implemented a prototype for carrays, but carrays are not simply lvalue assignable. Needs a lot of copies and special treatment inside generic serialization logic.
		static_assert(!std::is_array_v<CurrentType>, "Deserialization of carray pointer is not supported, please use insted std::array");

        j[names[index]] = DefaultSerialize(elem, pushError);

        if constexpr (index + 1 < sizeof...(objects)) {
            Serialize<index + 1>(j, names, pushError, std::forward<O>(objects)...);
        }
    }

    template<size_t index = 0, typename...O>
    static void Deserialize(const nlohmann::json& j, const std::vector<std::string> names, std::function<void(JSerError)>& pushError, O&& ... objects)
    {
		auto& elem = get<index>(objects...);

		using CurrentType = std::remove_reference<decltype(elem)>::type;
        static_assert(!std::is_pointer_v<CurrentType>, "Deserialization does not support pointer types");
        // Implemented a prototype for carrays, but carrays are not simply lvalue assignable. Needs a lot of copies and special treatment inside generic serialization logic.
        static_assert(!std::is_array_v<CurrentType>, "Deserialization of carray pointer is not supported, please use insted std::array");

        if (!j.contains(names[index]))
        {
            pushError({ JSerErrorTypes::MEMBER_ERROR, "Json file is missing member --> " + names[index] + " <-- of type " + std::string(typeid(elem).name()) + ". Field can not be deserialized."});
        }
        else
        {
            elem = DefaultDeserialize<CurrentType>(j[names[index]], pushError);
        }

        if constexpr (index + 1 < sizeof...(objects))
        {
            Deserialize<index + 1>(j, names, pushError, std::forward<O>(objects)...);
        }
    }

    std::vector<CustomSerializeItem> CustomSerializeChunks = {};
    std::vector<DefaultSerializeItem> DefaultSerializeChunks = {};
    std::vector<std::function<void()>> Validation = {};

    friend struct PolymorphicSerializer;
};