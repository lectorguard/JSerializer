#pragma once
#include <string>
#include <functional>
#include <vector>
#include "nlohmann/json.hpp"
#include <string>
#include <functional>
#include <array>
#include <vector>
#include <deque>
#include <forward_list>
#include <list>
#include <stack>
#include <queue>
#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <bitset>
#include <valarray>
#include <tuple>
#include "variant"

	
enum class JSerErrorTypes
{
	NO_ERROR,						// OK
	JSON_ERROR,						// Error inside the json library
	SETUP_MISSING_ERROR,			// You must call JSER_ADD_... before you can call DeserializeObject
	MEMBER_ERROR,					// Failed deserializing member variable
	POLYMORPHIC_ERROR,				// Object is polymorphic but it does not inherit from JSerializable
	DEFAULT_SERIALIZATION_ERROR,	// Error was returned during default serialization process
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

template<typename Test, template<typename...> class Ref>
struct is_specialization : std::false_type {};

template<template<typename...> class Ref, typename... Args>
struct is_specialization<Ref<Args...>, Ref> : std::true_type {};

template<typename T>
struct is_std_array : std::false_type {};

template<typename T, std::size_t N>
struct is_std_array<std::array<T, N>> : std::true_type {};

template<typename T>
struct is_bitset : std::false_type {};

template<std::size_t N>
struct is_bitset<std::bitset<N>> : std::true_type {};


