#pragma once

#include "query/query_ast.h"

namespace clunkydb::query {
template <typename... Types> struct type_list {};

template <typename Lhs, typename Rhs> struct concat;

template <typename... Lhs, typename... Rhs>
struct concat<type_list<Lhs...>, type_list<Rhs...>> {
  using type = type_list<Lhs..., Rhs...>;
};

template <typename Lhs, typename Rhs>
using concat_t = typename concat<Lhs, Rhs>::type;

template <typename Query> struct referenced_fields;
} // namespace clunkydb::query
