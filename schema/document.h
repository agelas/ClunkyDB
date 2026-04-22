#ifndef CLUNKYDB_SCHEMA_DOCUMENT_H_
#define CLUNKYDB_SCHEMA_DOCUMENT_H_

#include <concepts>
#include <tuple>
#include <type_traits>

#include "schema/fixed_string.h"

namespace clunkydb::schema {

template <fixed_string Name, typename T> struct field {
  static constexpr auto name = Name;
  using value_type = T;
};

template <typename... Fields> struct document {
  using fields = std::tuple<Fields...>;
  static constexpr auto field_count = sizeof...(Fields);
};

template <typename T>
concept FieldSpec = requires {
  T::name;
  typename T::value_type;
} && FixedStringLike<decltype(T::name)>;

namespace detail {

template <typename T> struct is_document : std::false_type {};

template <typename... Fields>
struct is_document<document<Fields...>>
    : std::bool_constant<(FieldSpec<Fields> && ...)> {};

} // namespace detail

template <typename T>
concept DocumentSchema = detail::is_document<std::remove_cvref_t<T>>::value;

} // namespace clunkydb::schema

#endif // CLUNKYDB_SCHEMA_DOCUMENT_H_
