#include "nlohmann/json.hpp"
#include "Serializer/StackSerializer.h"
#include "Serializer/QueueSerializer.h"
#include "Utils/Utils.h"
#include <string>
#include <functional>
#include <queue>
#include <map>
#include <bitset>
#include <stack>
#include "variant"

struct SerializationManager
{
	template<typename Test, template<typename...> class Ref>
	struct is_specialization : std::false_type {};

	template<template<typename...> class Ref, typename... Args>
	struct is_specialization<Ref<Args...>, Ref> : std::true_type {};

	template<typename T>
	struct is_bitset : std::false_type {};

	template<std::size_t N>
	struct is_bitset<std::bitset<N>> : std::true_type {};

	template<typename T> 
	constexpr void SerializeByJSER(JSupTypes SerializationType, nlohmann::json& j, const std::string& name, T&& obj, std::function<void(JSerError)>& pushError)
	{
		std::visit([&j, name, &obj, &pushError](const auto& x) { x.Serialize(j, name, obj, pushError); }, SerializationBehavior[SerializationType]);
	}

	template<typename T> 
	constexpr void DeserializeByJSER(JSupTypes SerializationType, const nlohmann::json& j, const std::string& name, T&& obj, std::function<void(JSerError)>& pushError)
	{
		std::visit([&j, name, &obj, &pushError](const auto& x) { x.Deserialize(j, name, obj, pushError); }, SerializationBehavior[SerializationType]);
	}

	template<typename Type>
	inline constexpr JSupTypes GetSerializationType()
	{
		if constexpr (is_specialization<Type, std::stack>::value) return JSupTypes::STACK;
		else if constexpr (is_specialization<Type, std::queue>::value) return JSupTypes::QUEUE;
		//else if constexpr (is_specialization<CurrentType, std::priority_queue>::value) return JSupTypes::PRIORITY_QUEUE;
		//else if constexpr (is_specialization<CurrentType, std::multimap>::value) return JSupTypes::MULTIMAP;
		//else if constexpr (is_specialization<CurrentType, std::unordered_multimap>::value) return JSupTypes::UNORDERED_MULTIMAP;
		//else if constexpr (is_bitset<CurrentType>::value) return JSupTypes::BITSET;
		//else if constexpr (std::is_polymorphic_v<CurrentType>) return JSupTypes::POLYMORPHIC;
		//else if constexpr (std::is_pointer_v<CurrentType>) 
		//{
		//    static_assert(false, "No pointer supported");
		//    return JSupTypes::POINTER;
		//}

		return JSupTypes::NO_CUSTOMIZED_SERIALIZATION;
	}

	using SerializerType = std::variant<StackSerializer, QueueSerializer>;// ,
		//PriorityQueueSerializer>;
		//MultimapSerializer,
		//UnorderedMultimapSerializer,
		//BitsetSerializer,
		//BitsetSerializer,
		//PolymorphicSerializer,
		//PointerSerializer>;

	std::map<JSupTypes, SerializerType> SerializationBehavior = std::map<JSupTypes, SerializerType>
	{
		{JSupTypes::STACK, StackSerializer()},
		{JSupTypes::QUEUE, QueueSerializer()},
		//{JSupTypes::PRIORITY_QUEUE, PriorityQueueSerializer()},
		//{JSupTypes::MULTIMAP, MultimapSerializer()},
		//{JSupTypes::UNORDERED_MULTIMAP, UnorderedMultimapSerializer()},
		//{JSupTypes::BITSET, BitsetSerializer()},
		//{JSupTypes::POLYMORPHIC, PolymorphicSerializer()},
		//{JSupTypes::POINTER, PointerSerializer()},
	};
};

struct PriorityQueueSerializer  
{
	template<typename T>
	void Serialize(nlohmann::json& j, const std::string& name, T&& obj, std::function<void()>& pushError)
	{
		std::cout << "Serializing queue" << std::endl;
	};

	template<typename T>
	void Deserialize(const nlohmann::json& j, const std::string& name, T&& obj, std::function<void()>& pushError)
	{
		std::cout << "Serializing queue" << std::endl;
	};

};

struct MultimapSerializer 
{
	template<typename T>
	void Serialize(nlohmann::json& j, const std::string& name, T&& obj, std::function<void()>& pushError)
	{
		std::cout << "Serializing queue" << std::endl;
	};

	template<typename T>
	void Deserialize(const nlohmann::json& j, const std::string& name, T&& obj, std::function<void()>& pushError)
	{
		std::cout << "Serializing queue" << std::endl;
	};

};

struct UnorderedMultimapSerializer 
{
	template<typename T>
	void Serialize(nlohmann::json& j, const std::string& name, T&& obj, std::function<void()>& pushError)
	{
		std::cout << "Serializing queue" << std::endl;
	};

	template<typename T>
	void Deserialize(const nlohmann::json& j, const std::string& name, T&& obj, std::function<void()>& pushError)
	{
		std::cout << "Serializing queue" << std::endl;
	};

};

struct BitsetSerializer 
{
	template<typename T>
	void Serialize(nlohmann::json& j, const std::string& name, T&& obj, std::function<void()>& pushError)
	{
		std::cout << "Serializing queue" << std::endl;
	};

	template<typename T>
	void Deserialize(const nlohmann::json& j, const std::string& name, T&& obj, std::function<void()>& pushError)
	{
		std::cout << "Serializing queue" << std::endl;
	};

};


struct PolymorphicSerializer 
{
	template<typename T>
	void Serialize(nlohmann::json& j, const std::string& name, T&& obj, std::function<void()>& pushError)
	{
		std::cout << "Serializing queue" << std::endl;
	};

	template<typename T>
	void Deserialize(const nlohmann::json& j, const std::string& name, T&& obj, std::function<void()>& pushError)
	{
		std::cout << "Serializing queue" << std::endl;
	};

};


struct PointerSerializer 
{
	template<typename T>
	void Serialize(nlohmann::json& j, const std::string& name, T&& obj, std::function<void()>& pushError)
	{
		std::cout << "Serializing queue" << std::endl;
	};

	template<typename T>
	void Deserialize(const nlohmann::json& j, const std::string& name, T&& obj, std::function<void()>& pushError)
	{
		std::cout << "Serializing queue" << std::endl;
	};

};