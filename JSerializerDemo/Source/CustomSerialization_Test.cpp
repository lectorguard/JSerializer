#define BOOST_UT_DISABLE_MODULE // After updgrading to VS 2022 we can use modules
#include "boost/ut.hpp"
#include "JSerializer.h"
#include <iostream>
#include <vector>
#include <functional>

// TO DO , write new system similar to the default serializer

namespace CustomSerialization_Test
{
    template<typename T>
    struct Vector3
    {
#if defined(__clang__)

#elif defined(__GNUC__) || defined(__GNUG__)
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
			return jser::is_specialization<Type, Vector3>();
		}

		template<typename M, typename T>
		std::optional<nlohmann::json> Serialize(T& obj, jser::PushErrorType pushError) const
		{
			if constexpr (IsCorrectType<T>())
			{
				using V = typename T::value_type;

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

	struct CustomBitsetSerializer
	{
		template<typename Type>
		inline static constexpr bool IsCorrectType()
		{
			return jser::is_bitset<Type>();
		}

		template<typename M, typename T>
		std::optional<nlohmann::json> Serialize(T& obj, jser::PushErrorType pushError) const
		{
			if constexpr (IsCorrectType<T>())
			{
				return nlohmann::json(obj.to_string());
			}
			return std::nullopt;
		}

		template<typename M, typename T>
		std::optional<T> Deserialize(const nlohmann::json& j, jser::PushErrorType pushError) const
		{
			using CurrentType = std::remove_reference<T>::type;

			if constexpr (IsCorrectType<T>())
			{
				return T(j.get<std::string>());
			}
			return std::nullopt;
		}
	};

    CREATE_EXTENDED_JSER_MANAGER_TYPE(JSERExtendedManager, Vector3_Serializer);
	CREATE_CUSTOM_JSER_MANAGER_TYPE(JSER_Custom_Manager, CustomBitsetSerializer, jser::PolymorphicSerializer);

    struct Foo : jser::JSerializable
    {
        std::vector<uint64_t> foo = {15,56656565,498,48};
        Vector3<std::bitset<4>> myVector = { std::bitset < 4>{"1100"},std::bitset < 4>{"1110"},std::bitset < 4>{"1111"}};

        void compare(const Foo& Rhs)
        {
            using namespace boost::ut;

            expect(foo == Rhs.foo);
            expect(!myVector.X.to_string().compare(Rhs.myVector.X.to_string()));
            expect(!myVector.Y.to_string().compare(Rhs.myVector.Y.to_string()));
            expect(!myVector.Z.to_string().compare(Rhs.myVector.Z.to_string()));
        };

        jser::JserChunkAppender AddItem() override
        {
            return JSerializable::AddItem().Append(JSER_ADD(JSERExtendedManager, foo, myVector));
        }

    };

	struct FooCustomBitset : jser::JSerializable
	{
		std::bitset<6> foo_bitset{ "101010" };

		jser::JserChunkAppender AddItem() override
		{
			return JSerializable::AddItem().Append(JSER_ADD(JSER_Custom_Manager, foo_bitset));
		}
	};

	struct MixedManagers : jser::JSerializable
	{
		std::bitset<6> foo_bitset{ "101010" };
		Foo foo;

		jser::JserChunkAppender AddItem() override
		{
			// JSER Custom Manager needs Polymorphic Serializer otherwise Foo can not be (de)serialized
			return JSerializable::AddItem().Append(JSER_ADD(JSER_Custom_Manager, foo_bitset, foo));
		}
	};

    boost::ut::suite CustomSerialization_Test = [] {
        using namespace boost::ut;

        "custom serialization"_test = [] {
            Foo foo;
            std::list<jser::JSerError> errorList;
            std::string result = foo.SerializeObjectString(std::back_inserter(errorList));
            expect(errorList.size() == 0) << "Serialization of many object associations throws error";

            Foo deserialized;
            deserialized.DeserializeObject(result, std::back_inserter(errorList));
            expect(errorList.size() == 0) << "Deserialization of many object associations throws error";
            foo.compare(deserialized);
        };


        "custom serialization from string"_test = [] {
            Foo foo;
            std::list<jser::JSerError> errorList;
            std::string result = foo.SerializeObjectString(std::back_inserter(errorList));
            expect(errorList.size() == 0) << "Serialization of many object associations throws error";
            const char* expectedResult = R"({"foo":[15,56656565,498,48],"myVector":["1100","1110","1111"]})";
            expect(!result.compare(expectedResult));

            Foo deserialized;
            deserialized.DeserializeObject(expectedResult, std::back_inserter(errorList));
            expect(errorList.size() == 0) << "Deserialization of many object associations throws error";
            foo.compare(deserialized);
        };

		"complete custom serialization"_test = []
		{
			FooCustomBitset foo;
			std::list<jser::JSerError> errorList;
			std::string result = foo.SerializeObjectString(std::back_inserter(errorList));
			expect(errorList.size() == 0) << "Serialization of many object associations throws error";

			FooCustomBitset deserialized;
			deserialized.DeserializeObject(result, std::back_inserter(errorList));
			expect(errorList.size() == 0) << "Deserialization of many object associations throws error";
			expect(!foo.foo_bitset.to_string().compare(deserialized.foo_bitset.to_string()));
		};

		"mixed manager"_test = []
		{
			MixedManagers foo;
			std::list<jser::JSerError> errorList;
			std::string result = foo.SerializeObjectString(std::back_inserter(errorList));
			expect(errorList.size() == 0) << "Serialization of many object associations throws error";

			MixedManagers deserialized;
			deserialized.DeserializeObject(result, std::back_inserter(errorList));
			expect(errorList.size() == 0) << "Deserialization of many object associations throws error";

			expect(!foo.foo_bitset.to_string().compare(deserialized.foo_bitset.to_string()));
			foo.foo.compare(deserialized.foo);
		};
    };
}