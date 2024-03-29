#pragma once
#include "nlohmann/json.hpp"
#include "Utils/Utils.h"
#include "functional"
#include <optional>


namespace jser
{
	struct PolymorphicSerializer
	{
		template<typename Type>
		inline static constexpr bool IsCorrectType() 
		{
			return std::is_polymorphic_v<Type>;
		}
	
		template<typename M, typename T>
		std::optional<nlohmann::json> Serialize(T& obj, PushErrorType pushError)
		{
			if constexpr (IsCorrectType<T>())
			{
				nlohmann::json j;

#if defined(__GNUC__) || defined(__GNUG__)
				// If this test fails at compile time, temp is probably not inheriting from JSerializable
				if (JSerializable* serializable = &obj)
#elif defined(_MSC_VER) || defined(__clang__)
				if (JSerializable* serializable = dynamic_cast<JSerializable*>(&obj)) // runtime check
#endif	
				{
					if (std::optional<nlohmann::json> subj = serializable->SerializeObject_Internal(pushError))
					{
						j = *subj;
					}
				}
				else pushError({ JSerErrorTypes::POLYMORPHIC_ERROR, " " + std::string(typeid(T).name()) + " must inherit from JSerializable in order to be Serializable. " });
				return j;
			}
			return std::nullopt;
			
		};
	
		template<typename M, typename T>
		std::optional<T> Deserialize(const nlohmann::json& j, PushErrorType pushError) const
		{
			if constexpr (IsCorrectType<T>())
			{
				T temp;

#if defined(__GNUC__) || defined(__GNUG__)
				// If this test fails at compile time, temp is probably not inheriting from JSerializable
				if (JSerializable* serializable = &temp)
#elif defined(_MSC_VER) || defined(__clang__)
				if (JSerializable* serializable = dynamic_cast<JSerializable*>(&temp)) // runtime check
#endif	
				{
					serializable->DeserializeObject_Internal(j, pushError);
				}
				else pushError({ JSerErrorTypes::POLYMORPHIC_ERROR, " " + std::string(typeid(T).name()) + " must inherit from JSerializable in order to be Serializable. " });
				return temp;
			}
			return std::nullopt;
		}
	};
}

