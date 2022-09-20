#define BOOST_UT_DISABLE_MODULE // After updgrading to VS 2022 we can use modules
#include "boost/ut.hpp"
#include "JSerializer.h"
#include <memory>

namespace Pointer_Test
{
	CREATE_DEFAULT_JSER_MANAGER_TYPE(JSERManager);

	using namespace jser;

	struct Foo2 : JSerializable
	{
		bool foo_bool;
		std::vector<int> foo_vector;
		Foo2() {};
		Foo2(bool foo_bool, std::vector<int> foo_vector) : foo_bool(foo_bool), foo_vector(foo_vector) {};

		JserChunkAppender AddItem() override
		{
			return JSerializable::AddItem().Append(JSER_ADD(JSERManager, foo_bool, foo_vector));
		}
	};

	struct Foo : JSerializable
	{
		std::unique_ptr<float> unique_ptr;
		std::shared_ptr<float> shared_ptr;
		std::shared_ptr<int> null_ptr = nullptr; 
		std::vector<int>* vec_raw_ptr = new std::vector<int>({ 2, 55, 6, 156, 1565 });
		std::vector<std::set<std::shared_ptr<Foo2>>> complex_example;

		Foo()
		{
			unique_ptr = std::make_unique<float>(5612.0f);
			shared_ptr = std::make_shared<float>(15612.05f);
		}

		JserChunkAppender AddItem() override
		{
			JserChunkAppender appender;
			if (vec_raw_ptr)
				appender.Append(JSER_ADD(JSERManager, *vec_raw_ptr));
			return appender.Append(JSER_ADD(JSERManager, unique_ptr, shared_ptr, complex_example, null_ptr));
		}
	};

	// Circular Dependencies

	struct CB;
	struct CA : JSerializable
	{
		int32_t foo = 6;
		std::shared_ptr<CB> cb;

		JserChunkAppender AddItem() override
		{
			return JSerializable::AddItem().Append(JSER_ADD(JSERManager, foo, cb));
		}

	};

	struct CB : JSerializable
	{
		int32_t foo = 7;
		std::shared_ptr<CA> ca;

		JserChunkAppender AddItem() override
		{
			return JSerializable::AddItem().Append(JSER_ADD(JSERManager, foo, ca));
		}

	};

	boost::ut::suite Association_Test = [] {
		using namespace boost::ut;
		"pointer"_test = []
		{
			Foo foo;
			foo.complex_example = { {std::make_shared<Foo2>(Foo2(false, {1651,556,165,65}))},
									{std::make_shared<Foo2>(Foo2(true, {16565,555166,145665,65455}))} ,
									{std::make_shared<Foo2>(Foo2(false, {151,56,65,5}))} };
			*foo.vec_raw_ptr = {3,3,3};
			std::list<JSerError> errorList;
			std::string result = foo.SerializeObjectString(std::back_inserter(errorList));
			expect(errorList.size() ==  0) << "Pointer could not be successful serialized";

			errorList.clear();
			Foo deserialized;
			deserialized.null_ptr = std::make_shared<int>(6);
			deserialized.DeserializeObject(result, std::back_inserter(errorList));
			expect(errorList.size() == 0) << "Pointer could not be successful deserialized";
			
			expect(*foo.shared_ptr == *deserialized.shared_ptr);
			expect(*foo.unique_ptr == *deserialized.unique_ptr);

			bool isEqual = std::equal(foo.complex_example.begin(), foo.complex_example.end(), deserialized.complex_example.begin(), [](auto Lhs, auto Rhs)
				{
					return std::equal(Lhs.begin(), Lhs.end(), Rhs.begin(), [](auto Lhs, auto Rhs)
						{
							return Lhs->foo_bool == Rhs->foo_bool && Rhs->foo_vector == Lhs->foo_vector;
						});
				});
			expect(isEqual);
			expect(foo.null_ptr.get() == deserialized.null_ptr.get());
			expect(*foo.vec_raw_ptr == *deserialized.vec_raw_ptr);

		};

		"circular dependencies"_test = []
		{
			auto foo_ca = std::make_shared<CA>(CA());
			auto foo_cb = std::make_shared<CB>(CB());

			foo_ca->cb = foo_cb;
			foo_cb->ca = foo_ca;

			std::list<JSerError> errorList;
			std::string result = foo_ca->SerializeObjectString(std::back_inserter(errorList));
			expect(errorList.size() == 1) << "Pointer could not be successful serialized";
			expect(errorList.front().Error == JSerErrorTypes::POINTER_ERROR);

			errorList.clear();
			CA deserialized;
			deserialized.DeserializeObject(result, std::back_inserter(errorList));
			expect(errorList.size() == 0) << "Pointer could not be successful deserialized";
			expect(deserialized.cb->ca.get() == nullptr);
		};

	};
}
