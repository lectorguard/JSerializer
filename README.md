
# JSerializer Library

## Lightweight C++ library for json serialization 

### Features

* Easy to use and set up
* Header only library
* Supports custom Serializer
* Primitive types are supported by default
* Std library collections supported by default
* Smart pointer supported by default
* Inheritance and Associations are supported by default
* Default serializer can be overwritten
* Build on top of well known and tested [nlohmann json](https://github.com/nlohmann/json) library
* Error handling
* Validation
* Tested using boost::ut library
* Circular dependencies detection (Circular dependencies are **not** supported)
* C++ 20

### Supported Compilers

* MSVC (Windows)
* GCC
* Clang (Version 12 or higher)

### Dependencies
* [nlohmann json](https://github.com/nlohmann/json)
* C++ 20

### Add library to cmake Project

* Download the project by hand or with [git submodules](https://www.vogella.com/tutorials/GitSubmodules/article.html)
* You can use the following CMake code to integrate the library

```
# By setting JSERIALIZER_ONLY_LIBRARY, only the library and its dependencies are built
set(JSERIALIZER_ONLY_LIBRARY true)
# Just add the subdirectory in which you copied the project
add_subdirectory(ThirdParty/JSerializer)
# Link the library with your application, the project will always automatically build nlohmann_json and JSerializerLib 
# You need to link always both libraries 
target_link_libraries(<yourAppOrLibrary> nlohmann_json JSerializerLib)
```

* Thats it

### Supported Standard Library Types 
* std::array
* std::list
* std::vector
* std::forward_list
* std::set
* std::multiset
* std::valarray 
* std::deque
* std::unordered_set
* std::unordered_multiset
* std::map
* std::unordered_map
* std::multimap
* std::unordered_multimap
* std::tuple
* std::bitset
* std::stack
* std::queue
* std::priority_queue
* std::unique_ptr
* std::shared_ptr

### How to set up

Just 2 simple steps are necessary to set up the (de)serializer. First, create a manager type, which defines the behavior of the (de)serializer. Second, let your struct/class inherit from jser::JSerializable and override the AddItem() method. Inside the method you can add elements you want to (de)serialize.

```cpp
#include <cassert>
#include "JSerializer.h"

// This manager type shows the (de)serializer 
// how to (de)serialize an object 
// You can give the type a custom name
CREATE_DEFAULT_JSER_MANAGER_TYPE(JSERManager);

struct TestStruct : jser::JSerializable
{
  // All JSerializable objects must be default constructible
  // If you add a custom constructor, 
  // you have to add a default constructor as well

  std::array<int, 3> foo = { 0,0,0 };
  std::list<int> foo_list = { 45,48,513,8,61,86,156 };
  int foo_int = 4;

  // Every JSerializable object has an AddItem function,
  // there we set the variables we want to (de)serialize
  jser::JserChunkAppender AddItem() override
  {
    // With JSER_ADD macro, we can add easily variables by writing the variable name
    // You can also call the raw function call 
    // CreateSerializeItem<JSERManager>({"foo", "foo_list"}, foo, foo_list);
    return JSerializable::AddItem().Append(JSER_ADD(JSERManager, foo, foo_list, foo_int));
  }
};

int main()
{
  // Serialization process
  TestStruct foo;
  std::list<jser::JSerError> errorList;
  std::string result = foo.SerializeObjectString(std::back_inserter(errorList));
  assert(errorList.size() == 0);

  // Deserialization Process
  TestStruct deserialized;
  deserialized.DeserializeObject(result, std::back_inserter(errorList));
  assert(errorList.size() == 0);

  return 0;
}
```

## Features in Detail
### Associations

Any struct/class can (de)serialize other referenced structs/classes as long as they also inherit from JSerializer. If this is not the case you will run into an error. Have a look at the following example. 

```cpp
#include <memory>
#include <cassert>
#include "JSerializer.h"

CREATE_DEFAULT_JSER_MANAGER_TYPE(JSERManager);

struct TestStruct1 : jser::JSerializable
{
  std::string foo_name = "Hello C++";
  jser::JserChunkAppender AddItem() override
  {
    return JSerializable::AddItem().Append(JSER_ADD(JSERManager, foo_name));
  }
};

struct TestStruct2 : jser::JSerializable
{
  TestStruct1 foo_testStruct1;
  std::unique_ptr<TestStruct1> foo_uniquePtr; 
  jser::JserChunkAppender AddItem() override
  {
    return JSerializable::AddItem().Append(JSER_ADD(JSERManager, foo_testStruct1, foo_uniquePtr));
  }
};

int main()
{
  // TestStruct2 will also serialize TestStruct1 inside foo
  TestStruct2 foo;
  foo.foo_uniquePtr = std::make_unique<TestStruct1>();
  std::list<jser::JSerError> errorList;
  std::string result = foo.SerializeObjectString(std::back_inserter(errorList));
  check(errorList.size() == 0);

  // TestStruct2 will also deserialize TestStruct1 inside foo
  TestStruct2 deserialized;
  deserialized.DeserializeObject(result, std::back_inserter(errorList));
  check(errorList.size() == 0);

  return 0;
}
```

### Nested Standard Collections

As long as there is a (de)serializer provided for every used type you can nest types inside each other. JSerializer will (de)serialize your type correctly. 

```cpp
#include <vector>
#include <map>
#include <string>
#include <stack>
#include <cassert>
#include "JSerializer.h"


CREATE_DEFAULT_JSER_MANAGER_TYPE(JSERManager);

struct Foo : jser::JSerializable
{
  int16_t foo_int = 44;
  Foo() {};
  Foo(int foo): foo_int(foo){};
  jser::JserChunkAppender AddItem() override
  {
  	return JSerializable::AddItem().Append(JSER_ADD(JSERManager, foo_int));
  }
};

struct TestStruct : jser::JSerializable
{
  // Complex type will be (de)serialized correctly
  std::map<std::string, std::vector<std::stack<Foo>>> foo_complex_type;
  jser::JserChunkAppender AddItem() override
  {
    return JSerializable::AddItem().Append(JSER_ADD(JSERManager, foo_complex_type));
  }
};

...
```

### Inheritance 

As long as you call the parent implementation of AddItem() inside the child classes, the object will be (de)serialized correctly.


```cpp
#include <cassert>
#include "JSerializer.h"

CREATE_DEFAULT_JSER_MANAGER_TYPE(JSERManager);

struct Foo : jser::JSerializable
{
  uint32_t foo = 9;

  jser::JserChunkAppender AddItem() override
  {
    return JSerializable::AddItem().Append(JSER_ADD(JSERManager, foo));
  }
};

struct Foo1 : Foo
{
  uint32_t foo1 = 76543;

  jser::JserChunkAppender AddItem() override
  {
    // We call Foo::AddItem() instead of JSerializable::AddItem() !!!
    return Foo::AddItem().Append(JSER_ADD(JSERManager, foo1));
  }
};


...
```

### Subclasses

Very similar to associations. Every subclass must inherit from JSerializable and override the AddItem() method.

```cpp
#include <cassert>
#include "JSerializer.h"

CREATE_DEFAULT_JSER_MANAGER_TYPE(JSERManager);

struct Foo : public jser::JSerializable
{
  jser::JserChunkAppender AddItem() override
  {
    return JSerializable::AddItem().Append(JSER_ADD(JSERManager, foo, obj1));
  }

  struct Foo1 : public jser::JSerializable
  {
    int32_t foo = 9;

    jser::JserChunkAppender AddItem() override
    {
      return JSerializable::AddItem().Append(JSER_ADD(JSERManager, foo));
    }
  };

  Foo1 obj1;
  int32_t foo = 5;
};

...
```

### Custom Serializer

You can write your own (de)serializer to replace or extend the default serializer.
Please copy the following template as basis for your own (de)serializer.

```cpp
#include "JSerializer.h"

struct CustomTypeSerializer
{
  template<typename Type>
  inline static constexpr bool IsCorrectType()
  {
    // Use a type trait to check the type (must be constexpr)
    // most commonly used in lib for template types :
    // return jser::is_specialization<Type, CustomType>() 
    return ... ; 
  }

  template<typename M, typename T>
  std::optional<nlohmann::json> Serialize(T& obj, jser::PushErrorType pushError) const
  {
    if constexpr (IsCorrectType<T>())
    {
      nlohmann::json json_collection = nlohmann::json::array();
      // Enter code here for your conversion from obj to nlohmann::json
      // An easy way of doing this is calling recursively 
      // json_collection.push_back(jser::DefaultSerialize<M>(obj.yourElement, pushError));
      // for every element in your type

      // You can call the pushError function in case of an unexpected error like this
      // pushError({ JSerErrorTypes::<MatchingErrorType>, "Description of the error" });
      return json_collection;
    }
    return std::nullopt;
  }

  template<typename M, typename T>
  std::optional<T> Deserialize(const nlohmann::json& j, jser::PushErrorType pushError) const
  {
    if constexpr (IsCorrectType<T>())
    {
      T temp;
      // Enter code here for your conversion from json back to your object type
      // An easy way of doing this is calling recursively 
      // temp.element = jser::DefaultDeserialize<M, decltype(temp.element)>(j.at(0), pushError);
      // for every element in your type

      // You can call the pushError function in case of an unexpected error like this
      // pushError({ JSerErrorTypes::<MatchingErrorType>, "Description of the error" });
      return temp;
    }
    return std::nullopt;
  }
};

// You can now extend the Serializer type with the CustomTypeSerializer
CREATE_EXTENDED_JSER_MANAGER_TYPE(JSERExtendedManager, CustomTypeSerializer);
// Or you dont want to use the default serializer and use instead just you custom types
CREATE_CUSTOM_JSER_MANAGER_TYPE(JSERCustomManager, CustomBitsetSerializer);
// Or you can use a mix of some default serializer and custom serializer
CREATE_CUSTOM_JSER_MANAGER_TYPE(JSERMixedManager, CustomBitsetSerializer,jser::PolymorphicSerializer, jser::ArraySerializer);
```

If you need some more inspiration for your own (de)serializer, have a look at the following example:

```cpp
#include <cassert>
#include "JSerializer.h"

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
    // Ignores all the template stuff, just the name of the first type (Vector3) must be correct
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

...
```


### Validation

Before serialization and after deserialization, you can validate objects by overriding the Validate() function. You can also generate Custom validation errors, which will show up later in your error list. See example :

```cpp
#include <cassert>
#include "JSerializer.h"

CREATE_DEFAULT_JSER_MANAGER_TYPE(JSERManager);

struct Foo : jser::JSerializable
{
  std::stack<float> foo_stack;
  const std::vector<float> stack_elements = {12.5f , 9.5f , 7.5f , 23.5f , 15.5f, 16.5f, 5.0f};
  Foo()
  {
    for (const float& elem : stack_elements)
    {
      foo_stack.emplace(elem);
    }
  }

  void Validate(jser::JSerEvent jser_event, jser::PushErrorType push_error) override
  {
    if (foo_stack.size() != stack_elements.size())
    {
  	  push_error({ jser::JSerErrorTypes::VALIDATION_ERROR, "Check if validation errors ar working"});
  	  return;
    }
  }

  jser::JserChunkAppender AddItem() override
  {
      return JSerializable::AddItem().Append(JSER_ADD(JSERManager, foo_stack));
  }
};

int main()
{
  Foo foo;
  foo.foo_stack.push(156.0f);
  std::list<jser::JSerError> errorList;
  std::string result = foo.SerializeObjectString(std::back_inserter(errorList));
  assert(errorList.size() == 1);
  assert(errorList.front().Error == jser::JSerErrorTypes::VALIDATION_ERROR);
  errorList.clear();

  Foo deserialized;
  deserialized.DeserializeObject(result, std::back_inserter(errorList));
  assert(errorList.size() == 1);
  assert(errorList.front().Error == jser::JSerErrorTypes::VALIDATION_ERROR);

  return 0;
}
```



### Error Handling

When (de)serializing, you need to pass a list or similar to JSerializer. When an error is happening, the serializer will try to continue (de)serializing. At the end you get a list/vector/... of all errors in the correct order. Based on the result you can decide what you want to do. An error can be one of the following :

```cpp
enum class JSerErrorTypes
{
	NO_ERROR,                     // OK
	JSON_ERROR,                   // Error inside the json library
	SETUP_MISSING_ERROR,          // You must call JSER_ADD_... before you can call (de)serializeObject
	MEMBER_ERROR,                 // Failed (de)serializing member variable
	POLYMORPHIC_ERROR,            // Object is polymorphic but it does not inherit from JSerializable
	DEFAULT_SERIALIZATION_ERROR,  // Error was returned during default serialization process
	POINTER_ERROR,                // Weak pointers and circular dependencies are not supported
	VALIDATION_ERROR,             // Custom error which can be called from user during validationprocess
};

```

We can also talk about some edge cases :

* Circular Dependencies are not supported, but will be detected and cause a pointer error
* Weak Pointer are not supported 
* Raw Pointer, (You can pass dereferenced raw pointers to the library as long as they are not nullptr f.e: JSER_ADD(JSERManager, *myRawPtr))
* Shared Pointer will be replaced with a make_shared call on deserialization, so you need to update weak and other shared ptrs after deserialization

Here is an example which produces an error :

```cpp
#include <cassert>
#include "JSerializer.h"

CREATE_DEFAULT_JSER_MANAGER_TYPE(JSERManager);

// Produces error, because AddItem is not overriden and no item was added to JSerializable
struct Foo : jser::JSerializable
{
	int foo = 42;
};

int main()
{
  Foo foo;
  std::list<jser::JSerError> errorList;
  const std::string result = foo.SerializeObjectString(std::back_inserter(errorList));
  for(jser::JSerError err : errorList)
  {
      // Here you can handle the different errors
  } 
};
```

## Summary

You can find all examples under **JSerializerDemo/Examples/** :)

## Bugs, Issues, Questions

Please create a new Issue if something is not working as intended. Thank you for checking out this library.

## License

MIT License