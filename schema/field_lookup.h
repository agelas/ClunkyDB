// Does this schema contain Name?
#pragma once

#include "schema/document.h"
#include <type_traits>

namespace clunkydb::schema {
template <typename Schema, fixed_string Name> struct has_field_named;

template <fixed_string Name, typename... Fields>
struct has_field_named<document<Fields...>, Name>
    : std::bool_constant<((Fields::name == Name) || ...)> {};
} // namespace clunkydb::schema
