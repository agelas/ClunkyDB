#pragma once

#include "query/field_ref.h"
#include "schema/fixed_string.h"

namespace clunkydb::query {

template <typename Field, typename Value> struct eq {
  Field field;
  Value value;
};

template <schema::fixed_string Name, typename Value>
constexpr auto operator==(field_ref<Name> field,
                          Value value) -> eq<field_ref<Name>, Value> {
  return eq<field_ref<Name>, Value>{
      .field = field,
      .value = value,
  };
}

template <typename Lhs, typename Rhs> struct logical_and {
  Lhs lhs;
  Rhs rhs;
};

template <typename Lhs, typename Rhs>
constexpr auto operator&&(Lhs lhs, Rhs rhs) -> logical_and<Lhs, Rhs> {
  return logical_and<Lhs, Rhs>{
      .lhs = lhs,
      .rhs = rhs,
  };
}
} // namespace clunkydb::query
