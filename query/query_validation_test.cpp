#include "budget/paycheck_schema.h"
#include "query/query_validation.h"

namespace budget = clunkydb::budget;
namespace query = clunkydb::query;

using Valid = decltype((query::field<"PaycheckNum"> == 20) &&
                       (query::field<"Date"> == "2026-07-13"));

using Invalid = decltype(query::field<"NotAField"> == 22);

static_assert(query::ValidQuery<budget::PaycheckSchema, Valid>);
static_assert(!query::ValidQuery<budget::PaycheckSchema, Invalid>);

int main() { return 0; }
