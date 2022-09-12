#pragma once
#pragma once
#include "nlohmann/json.hpp"
#include "Utils/Utils.h"
#include "functional"
#include <optional>
#include <map>
#include <algorithm>

template<typename T> static nlohmann::json DefaultSerialize(const T& elem, const std::function<void(JSerError)>& pushError);
template<typename T> static T DefaultDeserialize(const nlohmann::json& j, const std::function<void(JSerError)>& pushError);

struct TupleSerializer
{
	template<typename Type, typename RawType = std::decay_t<Type>>
	inline static constexpr bool IsCorrectType()
	{
		return is_specialization<RawType, std::tuple>();
	}

	template<typename T>
	std::optional<nlohmann::json> Serialize(const T& obj, const std::function<void(JSerError)>& pushError) const
	{
		if constexpr (IsCorrectType<T>())
		{
			nlohmann::json json_collection = nlohmann::json::array();
			std::apply([&json_collection, &pushError](auto&&... args) 
				{
				((json_collection.push_back(DefaultSerialize(args, pushError))), ...); 
				}, obj);
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
			T temp;
			if (std::tuple_size<T>() != j.size())
			{
				pushError({ JSerErrorTypes::MEMBER_ERROR, "Tuple Size " + std::to_string(std::tuple_size<T>()) + "is not equal to number of elements in json " + std::to_string(j.size()) });
				return temp;
			}
			
			uint32_t index = 0;
			std::apply([&index, &j, &pushError](auto&&... args)
				{
					((args = DefaultDeserialize<std::remove_reference<decltype(args)>::type>(j.at(index++), pushError)), ...);
				}, temp);
			return temp;
		}
		return std::nullopt;
	}
};

