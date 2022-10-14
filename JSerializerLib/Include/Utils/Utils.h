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
#include <variant>

	
namespace jser
{
	enum class JSerErrorTypes
	{
		NO_ERROR,						// OK
		JSON_ERROR,						// Error inside the json library
		SETUP_MISSING_ERROR,			// You must call JSER_ADD_... before you can call DeserializeObject
		MEMBER_ERROR,					// Failed deserializing member variable
		POLYMORPHIC_ERROR,				// Object is polymorphic but it does not inherit from JSerializable
		DEFAULT_SERIALIZATION_ERROR,	// Error was returned during default serialization process
		POINTER_ERROR,					// Weak pointers and circular dependencies are not supported
		VALIDATION_ERROR,				// Custom error which can be called from user during validation process
	};
	
	enum class JSerEvent
	{
		BEFORE_SERIALIZATION,
		AFTER_DESERIALIZATION
	};
	
	struct JSerError
	{
		const JSerErrorTypes Error = JSerErrorTypes::NO_ERROR;
		const std::string Message = "";
	};
	
	template<class T>
	concept JSerErrorCompatible = std::is_same_v<typename T::value_type, JSerError>;
	
	using PushErrorType = typename std::function<void(JSerError)>&;
	
	struct DefaultSerializeItem
	{
		std::vector<std::string> ParameterNames = {};
		std::function<void(nlohmann::json&, const std::vector<std::string>&, PushErrorType)> SerializeCB = nullptr;
		std::function<void(nlohmann::json&, const std::vector<std::string>&, PushErrorType)> DeserializeCB = nullptr;
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
	
	template <typename... Types>
	struct ToTuple;
	
	template <typename... Types>
	struct ToTuple<std::variant<Types...>>
	{
		using type = std::tuple<Types...>;
	};
	
	template<typename V>
	inline static constexpr auto CreateJSERArray()->std::array<V, std::variant_size_v<V>>
	{
		using ArrayType = std::array<V, std::variant_size_v<V>>;
		using TupleType = typename ToTuple<V>::type;
	
		ArrayType my_array;
		std::apply([&my_array](auto&& ... args) { my_array = { {args ...} }; }, TupleType());
		return my_array;
	}
	
	template<typename M, typename T>
#if defined(__clang__)
		inline static nlohmann::json DefaultSerialize(T&& elem, PushErrorType pushError);
#elif defined(_MSC_VER) || defined(__GNUC__) || defined(__GNUG__)
		inline static constexpr nlohmann::json DefaultSerialize(T&& elem, PushErrorType pushError);
#endif
	
	template<typename M, typename T>
	inline static constexpr T DefaultDeserialize(const nlohmann::json& j, PushErrorType pushError);
	
	template<typename M, typename T>
#if defined(__clang__)
		inline static std::optional<nlohmann::json> SerializeByJSER(T&& obj, PushErrorType pushError);
#elif defined(_MSC_VER) || defined(__GNUC__) || defined(__GNUG__)
		inline static constexpr std::optional<nlohmann::json> SerializeByJSER(T&& obj, PushErrorType pushError);
#endif
	
	template<typename M, typename T>
	inline static constexpr std::optional<T> DeserializeByJSER(const nlohmann::json& j, PushErrorType pushError);
	
	template<typename M, typename T>
	inline static constexpr bool IsHandledByJSER();
	
	struct JserChunkAppender
	{
		JserChunkAppender& Append(DefaultSerializeItem item)
		{
			SerializationItems.push_back(item);
			return *this;
		}
	
		JserChunkAppender& Append(std::vector<DefaultSerializeItem> items)
		{
			SerializationItems.insert(SerializationItems.end(), items.begin(), items.end());
			return *this;
		}
	
		std::vector<DefaultSerializeItem> GetItems() { return SerializationItems; };
	
	private:
		std::vector<DefaultSerializeItem> SerializationItems = {};
	};
	
	
	struct JSerializable {
	
		using CustomCB = std::function<void(nlohmann::json&, PushErrorType)>;
	
		virtual ~JSerializable() = default;
	
		// Can not be const because Serialize and Deserialize need to call this function, deserialization process is not a const process. 
		virtual JserChunkAppender AddItem() { return JserChunkAppender(); };
	
		// Validation function is called before serialization and after deserialization is finished
		virtual void Validate(JSerEvent jser_event, PushErrorType push_error) {};
	
		template<JSerErrorCompatible T>
		inline std::string SerializeObjectString(std::back_insert_iterator<T> error);
	
		template<JSerErrorCompatible T>
		inline nlohmann::json SerializeObjectJson(std::back_insert_iterator<T> error);
	
		template<JSerErrorCompatible T>
		inline void DeserializeObject(nlohmann::json j, std::back_insert_iterator<T> error);
	
		template<JSerErrorCompatible T>
		inline void DeserializeObject(const char* json, std::back_insert_iterator<T> error);
	
		template<JSerErrorCompatible T>
		inline void DeserializeObject(std::string json, std::back_insert_iterator<T> error);
	
		template<typename M, typename...O>
#if defined(__clang__)
		inline DefaultSerializeItem CreateSerializeItem( const std::vector<std::string>& names, O&& ... objects);
#elif defined(_MSC_VER) || defined(__GNUC__) || defined(__GNUG__)
		inline constexpr DefaultSerializeItem CreateSerializeItem( const std::vector<std::string>& names, O&& ... objects);
#endif
	
	private:
	
	
		inline nlohmann::json SerializeObject_Internal(PushErrorType pushError);
	
		inline void DeserializeObject_Internal(nlohmann::json j, PushErrorType pushError);
	
		template <int I, class... Ts>
		constexpr static decltype(auto) get(Ts&&... ts) {
			return std::get<I>(std::forward_as_tuple(ts...));
		}
	
		template<size_t index = 0, typename M, typename...O>
		static void Serialize(nlohmann::json& j, const std::vector<std::string>& names, PushErrorType pushError,  O&& ... objects);
	
		template<size_t index = 0, typename M, typename...O>
		static void Deserialize(const nlohmann::json& j, const std::vector<std::string>& names, PushErrorType pushError,  O&& ... objects);
	
		template<typename T>
		static void PrepareDeserialization(T&& elem, PushErrorType pushError);
	
	
		bool bSerializationStarted = false;
		friend struct PolymorphicSerializer;
	};
}


