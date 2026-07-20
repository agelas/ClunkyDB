#pragma once

#include "query/field_accessor.h"
#include "query/field_ref.h"
#include "query/query_ast.h"
#include "schema/fixed_string.h"

namespace clunkydb::query {

template <schema::fixed_string<Name>, typename Value, typename Doc>
constexpr auto eval(const eq<field_ref<Name>, Value> &query,
                    const Doc &doc) -> bool {
  return eval(query.lhs, doc) && eval(query.rhs, doc);
}
} // namespace clunkydb::query
