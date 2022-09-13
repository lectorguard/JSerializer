#pragma once
#include "nlohmann/json.hpp"
#include "Utils/Utils.h"
#include "functional"
#include <optional>


struct JSerializable;
nlohmann::json SerializeObject_Internal(PushErrorType pushError);
void DeserializeObject_Internal(nlohmann::json j, PushErrorType pushError);


struct PolymorphicSerializer
{
	template<typename Type>
	inline static constexpr bool IsCorrectType() 
	{
		return std::is_polymorphic_v<Type>;
	}

	template<typename T>
	std::optional<nlohmann::json> Serialize(T& obj, PushErrorType pushError)
	{
		if constexpr (IsCorrectType<T>())
		{
			nlohmann::json j;
			if (JSerializable* serializable = dynamic_cast<JSerializable*>(&obj)) // not constexpr
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

	template<typename T>
	std::optional<T> Deserialize(const nlohmann::json& j, PushErrorType pushError) const
	{
		if constexpr (IsCorrectType<T>())
		{
			T temp;
			if (JSerializable* serializable = dynamic_cast<JSerializable*>(&temp)) // not constexpr
			{
				serializable->DeserializeObject_Internal(j, pushError);
			}
			// does not compile with "j[names[index]] = elem;", if statement above must be a constexpr but it isnt :/
			else pushError({ JSerErrorTypes::POLYMORPHIC_ERROR, " " + std::string(typeid(T).name()) + " must inherit from JSerializable in order to be Serializable. " });
			return temp;
		}
		return std::nullopt;
	}
};

