#pragma once
#include <string>
#include <functional>
#include <vector>
#include "nlohmann/json.hpp"


enum class JSerErrorTypes
{
	NO_ERROR,               // OK
	JSON_ERROR,             // Error inside the json library
	SETUP_MISSING_ERROR,    // You must call JSER_ADD_... before you can call DeserializeObject
	MEMBER_ERROR,           // Failed deserializing member variable
	POLYMORPHIC_ERROR,      // Object is polymorphic but it does not inherit from JSerializable

};

enum class JSupTypes : uint8_t
{
	NO_CUSTOMIZED_SERIALIZATION,
	STACK,
	QUEUE,
	PRIORITY_QUEUE,
	MULTIMAP,
	UNORDERED_MULTIMAP,
	BITSET,
	POLYMORPHIC,
	POINTER
};


struct JSerError
{
	const JSerErrorTypes Error = JSerErrorTypes::NO_ERROR;
	const std::string Message = "";
};

template<class T>
concept JSerErrorCompatible = std::is_same_v<typename T::value_type, JSerError>;


struct CustomSerializeItem
{
	std::function<void(nlohmann::json&, std::function<void(JSerError)>&)> SerializeCB = nullptr;
	std::function<void(nlohmann::json&, std::function<void(JSerError)>&)> DeserializeCB = nullptr;
};

struct DefaultSerializeItem
{
	std::vector<std::string> ParameterNames = {};
	std::function<void(nlohmann::json&, std::vector<std::string>&, std::function<void(JSerError)>&)> SerializeCB = nullptr;
	std::function<void(nlohmann::json&, std::vector<std::string>&, std::function<void(JSerError)>&)> DeserializeCB = nullptr;
};