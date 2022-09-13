#pragma once

#define EXPAND( x ) x
#define F(x, ...) X = x and VA_ARGS = __VA_ARGS__
#define G(...) EXPAND( F(__VA_ARGS__) )


// Make a FOREACH macro
#define FE_0(WHAT)
#define FE_1(WHAT, X) WHAT(X)
#define FE_2(WHAT, X, ...) WHAT(X)EXPAND(FE_1(WHAT, __VA_ARGS__))
#define FE_3(WHAT, X, ...) WHAT(X)EXPAND(FE_2(WHAT, __VA_ARGS__))
#define FE_4(WHAT, X, ...) WHAT(X)EXPAND(FE_3(WHAT, __VA_ARGS__))
#define FE_5(WHAT, X, ...) WHAT(X)EXPAND(FE_4(WHAT, __VA_ARGS__))
#define FE_6(WHAT, X, ...) WHAT(X)EXPAND(FE_5(WHAT, __VA_ARGS__))
#define FE_7(WHAT, X, ...) WHAT(X)EXPAND(FE_6(WHAT, __VA_ARGS__))
#define FE_8(WHAT, X, ...) WHAT(X)EXPAND(FE_7(WHAT, __VA_ARGS__))
#define FE_9(WHAT, X, ...) WHAT(X)EXPAND(FE_8(WHAT, __VA_ARGS__))
#define FE_10(WHAT, X, ...) WHAT(X)EXPAND(FE_9(WHAT, __VA_ARGS__))
#define FE_11(WHAT, X, ...) WHAT(X)EXPAND(FE_10(WHAT, __VA_ARGS__))
#define FE_12(WHAT, X, ...) WHAT(X)EXPAND(FE_11(WHAT, __VA_ARGS__))
#define FE_13(WHAT, X, ...) WHAT(X)EXPAND(FE_12(WHAT, __VA_ARGS__))
#define FE_14(WHAT, X, ...) WHAT(X)EXPAND(FE_13(WHAT, __VA_ARGS__))
#define FE_15(WHAT, X, ...) WHAT(X)EXPAND(FE_14(WHAT, __VA_ARGS__))
#define FE_16(WHAT, X, ...) WHAT(X)EXPAND(FE_15(WHAT, __VA_ARGS__))
#define FE_17(WHAT, X, ...) WHAT(X)EXPAND(FE_16(WHAT, __VA_ARGS__))
//... repeat as needed

#define GET_MACRO(_0,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,NAME,...) NAME 
#define FOR_EACH(action,...) \
    EXPAND(GET_MACRO(_0,__VA_ARGS__,FE_17,FE_16,FE_15,FE_14,FE_13,FE_12,FE_11,FE_10,FE_9,FE_8,FE_7,FE_6,FE_5,FE_4,FE_3,FE_2,FE_1,FE_0,)(action,__VA_ARGS__))

// Actions
#define COMMA ,
#define STRWITHCOMMA(X) #X COMMA
#define COMMACONTENT(X) COMMA X
#define OPEN_BRACKET  {
#define CLOSE_BRACKET }
// Helper function
#define JSER_ADD(...) AddDefaultSerializeItem(OPEN_BRACKET FOR_EACH(STRWITHCOMMA,__VA_ARGS__) CLOSE_BRACKET FOR_EACH(COMMACONTENT,__VA_ARGS__))
#define JSER_ADD_CUSTOM(SerializeCB,DeserializeCB)AddCustomSerializeItem(SerializeCB, DeserializeCB)
#define JSER_ADD_VAL(x) AddValidation([this]()x)


#define DEFAULT_SERIALIZE_DEFINITION template<typename T> static nlohmann::json DefaultSerialize(T&& elem, PushErrorType pushError);
#define DEFAULT_DESERIALIZE_DEFINITION template<typename T> static T DefaultDeserialize(const nlohmann::json& j, PushErrorType pushError);