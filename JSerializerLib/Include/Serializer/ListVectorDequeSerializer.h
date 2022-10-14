#pragma once
#include "nlohmann/json.hpp"
#include "Utils/Utils.h"
#include "functional"
#include <optional>
#include <vector>
#include <algorithm>

namespace jser
{
	struct ListVectorDequeSerializer
	{
		template<typename Type>
		inline static constexpr bool IsCorrectType() 
		{
			return is_specialization<Type, std::vector>() || is_specialization<Type, std::list>() || is_specialization<Type, std::deque>();
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
						return DefaultSerialize<M>(elem, pushError);
					});
				return json_collection;
			}
			return std::nullopt;
		}
	
		template<typename M, typename T>
		std::optional<T> Deserialize(const nlohmann::json& j, PushErrorType pushError) const
		{
			if constexpr (IsCorrectType<T>())
			{
				using V = typename T::value_type;
	
				T temp;
				std::transform(j.begin(), j.end(), std::back_inserter(temp), [&pushError](const nlohmann::json& json_elem)
					{
						return DefaultDeserialize<M,V>(json_elem, pushError);
					});
				return temp;
			}
			return std::nullopt;
		}
	};
}
