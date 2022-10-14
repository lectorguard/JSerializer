#define BOOST_UT_DISABLE_MODULE // After updgrading to VS 2022 we can use modules
#include "boost/ut.hpp"
#include <cassert>
#include "JSerializer.h"

namespace CustomSerializerExample
{
	template<typename T>
	struct Vector3
	{
#if defined(__clang__) || defined(__GNUC__) || defined(__GNUG__)
		typedef T 	value_type;
#elif defined(_MSC_VER)
		using value_type = typename T;	
#endif	

		value_type X;
		value_type Y;
		value_type Z;
	};

	struct Vector3_Serializer
	{
		template<typename Type>
		inline static constexpr bool IsCorrectType()
		{
			// Ignores all the template stuff, just the name of the first type (Vector3) must be correct
			return jser::is_specialization<Type, Vector3>();
		}

		template<typename M, typename T>
		std::optional<nlohmann::json> Serialize(T& obj, jser::PushErrorType pushError) const
		{
			if constexpr (IsCorrectType<T>())
			{
				nlohmann::json json_collection = nlohmann::json::array();
				json_collection.push_back(jser::DefaultSerialize<M>(obj.X, pushError));
				json_collection.push_back(jser::DefaultSerialize<M>(obj.Y, pushError));
				json_collection.push_back(jser::DefaultSerialize<M>(obj.Z, pushError));
				return json_collection;
			}
			return std::nullopt;
		}

		template<typename M, typename T>
		std::optional<T> Deserialize(const nlohmann::json& j, jser::PushErrorType pushError) const
		{
			if constexpr (IsCorrectType<T>())
			{
				using V = typename T::value_type;
				T temp;
				temp.X = jser::DefaultDeserialize<M, V>(j.at(0), pushError);
				temp.Y = jser::DefaultDeserialize<M, V>(j.at(1), pushError);
				temp.Z = jser::DefaultDeserialize<M, V>(j.at(2), pushError);
				return temp;
			}
			return std::nullopt;
		}
	};

	CREATE_EXTENDED_JSER_MANAGER_TYPE(JSERExtendedManager, Vector3_Serializer);

	struct Foo : jser::JSerializable
	{
		Vector3<int> myVector = { 3,3,3 };
		jser::JserChunkAppender AddItem() override
		{
			// JSER library knows now type Vector3, because we passed the JSERExtendedManager
			return JSerializable::AddItem().Append(JSER_ADD(JSERExtendedManager, myVector));
		}
	};

	boost::ut::suite Suite = [] {
		using namespace boost::ut;
		"main"_test = []
		{
			Foo foo;
			std::list<jser::JSerError> errorList;
			std::string result = foo.SerializeObjectString(std::back_inserter(errorList));
			assert(errorList.size() == 0);

			// Deserialization Process
			Foo deserialized;
			deserialized.DeserializeObject(result, std::back_inserter(errorList));
			assert(errorList.size() == 0);
		};
	};
}
