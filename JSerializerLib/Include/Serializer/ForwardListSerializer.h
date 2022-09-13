#pragma once
#include "nlohmann/json.hpp"
#include "Utils/Utils.h"
#include "functional"
#include <optional>
#include <list>
#include <algorithm>

template<typename T> static nlohmann::json DefaultSerialize(T&& elem, PushErrorType pushError);
template<typename T> static T DefaultDeserialize(const nlohmann::json& j, PushErrorType pushError);

struct ForwardListSerializer
{
	template<typename Type>
	inline static constexpr bool IsCorrectType()
	{
		return is_specialization<Type, std::forward_list>();
	}

	template<typename T>
	std::optional<nlohmann::json> Serialize(T& obj, PushErrorType pushError) const
	{
		if constexpr (IsCorrectType<T>())
		{
			using V = typename T::value_type;

			nlohmann::json json_collection = nlohmann::json::array();
			std::transform(obj.begin(), obj.end(), std::back_inserter(json_collection), [&pushError](V elem)
				{
					return DefaultSerialize(elem, pushError);
				});
			return json_collection;
		}
		return std::nullopt;
	}

	template<typename T>
	std::optional<T> Deserialize(const nlohmann::json& j, PushErrorType pushError) const
	{
		using CurrentType = std::remove_reference<T>::type;

		if constexpr (IsCorrectType<T>())
		{
			using V = typename T::value_type;

			T temp;
			std::transform(j.begin(), j.end(), std::front_inserter(temp), [&pushError](const nlohmann::json& json_elem)
				{
					return DefaultDeserialize<V>(json_elem, pushError);
				});
			temp.reverse();
			return temp;
		}
		return std::nullopt;
	}
};

