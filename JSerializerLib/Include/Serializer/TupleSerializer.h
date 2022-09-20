#pragma once
#include "nlohmann/json.hpp"
#include "Utils/Utils.h"
#include "functional"
#include <optional>
#include <map>
#include <algorithm>

namespace jser
{
	struct TupleSerializer
	{
		template<typename Type>
		inline static constexpr bool IsCorrectType()
		{
			return is_specialization<Type, std::tuple>();
		}
	
		template<typename M, typename T>
		std::optional<nlohmann::json> Serialize(T& obj, PushErrorType pushError) const
		{
			if constexpr (IsCorrectType<T>())
			{
				nlohmann::json json_collection = nlohmann::json::array();
				std::apply([&json_collection, &pushError](auto&&... args) 
					{
					((json_collection.push_back(DefaultSerialize<M>(args, pushError))), ...);
					}, obj);
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
				T temp;
				if (std::tuple_size<T>() != j.size())
				{
					pushError({ JSerErrorTypes::MEMBER_ERROR, "Tuple Size " + std::to_string(std::tuple_size<T>()) + "is not equal to number of elements in json " + std::to_string(j.size()) });
					return temp;
				}
				
				uint32_t index = 0;
				std::apply([&index, &j, &pushError](auto&&... args)
					{
						((args = DefaultDeserialize<M, std::decay_t<decltype(args)>>(j.at(index++), pushError)), ...);
					}, temp);
				return temp;
			}
			return std::nullopt;
		}
	};
}

