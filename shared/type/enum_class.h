#pragma once
#include <boost/preprocessor.hpp>

// https://stackoverflow.com/questions/5093460/how-to-convert-an-enum-type-variable-to-a-string/42317427#42317427
#define HELPER1(...) ((__VA_ARGS__)) HELPER2
#define HELPER2(...) ((__VA_ARGS__)) HELPER1
#define HELPER1_END
#define HELPER2_END
#define ADD_PARENTHESES_FOR_EACH_TUPLE_IN_SEQ(sequence) BOOST_PP_CAT(HELPER1 sequence,_END)


#define CREATE_ENUM_ELEMENT_IMPL(elementTuple)                                          \
BOOST_PP_IF(BOOST_PP_EQUAL(BOOST_PP_TUPLE_SIZE(elementTuple), 3),                       \
    BOOST_PP_TUPLE_ELEM(0, elementTuple) = BOOST_PP_TUPLE_ELEM(1, elementTuple),        \
    BOOST_PP_TUPLE_ELEM(0, elementTuple)                                                \
),

#define CREATE_ENUM_ELEMENT(r, data, elementTuple)                                      \
    CREATE_ENUM_ELEMENT_IMPL(BOOST_PP_TUPLE_PUSH_BACK(elementTuple, _))

#define DEFINE_CASE_HAVING_ONLY_ENUM_ELEMENT_NAME(enumName, element)                    \
    case enumName::element : return BOOST_PP_STRINGIZE(element);

#define GENERATE_CASE_FOR_SWITCH(r, enumName, elementTuple)                             \
    DEFINE_CASE_HAVING_ONLY_ENUM_ELEMENT_NAME(enumName, BOOST_PP_TUPLE_ELEM(0, elementTuple))

#define DEFINE_CASE_HAVING_ONLY_RETURN_TRUE(enumName, element) \
    case enumName::element : return true;

#define GENERATE_CASE_FOR_IS_VALID_SWITCH(r, enumName, elementTuple)                    \
    DEFINE_CASE_HAVING_ONLY_RETURN_TRUE(enumName, BOOST_PP_TUPLE_ELEM(0, elementTuple))

// ENUM_CLASS(Name, type,
// (NAME0, value0)
// (NAME1, value1)
// )
#define ENUM_CLASS(enumName, type, enumElements)    \
enum class enumName : type {                                                    \
    BOOST_PP_SEQ_FOR_EACH(                                                      \
        CREATE_ENUM_ELEMENT,                                                    \
        0,                                                                      \
        ADD_PARENTHESES_FOR_EACH_TUPLE_IN_SEQ(enumElements)                     \
    )                                                                           \
};                                                                              \
inline const char* ToString(const enumName element) {                           \
        switch (element) {                                                      \
            BOOST_PP_SEQ_FOR_EACH(                                              \
                GENERATE_CASE_FOR_SWITCH,                                       \
                enumName,                                                       \
                ADD_PARENTHESES_FOR_EACH_TUPLE_IN_SEQ(enumElements)             \
            )                                                                   \
            default: return "[Unknown " BOOST_PP_STRINGIZE(enumName) "]";       \
        }                                                                       \
}                                                                               \
inline bool IsValid(const enumName element) {                                   \
        switch (element) {                                                      \
            BOOST_PP_SEQ_FOR_EACH(                                              \
                GENERATE_CASE_FOR_IS_VALID_SWITCH,                              \
                enumName,                                                       \
                ADD_PARENTHESES_FOR_EACH_TUPLE_IN_SEQ(enumElements)             \
            )                                                                   \
            default: return false;                                              \
        }                                                                       \
}
