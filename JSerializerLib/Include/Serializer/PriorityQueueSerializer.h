#pragma once
#include "nlohmann/json.hpp"
#include "Utils/Utils.h"
#include "functional"
#include <optional>
#include <queue>
#include <algorithm>

namespace jser
{
	struct PriorityQueueSerializer
	{
		template<typename Type>
		inline static constexpr bool IsCorrectType()
		{
			return is_specialization<Type, std::priority_queue>();
		}
	
		template<typename M, typename T>
		std::optional<nlohmann::json> Serialize(T& obj, PushErrorType pushError) const
		{
			if constexpr (IsCorrectType<T>())
			{
				nlohmann::json json_collection = nlohmann::json::array();
				T copy = obj;
				while (!copy.empty())
				{
					json_collection.push_back(DefaultSerialize<M>(copy.top(), pushError));
					copy.pop();
				}
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
}

