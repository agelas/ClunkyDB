#include <cassert>
#include <concepts>
#include <utility>

#include "budget/types.h"
#include "query/field_accessor.h"

namespace budget = clunkydb::budget;
namespace query = clunkydb::query;

int main() {
  const budget::PaycheckDoc doc{
      .paycheck_num = 42,
      .date = "2026-07-13",
      .allocations = {},
      .expense_items = {},
      .savings_accounts = {},
  };

  assert((query::FieldAccessor<budget::PaycheckDoc, "PaycheckNum">::get(doc) ==
          42));
  assert((query::FieldAccessor<budget::PaycheckDoc, "Date">::get(doc) ==
          "2026-07-13"));

  return 0;
}
