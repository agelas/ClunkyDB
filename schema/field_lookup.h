// Does this schema contain Name?
#pragma once

#include "schema/document.h"
#include <type_traits>

namespace clunkydb::schema {
template <typename Schema, fixed_string Name> struct has_field_named;

template <fixed_string Name, typename... Fields>
struct has_field_named<document<Fields...>, Name>
    : std::bool_constant<((Fields::name == Name) || ...)> {};

template <typename Schema, fixed_string Name>
inline constexpr bool has_field_named_v = has_field_named<Schema, Name>::value;
} // namespace clunkydb::schema
