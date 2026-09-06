#pragma once

#include "query/query_ast.h"
#include "schema/fixed_string.h"

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

template <schema::fixed_string Name, typename Value>
struct referenced_fields<eq<field_ref<Name>, Value>> {
  using type = type_list<field_ref<Name>>;
};

template <typename Lhs, typename Rhs>
struct referenced_fields<logical_and<Lhs, Rhs>> {
  using type = concat_t<typename referenced_fields<Lhs>::type,
                        typename referenced_fields<Rhs>::type>;
};

template <typename Lhs, typename Rhs>
struct referenced_fields<logical_or<Lhs, Rhs>> {
  using type = concat_t<typename referenced_fields<Lhs>::type,
                        typename referenced_fields<Rhs>::type>;
};

template <typename Query>
using referenced_fields_t = typename referenced_fields<Query>::type;
} // namespace clunkydb::query
