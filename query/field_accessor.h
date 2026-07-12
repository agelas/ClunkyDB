#pragma once

#include "budget/types.h"
#include "schema/fixed_string.h";
#include <cstdint>

namespace clunkydb::query {
template <typename Doc, schema::fixed_string Name> struct FieldAccessor;

template <> struct FieldAccessor<budget::PaycheckDoc, "PaycheckNum"> {
  static auto get(const budget::PaycheckDoc &doc) -> const std::int64_t & {
    return doc.paycheck_num;
  }
};
} // namespace clunkydb::query
