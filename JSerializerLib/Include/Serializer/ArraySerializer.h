#pragma once
#include "nlohmann/json.hpp"
#include "Utils/Utils.h"
#include "functional"
#include <optional>
#include <array>
#include <algorithm>

namespace jser
{
	struct ArraySerializer
	{
		template<typename Type>
		inline static constexpr bool IsCorrectType()
		{
			return is_std_array<Type>();
		}
	
		template<typename M, typename T>
		std::optional<nlohmann::json> Serialize(T& obj, PushErrorType pushError) const
		{
			if constexpr (IsCorrectType<T>())
			{
				using V = typename T::value_type;
	
				nlohmann::json json_collection = nlohmann::json::array();
				for (size_t i = 0; i < std::size(obj); ++i)
				{
					nlohmann::json serialized = DefaultSerialize<M>(obj[i], pushError);
					json_collection.push_back(serialized);
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
				if (std::size(temp) != j.size())
				{
					pushError({ 
						JSerErrorTypes::MEMBER_ERROR, 
						"Size of fixed array with type " + std::string(typeid(temp).name()) + " has a different length than json : " + std::to_string(std::size(temp)) + " != " + std::to_string(j.size())
						});
				}
				for (size_t i = 0; i < j.size(); ++i)
				{
					V deserialized = DefaultDeserialize<M,V>(j.at(i), pushError);
					temp[i] = deserialized;
				}
				return temp;
			}
			return std::nullopt;
		}
	};
}

