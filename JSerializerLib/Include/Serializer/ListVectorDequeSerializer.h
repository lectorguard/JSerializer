#pragma once
#include "nlohmann/json.hpp"
#include "Utils/Utils.h"
#include "functional"
#include <optional>
#include <vector>
#include <algorithm>

template<typename T> static nlohmann::json DefaultSerialize(const T& elem, const std::function<void(JSerError)>& pushError);
template<typename T> static T DefaultDeserialize(const nlohmann::json& j, const std::function<void(JSerError)>& pushError);

struct ListVectorDequeSerializer
{
	template<typename Type, typename RawType = std::decay_t<Type>>
	inline static constexpr bool IsCorrectType() 
	{
		return is_specialization<RawType, std::vector>() || is_specialization<RawType, std::list>() || is_specialization<RawType, std::deque>();
	}

	template<typename T>
	std::optional<nlohmann::json> Serialize(const T& obj, const std::function<void(JSerError)>& pushError) const
	{
		if constexpr (IsCorrectType<T>())
		{
			using V = typename T::value_type;

			nlohmann::json json_collection = nlohmann::json::array();
			std::transform(obj.begin(), obj.end(), std::back_inserter(json_collection), [&pushError](const V& elem) 
				{ 
					return DefaultSerialize(elem, pushError); 
				});
			return json_collection;
		}
		return std::nullopt;
	}

	template<typename T>
	std::optional<T> Deserialize(const nlohmann::json& j, const std::function<void(JSerError)>& pushError) const
	{
		using CurrentType = std::remove_reference<T>::type;

		if constexpr (IsCorrectType<T>())
		{
			using V = typename T::value_type;

			T temp;
			std::transform(j.begin(), j.end(), std::back_inserter(temp), [&pushError](const nlohmann::json& json_elem)
				{
					return DefaultDeserialize<V>(json_elem, pushError);
				});
			return temp;
		}
		return std::nullopt;
	}
};
