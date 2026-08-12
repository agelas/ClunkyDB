#include <cassert>
#include <concepts>

#include "query/field_ref.h"
#include "query/query_ast.h"
#include "query/query_eval.h"
#include "schema/fixed_string.h"

namespace budget = clunkydb::budget;
namespace schema = clunkydb::schema;
namespace query = clunkydb::query;

const budget::PaycheckDoc doc{
    .paycheck_num = 22,
    .date = "2026-07-13",
    .allocations = {},
    .expense_items = {},
    .savings_accounts = {},
};

const auto paycheck_22 = query::field<"PaycheckNum"> == 22;
const auto june_date = query::field<"Date"> == "2026-07-13";

int main() {

  assert(query::eval(paycheck_22, doc));
  assert(query::eval(paycheck_22 && june_date, doc));

  return 0;
}
