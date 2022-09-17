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
        using value_type = typename T;

        value_type X;
        value_type Y;
        value_type Z;
    };



	struct Vector3_Serializer
	{
		template<typename Type>
		inline static constexpr bool IsCorrectType()
		{
			return is_specialization<Type, Vector3>();
		}

		template<typename M, typename T>
		std::optional<nlohmann::json> Serialize(T& obj, PushErrorType pushError) const
		{
			if constexpr (IsCorrectType<T>())
			{
				using V = typename T::value_type;

				nlohmann::json json_collection = nlohmann::json::array();
                json_collection.push_back(DefaultSerialize<M>(obj.X, pushError));
                json_collection.push_back(DefaultSerialize<M>(obj.Y, pushError));
                json_collection.push_back(DefaultSerialize<M>(obj.Z, pushError));
				return json_collection;
			}
			return std::nullopt;
		}

		template<typename M, typename T>
		std::optional<T> Deserialize(const nlohmann::json& j, PushErrorType pushError) const
		{
			if constexpr (IsCorrectType<T>())
			{
				using V = typename T::value_type;

				T temp;
                temp.X = DefaultDeserialize<M, V>(j.at(0), pushError);
                temp.Y = DefaultDeserialize<M, V>(j.at(1), pushError);
                temp.Z = DefaultDeserialize<M, V>(j.at(2), pushError);
				return temp;
			}
			return std::nullopt;
		}
	};

    CREATE_EXTENDED_JSER_MANAGER_TYPE(JSERManager, Vector3_Serializer);

    struct Foo : JSerializable
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

        JserChunkAppender AddItem() override
        {
            return JSerializable::AddItem().Append(JSER_ADD(JSERManager, foo, myVector));
        }

    };

    boost::ut::suite CustomSerialization_Test = [] {
        using namespace boost::ut;

        "custom serialization"_test = [] {
            Foo foo;
            std::list<JSerError> errorList;
            std::string result = foo.SerializeObjectString(std::back_inserter(errorList));
            expect(errorList.size() == 0) << "Serialization of many object associations throws error";

            Foo deserialized;
            deserialized.DeserializeObject(result, std::back_inserter(errorList));
            expect(errorList.size() == 0) << "Deserialization of many object associations throws error";
            foo.compare(deserialized);
        };


        "custom serialization from string"_test = [] {
            Foo foo;
            std::list<JSerError> errorList;
            std::string result = foo.SerializeObjectString(std::back_inserter(errorList));
            expect(errorList.size() == 0) << "Serialization of many object associations throws error";
            const char* expectedResult = R"({"foo":[15,56656565,498,48],"myVector":["1100","1110","1111"]})";
            expect(!result.compare(expectedResult));

            Foo deserialized;
            deserialized.DeserializeObject(expectedResult, std::back_inserter(errorList));
            expect(errorList.size() == 0) << "Deserialization of many object associations throws error";
            foo.compare(deserialized);
        };
    };
}