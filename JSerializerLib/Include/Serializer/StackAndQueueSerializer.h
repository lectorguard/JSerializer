#pragma once
#include "nlohmann/json.hpp"
#include "Utils/Utils.h"
#include "functional"
#include <optional>
#include <stack>
#include <queue>
#include <algorithm>

struct StackAndQueueSerializer
{
	template<typename Type>
	inline static constexpr bool IsCorrectType()
	{
		return is_specialization<Type, std::stack>() || is_specialization<Type, std::queue>();
	}

	template<typename M, typename T>
	std::optional<nlohmann::json> Serialize(T& obj, PushErrorType pushError) const
	{
		if constexpr (IsCorrectType<T>())
		{
			using V = typename T::value_type;

			nlohmann::json json_collection = nlohmann::json::array();
			auto casted_container = obj._Get_container();
			std::transform(casted_container.begin(), casted_container.end(), std::back_inserter(json_collection), [&pushError](V elem)
				{
					return DefaultSerialize<M>(elem, pushError);
				});
			return json_collection;
		}
		return std::nullopt;
	}

	template<typename M, typename T>
	std::optional<T> Deserialize(const nlohmann::json& j, PushErrorType pushError) const
	{
		using CurrentType = std::remove_reference<T>::type;

		if constexpr (IsCorrectType<T>())
		{
			using V = typename T::value_type;

			T temp;
			std::for_each(j.begin(), j.end(), [&temp, &pushError](const nlohmann::json& json_elem)
				{
					temp.push(DefaultDeserialize<M,V>(json_elem, pushError));
				});
			return temp;
		}
		return std::nullopt;
	}
};
