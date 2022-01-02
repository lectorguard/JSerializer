#include "JSerializer.h"
#include <iostream>

void JSerializable::AddValidation(std::function<void()> validationFunction)
{
	Validation.push_back(validationFunction);
}

std::string JSerializable::SerializeObjectString() 
{
	return SerializeObjectJson().dump();
}

nlohmann::json JSerializable::SerializeObjectJson()
{
	executeValidation();
	assert(SerializeChunks.size() > 0 && "You must call SetUpSerialization before you can call SerializeObject");
	nlohmann::json j;
	for (SerializeItem& item : SerializeChunks)
	{
		item.SerializeCB(j, item.ParameterNames);
	}
	return j;
}

void JSerializable::DeserializeObject(nlohmann::json j)
{
	assert(SerializeChunks.size() > 0 && "You must call SetUpSerialization before you can call DeserializeObject");
	for (SerializeItem& item : SerializeChunks)
	{
		item.DeserializeCB(j, item.ParameterNames);
	}
	executeValidation();
}

void JSerializable::DeserializeObject(const char* json)
{
	DeserializeObject(nlohmann::json::parse(json));
}

void JSerializable::DeserializeObject(std::string json)
{
	DeserializeObject(nlohmann::json::parse(json));
}

constexpr void JSerializable::executeValidation() 
{
	for (std::function<void()>& func : Validation)
	{
		func();
	}
}
