#include "JSerializer.h"

void Serializable::AddValidation(std::function<void()> validationFunction)
{
	Validation.push_back(validationFunction);
}

std::string Serializable::SerializeObject()
{
	executeValidation();
	assert(SerializeCB && "You must call SetUpSerialization before you can call SerializeObject");
	return SerializeCB();
}

void Serializable::DeserializeObject(nlohmann::json j)
{
	assert(DeserializeCB && "You must call SetUpSerialization before you can call DeserializeObject");
	DeserializeCB(j);
	executeValidation();
}

void Serializable::DeserializeObject(const char* json)
{
	DeserializeObject(nlohmann::json::parse(json));
}

void Serializable::DeserializeObject(std::string json)
{
	DeserializeObject(nlohmann::json::parse(json));
}

constexpr void Serializable::executeValidation()
{
	for (std::function<void()>& func : Validation)
	{
		func();
	}
}
