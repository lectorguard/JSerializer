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
#include "Serializer/PriorityQueueSerializer.h"
#include "Serializer/PointerSerializer.h"
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


template<typename M, typename T>
inline static constexpr std::optional<nlohmann::json> SerializeByJSER(T&& obj, PushErrorType pushError)
{
	static_assert(!std::is_const_v<T>, "Function ptr are used which are not following cost correctness, so no const value can be passed here");

	for (const auto& elem : CreateJSERArray<M>())
	{
		std::optional<nlohmann::json> j;
		std::visit([&j, &obj, &pushError](auto x)
			{
				j = x.Serialize<M>(obj, pushError);
			},elem);
		if (j) return j;
	}
	return std::nullopt;
	
}


template<typename M, typename T>
inline static constexpr std::optional<T> DeserializeByJSER(const nlohmann::json& j, PushErrorType pushError)
{
	for (const auto& elem : CreateJSERArray<M>())
	{
		std::optional<T> obj;
		std::visit([&obj, &j, &pushError](const auto& x) 
			{
				obj = x.Deserialize<M,T>(j, pushError); 
			}, elem);
		if (obj) return obj;
	}
	return std::nullopt;
}


template<typename M, typename T>
inline static constexpr bool IsHandledByJSER()
{
	bool serializable = false;
	for (auto elem : CreateJSERArray<M>())
	{
		std::visit([&serializable](auto x) {serializable = serializable || x.IsCorrectType<T>(); }, elem);
	}
	return serializable;
}

// Elem can not be const ref, because T could be another JSerializer. In that case we could only call const functions, but AddItem() can not be const. (Called during deserialization)

template<typename M, typename T>
inline static constexpr nlohmann::json DefaultSerialize(T&& elem, PushErrorType pushError)
{
	using CurrentType = std::remove_reference<decltype(elem)>::type;

	if constexpr (IsHandledByJSER<M,CurrentType>())
	{
		if (std::optional<nlohmann::json> jser_json = SerializeByJSER<M>(elem, pushError))
		{
			return *jser_json;
		}
		pushError({ JSerErrorTypes::DEFAULT_SERIALIZATION_ERROR, "JSerializable supported type " + std::string(typeid(CurrentType).name()) + " could not be serialized" });
		return nlohmann::json();
	}
	else
	{
		// If there is an error here, the type you passed to the serializer is probably not supported, please add a custom serializer for this type
		return nlohmann::json(elem);
	}
}


template<typename M, typename T>
inline static constexpr T DefaultDeserialize(const nlohmann::json& j, PushErrorType pushError)
{
	using CurrentType = std::remove_reference<T>::type;

	static_assert(std::is_default_constructible_v<T>, "Every type passed to JSerializer must have default constructor");

	if constexpr (IsHandledByJSER<M, CurrentType>())
	{
		if (std::optional<T> obj = DeserializeByJSER<M,T>(j, pushError))
		{
			return std::move(*obj);
		}
		pushError({ JSerErrorTypes::DEFAULT_SERIALIZATION_ERROR, "JSerializable supported type " + std::string(typeid(CurrentType).name()) + " could not be deserialized" });
		return T();
	}
	else
	{
		return j.get<T>();
	}
}

