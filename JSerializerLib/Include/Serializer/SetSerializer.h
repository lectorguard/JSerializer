#pragma once
#include "nlohmann/json.hpp"
#include "Utils/Utils.h"
#include "functional"
#include <optional>
#include <set>
#include <algorithm>

template<typename T> static nlohmann::json DefaultSerialize(const T& elem, const std::function<void(JSerError)>& pushError);
template<typename T> static T DefaultDeserialize(const nlohmann::json& j, const std::function<void(JSerError)>& pushError);

struct SetSerializer
{
	template<typename Type, typename RawType = std::decay_t<Type>>
	inline static constexpr bool IsCorrectType()
	{
		return	is_specialization<RawType, std::set>()					|| 
				is_specialization<RawType, std::multiset>()			|| 
				is_specialization<RawType, std::unordered_set>()		|| 
				is_specialization<RawType, std::unordered_multiset>();
	}

	template<typename T>
	std::optional<nlohmann::json> Serialize(const T& obj, const std::function<void(JSerError)>& pushError) const
	{
		if constexpr (IsCorrectType<T>())
		{
			using V = typename T::value_type;

			nlohmann::json json_collection = nlohmann::json::array();
			std::transform(obj.begin(), obj.end(), std::inserter(json_collection, json_collection.begin()), [&pushError](const V& elem)
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
			std::transform(j.begin(), j.end(), std::inserter(temp, temp.begin()), [&pushError](const nlohmann::json& json_elem)
				{
					return DefaultDeserialize<V>(json_elem, pushError);
				});
			return temp;
		}
		return std::nullopt;
	}
};
