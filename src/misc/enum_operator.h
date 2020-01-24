
#pragma once

// Macros to create generic enum operators

#define enum_bitwise_operator_rhs(enum_name, int_name, op) \
constexpr enum_name operator op(enum_name a, enum_name b) \
{ \
    return static_cast<enum_name>(static_cast<int_name>(a) op static_cast<int_name>(b)); \
}

#define enum_bitwise_operator_lhs(enum_name, op_full, op) \
constexpr enum_name& operator op_full(enum_name &a, enum_name b) \
{ \
    a = a op b; \
    return a; \
}

#define enum_bitwise_operator_invert(enum_name, int_name, alloptions) \
constexpr enum_name operator ~(enum_name a) \
{ \
    return static_cast<enum_name>(~static_cast<int_name>(a) & alloptions); \
}
