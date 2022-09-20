#pragma once
#include "nlohmann/json.hpp"
#include "Utils/Utils.h"
#include "functional"
#include <optional>
#include <map>
#include <algorithm>

namespace jser
{
	struct MapSerializer
	{
		template<typename Type>
		inline static constexpr bool IsCorrectType()
		{
			return is_specialization<Type, std::map>()				||
	 				is_specialization<Type, std::multimap>()		||
	 				is_specialization<Type, std::unordered_map>()	||
	 				is_specialization<Type, std::unordered_multimap>();
		}
	
		template<typename M, typename T>
		std::optional<nlohmann::json> Serialize(T& obj, PushErrorType pushError) const
		{
			if constexpr (IsCorrectType<T>())
			{
				using V = typename T::value_type;
	
				nlohmann::json json_collection = nlohmann::json::array();
				std::transform(obj.begin(), obj.end(), std::back_inserter(json_collection), [&pushError](V elem)
					{
						auto json_array = nlohmann::json::array();
						auto key_element = std::get<0>(elem);
						auto value_element = std::get<1>(elem);
						json_array.push_back(DefaultSerialize<M>(key_element, pushError));
						json_array.push_back(DefaultSerialize<M>(value_element, pushError));
						return json_array;
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
				using KeyType = CurrentType::key_type;
				using ValueType = CurrentType::mapped_type;
	
				T temp;
				std::transform(j.begin(), j.end(), std::inserter(temp, temp.begin()), [&pushError](const nlohmann::json& json_elem) -> std::pair<KeyType, ValueType>
					{
						if (json_elem.size() == 2)
						{
							return 
							{ 
								DefaultDeserialize<M,KeyType>(json_elem.at(0), pushError), 
								DefaultDeserialize<M,ValueType>(json_elem.at(1), pushError) 
							};
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
}

