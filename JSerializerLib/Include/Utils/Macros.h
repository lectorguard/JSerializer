#pragma once
#include <variant>

#define JSER_HELPER_EXPAND( x ) x
#define JSER_HELPER_F(x, ...) X = x and VA_ARGS = __VA_ARGS__
#define JSER_HELPER_G(...) JSER_HELPER_EXPAND( JSER_HELPER_F(__VA_ARGS__) )


// Make a FOREACH macro
#define JSER_HELPER_FE_0(WHAT)
#define JSER_HELPER_FE_1(WHAT, X) WHAT(X)
#define JSER_HELPER_FE_2(WHAT, X, ...) WHAT(X)JSER_HELPER_EXPAND(JSER_HELPER_FE_1(WHAT, __VA_ARGS__))
#define JSER_HELPER_FE_3(WHAT, X, ...) WHAT(X)JSER_HELPER_EXPAND(JSER_HELPER_FE_2(WHAT, __VA_ARGS__))
#define JSER_HELPER_FE_4(WHAT, X, ...) WHAT(X)JSER_HELPER_EXPAND(JSER_HELPER_FE_3(WHAT, __VA_ARGS__))
#define JSER_HELPER_FE_5(WHAT, X, ...) WHAT(X)JSER_HELPER_EXPAND(JSER_HELPER_FE_4(WHAT, __VA_ARGS__))
#define JSER_HELPER_FE_6(WHAT, X, ...) WHAT(X)JSER_HELPER_EXPAND(JSER_HELPER_FE_5(WHAT, __VA_ARGS__))
#define JSER_HELPER_FE_7(WHAT, X, ...) WHAT(X)JSER_HELPER_EXPAND(JSER_HELPER_FE_6(WHAT, __VA_ARGS__))
#define JSER_HELPER_FE_8(WHAT, X, ...) WHAT(X)JSER_HELPER_EXPAND(JSER_HELPER_FE_7(WHAT, __VA_ARGS__))
#define JSER_HELPER_FE_9(WHAT, X, ...) WHAT(X)JSER_HELPER_EXPAND(JSER_HELPER_FE_8(WHAT, __VA_ARGS__))
#define JSER_HELPER_FE_10(WHAT, X, ...) WHAT(X)JSER_HELPER_EXPAND(JSER_HELPER_FE_9(WHAT, __VA_ARGS__))
#define JSER_HELPER_FE_11(WHAT, X, ...) WHAT(X)JSER_HELPER_EXPAND(JSER_HELPER_FE_10(WHAT, __VA_ARGS__))
#define JSER_HELPER_FE_12(WHAT, X, ...) WHAT(X)JSER_HELPER_EXPAND(JSER_HELPER_FE_11(WHAT, __VA_ARGS__))
#define JSER_HELPER_FE_13(WHAT, X, ...) WHAT(X)JSER_HELPER_EXPAND(JSER_HELPER_FE_12(WHAT, __VA_ARGS__))
#define JSER_HELPER_FE_14(WHAT, X, ...) WHAT(X)JSER_HELPER_EXPAND(JSER_HELPER_FE_13(WHAT, __VA_ARGS__))
#define JSER_HELPER_FE_15(WHAT, X, ...) WHAT(X)JSER_HELPER_EXPAND(JSER_HELPER_FE_14(WHAT, __VA_ARGS__))
#define JSER_HELPER_FE_16(WHAT, X, ...) WHAT(X)JSER_HELPER_EXPAND(JSER_HELPER_FE_15(WHAT, __VA_ARGS__))
#define JSER_HELPER_FE_17(WHAT, X, ...) WHAT(X)JSER_HELPER_EXPAND(JSER_HELPER_FE_16(WHAT, __VA_ARGS__))
//... repeat as needed

#define JSER_HELPER_GET_MACRO(_0,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,NAME,...) NAME 
#define JSER_HELPER_FOR_EACH(action,...) \
   JSER_HELPER_EXPAND(JSER_HELPER_GET_MACRO(_0,__VA_ARGS__,JSER_HELPER_FE_17,JSER_HELPER_FE_16,JSER_HELPER_FE_15,JSER_HELPER_FE_14,JSER_HELPER_FE_13, \
                        JSER_HELPER_FE_12,JSER_HELPER_FE_11,JSER_HELPER_FE_10,JSER_HELPER_FE_9,JSER_HELPER_FE_8,JSER_HELPER_FE_7,JSER_HELPER_FE_6,JSER_HELPER_FE_5, \
                        JSER_HELPER_FE_4,JSER_HELPER_FE_3,JSER_HELPER_FE_2,JSER_HELPER_FE_1, JSER_HELPER_FE_0,)(action,__VA_ARGS__))

// Actions
#define JSER_HELPER_COMMA ,
#define JSER_HELPER_STRWITHCOMMA(X) #X JSER_HELPER_COMMA
#define JSER_HELPER_COMMACONTENT(X) JSER_HELPER_COMMA X
#define JSER_HELPER_OPEN_BRACKET  {
#define JSER_HELPER_CLOSE_BRACKET }

// Create JSER Manager Type for Serialization
#define JSER_HELPER_DEFAUL_SERIALIZATION_CONSTRUCTOR_HELPER jser::ListVectorDequeSerializer ,jser::PolymorphicSerializer, \
                                               jser::SetSerializer,jser::ArraySerializer,jser::ValarraySerializer, \
                                               jser::ForwardListSerializer,jser::MapSerializer,jser::TupleSerializer, \
                                               jser::BitsetSerializer,jser::StackAndQueueSerializer,jser::PriorityQueueSerializer,\
                                               jser::PointerSerializer

#define CREATE_CUSTOM_JSER_MANAGER_TYPE(managerType, ...) using managerType = typename std::variant<__VA_ARGS__>
#define CREATE_EXTENDED_JSER_MANAGER_TYPE(managerType, ...) using managerType = typename std::variant<JSER_HELPER_DEFAUL_SERIALIZATION_CONSTRUCTOR_HELPER JSER_HELPER_FOR_EACH(JSER_HELPER_COMMACONTENT ,__VA_ARGS__)>
#define CREATE_DEFAULT_JSER_MANAGER_TYPE(managerType) using managerType = typename std::variant<JSER_HELPER_DEFAUL_SERIALIZATION_CONSTRUCTOR_HELPER>

// Add elements to serialize 
#define JSER_ADD(managerType, ...) CreateSerializeItem<managerType>(JSER_HELPER_OPEN_BRACKET JSER_HELPER_FOR_EACH(JSER_HELPER_STRWITHCOMMA,__VA_ARGS__) \
                                    JSER_HELPER_CLOSE_BRACKET JSER_HELPER_FOR_EACH(JSER_HELPER_COMMACONTENT,__VA_ARGS__))