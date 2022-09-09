#pragma once
#include "nlohmann/json.hpp"
#include "Utils/Utils.h"
#include "functional"
#include <optional>
#include <vector>
#include <algorithm>

template<typename T> static nlohmann::json DefaultSerialize(T&& elem, std::function<void(JSerError)>& pushError);
template<typename T> static T DefaultDeserialize(const nlohmann::json& j, std::function<void(JSerError)>& pushError);

struct ListVectorSerializer
{
	template<typename Type>
	inline static constexpr bool IsCorrectType() 
	{
		return is_specialization<Type, std::vector>()		||
			//is_std_array<Type>() || // Comment this out because back inserter is not supported 
			is_specialization<Type, std::forward_list>()	|| // needs to be tested
			is_specialization<Type, std::set>()				|| // needs to be tested
			is_specialization<Type, std::multiset>()		|| // needs to be tested
			is_specialization<Type, std::valarray>()		|| // needs to be tested
			is_specialization<Type, std::list>();
		// ||
					
				//is_specialization<Type, std::set>()				||
				//is_specialization<Type, std::multiset>()		||
				//is_specialization<Type, std::valarray>();
	}

	template<typename T>
	std::optional<nlohmann::json> Serialize(T& obj, std::function<void(JSerError)>& pushError) const
	{
		if constexpr (IsCorrectType<T>())
		{
			using V = typename T::value_type;

			nlohmann::json json_collection = nlohmann::json::array();
			std::transform(obj.begin(), obj.end(), std::back_inserter(json_collection), [&pushError](const V& elem) 
				{ 
					return DefaultSerialize(elem, pushError); 
				});
			//for (const V& elem : obj)
			//{
			//	// Default serializable
			//	nlohmann::json serialized = DefaultSerialize(elem, pushError);
			//	json_collection.push_back(serialized);
			//}
			return json_collection;
		}
		return std::nullopt;
	}

	template<typename T>
	std::optional<T> Deserialize(const nlohmann::json& j, std::function<void(JSerError)>& pushError) const
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

			//for (const nlohmann::json& elem : j)
			//{
			//	// Default deserialize
			//	V deserializedObject = DefaultDeserialize<V>(elem, pushError);
			//	temp.push_back(deserializedObject);
			//}
			return temp;
		}
		return std::nullopt;
	}
};
