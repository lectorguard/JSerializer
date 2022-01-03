#include "JSerializer.h"
#include <iostream>

void JSerializable::AddValidation(std::function<void()> validationFunction)
{
	Validation.push_back(validationFunction);
}

std::optional<std::string> JSerializable::SerializeObjectString(JSerError& error)
{
	error = JSerError::NO_ERROR;
	if (std::optional<nlohmann::json> json = SerializeObjectJson(error))
	{
		return json->dump(-1,(char)32,false, nlohmann::detail::error_handler_t::ignore);
	}
	return std::nullopt;
}

std::optional<nlohmann::json> JSerializable::SerializeObjectJson(JSerError& error)
{
	error = JSerError::NO_ERROR;
	executeValidation();
	if (SerializeChunks.size() == 0)
	{
		error = JSerError::SETUP_MISSING_ERROR;
		return std::nullopt;
	}
	nlohmann::json j;
	for (SerializeItem& item : SerializeChunks)
	{
		item.SerializeCB(j, item.ParameterNames, error);
		if (error != JSerError::NO_ERROR) return std::nullopt;
	}
	return j;
}

void JSerializable::DeserializeObject(nlohmann::json j, JSerError& error)
{
	error = JSerError::NO_ERROR;
	if (SerializeChunks.size() == 0)
	{
		error = JSerError::SETUP_MISSING_ERROR;
		return;
	}
	for (SerializeItem& item : SerializeChunks)
	{
		item.DeserializeCB(j, item.ParameterNames, error);
		if (error != JSerError::NO_ERROR) return;
	}
	executeValidation();
}

void JSerializable::DeserializeObject(const char* json, JSerError& error)
{
	error = JSerError::NO_ERROR;
	nlohmann::json j = nlohmann::json::parse(json, nullptr, false);
	if (j.is_discarded())
	{
		error = JSerError::JSON_ERROR;
		return;
	}
	DeserializeObject(j, error);
}

void JSerializable::DeserializeObject(std::string json, JSerError& error)
{
	error = JSerError::NO_ERROR;
	nlohmann::json j = nlohmann::json::parse(json, nullptr, false);
	if (j.is_discarded())
	{
		error = JSerError::JSON_ERROR;
		return;
	}
	DeserializeObject(j, error);
}

constexpr void JSerializable::executeValidation() 
{
	for (std::function<void()>& func : Validation)
	{
		func();
	}
}
