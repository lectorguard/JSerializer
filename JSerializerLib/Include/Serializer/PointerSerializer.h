#pragma once
#include "nlohmann/json.hpp"
#include "Utils/Utils.h"
#include <memory>
#include <type_traits>
#include <optional>

namespace jser
{
	struct PointerSerializer
	{
		template<typename Type>
		inline static constexpr bool IsCorrectType()
		{
			return is_specialization<Type, std::unique_ptr>() || is_specialization<Type, std::shared_ptr>();
		}
	
		template<typename M, typename T>
		std::optional<nlohmann::json> Serialize(T& obj, PushErrorType pushError) const
		{
			if constexpr (IsCorrectType<T>())
			{
				if (obj)
				{
					return DefaultSerialize<M>(*obj, pushError);
				}
				else
				{
					//Null
					return nlohmann::json();
				}
				
			}
			return std::nullopt;
		}
	
		template<typename M, typename T>
		std::optional<T> Deserialize(const nlohmann::json& j, PushErrorType pushError) const
		{
			if constexpr (IsCorrectType<T>())
			{
				using value_type = typename std::decay_t<T>::element_type;
	
				if constexpr (is_specialization<T, std::unique_ptr>())
				{
					if (!j.is_null())
					{
						return std::make_unique<value_type>(DefaultDeserialize<M, value_type>(j, pushError));
					}
					else
					{
						return nullptr;
					}
				}
				else if constexpr (is_specialization<T, std::shared_ptr>())
				{
					if (!j.is_null())
					{
						return std::make_shared<value_type>(DefaultDeserialize<M, value_type>(j, pushError));
					}
					else
					{
						return nullptr;
					}
				}
			}
			return std::nullopt;
		}
	
	};
}

