#pragma once
#include "nlohmann/json.hpp"
#include "Utils/Utils.h"
#include "functional"
#include <optional>
#include <map>
#include <algorithm>

template<typename T> static nlohmann::json DefaultSerialize(const T& elem, const std::function<void(JSerError)>& pushError);
template<typename T> static T DefaultDeserialize(const nlohmann::json& j, const std::function<void(JSerError)>& pushError);

struct MapSerializer
{
	template<typename Type, typename RawType = std::decay_t<Type>>
	inline static constexpr bool IsCorrectType()
	{
		return  is_specialization<RawType, std::map>()				||
 				is_specialization<RawType, std::multimap>()			||
 				is_specialization<RawType, std::unordered_map>()	||
 				is_specialization<RawType, std::unordered_multimap>();
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
					auto key_element = std::get<0>(elem);
					auto value_element = std::get<1>(elem);
					return nlohmann::json::array({ DefaultSerialize(key_element, pushError) , DefaultSerialize(value_element, pushError) });
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
			using KeyType = CurrentType::key_type;
			using ValueType = CurrentType::mapped_type;

			T temp;
			std::transform(j.begin(), j.end(), std::inserter(temp, temp.begin()), [&pushError](const nlohmann::json& json_elem) -> std::pair<KeyType, ValueType>
				{
					if (json_elem.size() == 2)
					{
						return { DefaultDeserialize<KeyType>(json_elem.at(0), pushError), DefaultDeserialize<ValueType>(json_elem.at(1), pushError) };
					}
					else
					{
						pushError({
							JSerErrorTypes::MEMBER_ERROR, 
							"Format of map inside json must be an array of 2 element arrays. But key value pair json elem has " + std::to_string(json_elem.size()) + " elements."
							});
						return std::pair<KeyType, ValueType>();
					}
					
				});
			return temp;

		}
		return std::nullopt;
	}
};

