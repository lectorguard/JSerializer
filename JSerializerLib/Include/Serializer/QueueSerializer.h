#pragma once
#include "nlohmann/json.hpp"
#include "Utils/Utils.h"
#include "functional"

struct QueueSerializer
{
	template<typename T>
	void Serialize(nlohmann::json& j, const std::string& name, T&& obj, std::function<void(JSerError)>& pushError) const
	{
		std::cout << "Serializing queue" << std::endl;
	};

	template<typename T>
	void Deserialize(const nlohmann::json& j, const std::string& name, T&& obj, std::function<void(JSerError)>& pushError) const
	{
		std::cout << "Serializing queue" << std::endl;
	};

};

