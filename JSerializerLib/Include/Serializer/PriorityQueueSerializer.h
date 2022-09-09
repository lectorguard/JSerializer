#pragma once
#include "nlohmann/json.hpp"
#include "Utils/Utils.h"
#include "functional"
#include <queue>
#include <vector>

struct PriorityQueueSerializer
{
	template<typename T, typename V = typename T::value_type>
	void Serialize(nlohmann::json& j, const std::string& name, T& obj, std::function<void(JSerError)>& pushError) const
	{
		//T prio_queue_copy = obj;
		//std::vector<V> target_container;
		//target_container.reserve(prio_queue_copy.size());
		//for()
		//T& original = obj;
		//std::deque<V> deque_casted = original._Get_container();
		//std::vector<V> serializable(deque_casted.begin(), deque_casted.end());
		//j[name] = serializable;
	};

	template<typename T, typename V = typename T::value_type>
	void Deserialize(const nlohmann::json& j, const std::string& name, T& obj, std::function<void(JSerError)>& pushError) const
	{
		//T temp;
		//const std::vector<V> deserialized = j[name];
		//std::for_each(deserialized.begin(), deserialized.end(),
		//	[&temp](auto f)
		//	{
		//		temp.push(f);
		//	}
		//);
		//obj = temp;
	}

};

