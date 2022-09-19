
# Json Serializer for C++

Simple to use json serializer for c++

## TODO

- Error Handling DONE
- Allow custom serialization logic DONE
- Deal with Pointer (Sounds broken to me)
- Add testing with https://github.com/boost-ext/ut
	- Circular dependency test
	- Validation test // DONE
	- Error type test -> Also error for pointer and circular dependencies // DONE
	- Pointer test
	- Rewrite Validation // DONE
	- Pointer problem, after shared pointer is deserialized the pointer is replaced, please update all related shared pointer and weak ptr
- Add support for the following collections
    - stack DONE
    - queue DONE
    - priority queue DONE
    - multimap DONE
    - unordered multimap DONE 
    - bitset DONE
    - Any container could be filled with polymorph objects DONE
    - test map of array of something else of .... DONE
    - Bessere test schreiben lul DONE 
    - Füge korrekte typ identifikationstest hinzu DONE
    - Überarbeite custom serialization, 
      sodass custom types standardmäßig deserialisiert werden können DONE
- Create namespace around the library
- Write docs inside this readme 

- You can pass dereferenced raw pointers as long as they are not null !!
- No support for weak_ptr because they can expire
- Smart pointers are deserialized by calling make_unique or make_shared. References need to be updated after deserialization. This can be done by overriding the validation virtual function

## Usage


Coming soon