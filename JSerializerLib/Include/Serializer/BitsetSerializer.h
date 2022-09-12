#pragma once
#include "nlohmann/json.hpp"
#include "Utils/Utils.h"
#include "functional"
#include <optional>
#include <bitset>

template<typename T> static nlohmann::json DefaultSerialize(T&& elem, std::function<void(JSerError)>& pushError);
template<typename T> static T DefaultDeserialize(const nlohmann::json& j, std::function<void(JSerError)>& pushError);

struct BitsetSerializer
{
	template<typename Type>
	inline static constexpr bool IsCorrectType()
	{
		return is_bitset<Type>();
	}

	template<typename T>
	std::optional<nlohmann::json> Serialize(T& obj, std::function<void(JSerError)>& pushError) const
	{
		if constexpr (IsCorrectType<T>())
		{
			return nlohmann::json(obj.to_string());
		}
		return std::nullopt;
	}

	template<typename T>
	std::optional<T> Deserialize(const nlohmann::json& j, std::function<void(JSerError)>& pushError) const
	{
		using CurrentType = std::remove_reference<T>::type;

		if constexpr (IsCorrectType<T>())
		{
			return T(j.get<std::string>());
		}
		return std::nullopt;
	}
};
