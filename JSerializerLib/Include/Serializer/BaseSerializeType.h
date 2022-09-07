#include "nlohmann/json.hpp"

#include <string>
#include <functional>

// struct BaseSerializerType
// {
// 
// 	std::function<bool(nlohmann::json&, std::string, template<typename T> T&&, std::function<void(JSerError)>&)> Serialize = nullptr;
// 
// 	//template<typename T>
// 	//void Serialize(nlohmann::json& j, const std::string& name, T&& obj, std::function<void(JSerError)>& pushError) = 0;
// 	//
// 	//template<typename T>
// 	//void Deserialize(const nlohmann::json& j, const std::string& name, T&& obj, std::function<void(JSerError)>& pushError) = 0;
// };


// struct Example : BaseSerializerType
// {
//	template<typename T>
//	void Serialize(nlohmann::json& j, const std::string& name, T&& obj, std::function<void(JSerError)>& pushError) override
//	{
//
//	};
//};

struct BaseSerializer {};

struct StackSerializer : BaseSerializer
{
	template<typename T>
	void Serialize(nlohmann::json& j, const std::string& name, T&& obj, std::function<void()>& pushError) const 
	{
		std::cout << "Serializing stack" << std::endl;
	};

	template<typename T>
	void Deserialize(const nlohmann::json& j, const std::string& name, T&& obj) const 
	{
		std::cout << "Deserializing stack" << std::endl;
	}

};

struct QueueSerializer : BaseSerializer
{
	template<typename T>
	void Serialize(nlohmann::json& j, const std::string& name, T&& obj, std::function<void()>& pushError) const 
	{
		std::cout << "Serializing queue" << std::endl;
	};

	template<typename T>
	void Deserialize(const nlohmann::json& j, const std::string& name, T&& obj) const 
	{
		std::cout << "Serializing queue" << std::endl;
	};

};

struct PriorityQueueSerializer : BaseSerializer
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

struct MultimapSerializer : BaseSerializer
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

struct UnorderedMultimapSerializer : BaseSerializer
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

struct BitsetSerializer : BaseSerializer
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


struct PolymorphicSerializer : BaseSerializer
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


struct PointerSerializer : BaseSerializer
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