#pragma once
#include "nlohmann/json.hpp"
#include "Utils/Utils.h"
#include "functional"
#include <optional>
#include <bitset>

namespace jser
{
	struct BitsetSerializer
	{
		template<typename Type>
		inline static constexpr bool IsCorrectType()
		{
			return is_bitset<Type>();
		}
	
		template<typename M, typename T>
		std::optional<nlohmann::json> Serialize(T& obj, PushErrorType pushError) const
		{
			if constexpr (IsCorrectType<T>())
			{
				return nlohmann::json(obj.to_string());
			}
			return std::nullopt;
		}
	
		template<typename M, typename T>
		std::optional<T> Deserialize(const nlohmann::json& j, PushErrorType pushError) const
		{
			if constexpr (IsCorrectType<T>())
			{
				return T(j.get<std::string>());
			}
			return std::nullopt;
		}
	};
}
