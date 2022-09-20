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

template<JSerErrorCompatible T>
inline std::string JSerializable::SerializeObjectString(std::back_insert_iterator<T> error)
{
	nlohmann::json j = SerializeObjectJson(error);
	return j.dump(-1, (char)32, false, nlohmann::detail::error_handler_t::ignore);
}

template<JSerErrorCompatible T>
inline nlohmann::json JSerializable::SerializeObjectJson(std::back_insert_iterator<T> error)
{
	std::function<void(JSerError)> pushError = [&error](JSerError newError) { error = newError; };
	return SerializeObject_Internal(pushError);
}

template<JSerErrorCompatible T>
inline void JSerializable::DeserializeObject(nlohmann::json j, std::back_insert_iterator<T> error)
{
	std::function<void(JSerError)> pushError = [&error](JSerError newError) { error = newError; };
	DeserializeObject_Internal(j, pushError);
}

template<JSerErrorCompatible T>
inline void JSerializable::DeserializeObject(const char* json, std::back_insert_iterator<T> error)
{
	DeserializeObject(std::string(json), error);
}

template<JSerErrorCompatible T>
inline void JSerializable::DeserializeObject(std::string json, std::back_insert_iterator<T> error)
{
	nlohmann::json j = nlohmann::json::parse(json, nullptr, false);
	if (j.is_discarded())
	{
		error = { JSerErrorTypes::JSON_ERROR, "The following json " + json + " could not be parsed and got discarded" };
		return;
	}
	DeserializeObject(j, error);
}

template<typename M, typename...O>
inline constexpr DefaultSerializeItem JSerializable::CreateSerializeItem( const std::vector<std::string>& names, O&& ... objects)
{
	assert(names.size() == sizeof...(objects) && " for each name there must be a parameter");

	return
       {
		names,
		[tup = std::forward_as_tuple(objects...)](nlohmann::json& j, const std::vector<std::string>& parameterNames,  PushErrorType pushError)
		{
			std::apply([&parameterNames,&j, &pushError](auto &&... args)
				{
					Serialize<0,M>(j, parameterNames, pushError, args...);
				}, tup);
		},
		[tup = std::forward_as_tuple(objects...), this](nlohmann::json j, const std::vector<std::string>& parameterNames,  PushErrorType pushError) mutable
		{
			std::apply([&parameterNames, &j, &pushError](auto &&... args)
				{
					Deserialize<0,M>(j, parameterNames, pushError, args...);
				}, tup);
		}
	};
};

inline nlohmann::json JSerializable::SerializeObject_Internal(PushErrorType pushError)
{
	if (bSerializationStarted)
	{
		pushError({ JSerErrorTypes::POINTER_ERROR, "Circular Dependencies detected aborting, current serialized type " + std::string(typeid(*this).name()) });
		return nlohmann::json();
	}

	Validate(JSerEvent::BEFORE_SERIALIZATION, pushError);
	std::vector<DefaultSerializeItem> DefaultSerializeChunks = AddItem().GetItems();
	if (DefaultSerializeChunks.size() == 0)
	{
		pushError({ JSerErrorTypes::SETUP_MISSING_ERROR, "You need to call JSER_ADD_ITEMS(...) or similar inside the constructor of " + std::string(typeid(*this).name()) + ", before calling SerializeObject. " });
	}
	
	nlohmann::json j;
	bSerializationStarted = true;
	for (DefaultSerializeItem& item : DefaultSerializeChunks)
	{
		item.SerializeCB(j, item.ParameterNames, pushError);
	}
	bSerializationStarted = false;
	return j;
}

inline void JSerializable::DeserializeObject_Internal(nlohmann::json j, PushErrorType pushError)
{
	std::vector<DefaultSerializeItem> DefaultSerializeChunks = AddItem().GetItems();
	if (DefaultSerializeChunks.size() == 0)
	{
		pushError({ JSerErrorTypes::SETUP_MISSING_ERROR, "You need to call JSER_ADD_ITEMS(...) or similar inside the constructor of " + std::string(typeid(*this).name()) + ", before calling DeserializeObject." });
	}
	for (DefaultSerializeItem& item : DefaultSerializeChunks)
	{
		item.DeserializeCB(j, item.ParameterNames, pushError);
	}
	Validate(JSerEvent::AFTER_DESERIALIZATION, pushError);
}

template<size_t index, typename M, typename...O>
static void JSerializable::Serialize(nlohmann::json& j, const std::vector<std::string>& names, PushErrorType pushError,  O&& ... objects)
{
	auto& elem = get<index>(objects...);

	using CurrentType = std::remove_reference<decltype(elem)>::type;
	// Implemented a prototype for carrays, but carrays are not simply lvalue assignable. Needs a lot of copies and special treatment inside generic serialization logic.
	static_assert(!std::is_array_v<CurrentType>, "Deserialization of carray pointer is not supported, please use insted std::array");

	j[names[index]] = DefaultSerialize<M>(elem, pushError);

	if constexpr (index + 1 < sizeof...(objects)) 
	{
		Serialize<index + 1, M>(j, names, pushError, std::forward<O>(objects)...);
	}
}

template<size_t index, typename M, typename...O>
static void JSerializable::Deserialize(const nlohmann::json& j, const std::vector<std::string>& names, PushErrorType pushError,  O&& ... objects)
{
	auto& elem = get<index>(objects...);

	using CurrentType = std::remove_reference<decltype(elem)>::type;
	// Implemented a prototype for carrays, but carrays are not simply lvalue assignable. Needs a lot of copies and special treatment inside generic serialization logic.
	static_assert(!std::is_array_v<CurrentType>, "Deserialization of carray pointer is not supported, please use insted std::array");

	if (!j.contains(names[index]))
	{
		pushError({ JSerErrorTypes::MEMBER_ERROR, "Json file is missing member --> " + names[index] + " <-- of type " + std::string(typeid(elem).name()) + ". Field can not be deserialized." });
	}
	else
	{
		elem = DefaultDeserialize<M, CurrentType>(j[names[index]], pushError);
	}

	if constexpr (index + 1 < sizeof...(objects))
	{
		Deserialize<index + 1, M>(j, names, pushError, std::forward<O>(objects)...);
	}
}
