#pragma once

#include "nlohmann/json.hpp"
#include "Utils/Utils.h"
#include "Serializer/ListVectorDequeSerializer.h"
#include "Serializer/PolymorphicSerializer.h"
#include "Serializer/SetSerializer.h"
#include "Serializer/ArraySerializer.h"
#include "Serializer/ValarraySerializer.h"
#include "Serializer/ForwardListSerializer.h"
#include "Serializer/MapSerializer.h"
#include "Serializer/TupleSerializer.h"
#include "Serializer/BitsetSerializer.h"
#include "Serializer/StackAndQueueSerializer.h"
#include <string>
#include <functional>
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
#include "variant"



using SerializerType = std::variant<
	ListVectorDequeSerializer, 
	PolymorphicSerializer,
	SetSerializer,
	ArraySerializer,
	ValarraySerializer,
	ForwardListSerializer,
	MapSerializer,
	TupleSerializer,
	BitsetSerializer,
	StackAndQueueSerializer>;

inline static constexpr const std::array<SerializerType, 10> SerializationBehavior =
{
	ListVectorDequeSerializer(),
	PolymorphicSerializer(),
	SetSerializer(),
	ArraySerializer(),
	ValarraySerializer(),
	ForwardListSerializer(),
	MapSerializer(),
	TupleSerializer(),
	BitsetSerializer(),
	StackAndQueueSerializer(),
};

template<typename T>
constexpr std::optional<nlohmann::json> SerializeByJSER(T&& obj, std::function<void(JSerError)>& pushError)
{
	static_assert(!std::is_const_v<T>, "Function ptr are used which are not following cost correctness, so no const value can be passed here");

	for (const auto& elem : SerializationBehavior)
	{
		std::optional<nlohmann::json> j;
		std::visit([&j, &obj, &pushError](auto x)
			{
				j = x.Serialize(obj, pushError);
			},elem);
		if (j) return j;
	}
	return std::nullopt;
	
}

template<typename T>
constexpr std::optional<T> DeserializeByJSER(const nlohmann::json& j, std::function<void(JSerError)>& pushError)
{
	for (const auto& elem : SerializationBehavior)
	{
		std::optional<T> obj;
		std::visit([&obj, &j, &pushError](const auto& x) 
			{
				obj = x.Deserialize<T>(j, pushError); 
			}, elem);
		if (obj) return obj;
	}
	return std::nullopt;
}

template<typename T>
inline constexpr bool IsHandledByJSER()
{
	bool serializable = false;
	for (auto elem : SerializationBehavior)
	{
		std::visit([&serializable](auto x) {serializable = serializable || x.IsCorrectType<T>(); }, elem);
	}
	return serializable;
}

template<typename T>
static nlohmann::json DefaultSerialize(T&& elem, std::function<void(JSerError)>& pushError)
{
	using CurrentType = std::remove_reference<decltype(elem)>::type;

	if constexpr (IsHandledByJSER<CurrentType>())
	{
		if (std::optional<nlohmann::json> jser_json = SerializeByJSER(elem, pushError))
		{
			return *jser_json;
		}
		pushError({ JSerErrorTypes::DEFAULT_SERIALIZATION_ERROR, "JSerializable supported type " + std::string(typeid(CurrentType).name()) + " could not be serialized" });
		return nlohmann::json();
	}
	else
	{
		nlohmann::json j;
		j = elem;
		return j;
	}
}

template<typename T>
static T DefaultDeserialize(const nlohmann::json& j, std::function<void(JSerError)>& pushError)
{
	using CurrentType = std::remove_reference<T>::type;

	if constexpr (IsHandledByJSER<CurrentType>())
	{
		if (std::optional<T> obj = DeserializeByJSER<T>(j, pushError))
		{
			return *obj;
		}
		pushError({ JSerErrorTypes::DEFAULT_SERIALIZATION_ERROR, "JSerializable supported type " + std::string(typeid(CurrentType).name()) + " could not be deserialized" });
		return T();
	}
	else
	{
		return j.get<T>();
	}
}

