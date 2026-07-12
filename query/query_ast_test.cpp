#include <cassert>
#include <concepts>

#include "query/field_ref.h"
#include "query/query_ast.h"
#include "schema/fixed_string.h"

namespace schema = clunkydb::schema;
namespace query = clunkydb::query;

static_assert(query::field_ref<"name">::name == schema::fixed_string{"name"});
static_assert(query::field<"name">.name == schema::fixed_string{"name"});

using Expected = query::eq<query::field_ref<"PaycheckNum">, int>;
static_assert(
    std::same_as<decltype(query::field<"PaycheckNum"> == 42), Expected>);

using Left = query::eq<query::field_ref<"PaycheckNum">, int>;
using Right = query::eq<query::field_ref<"Date">, const char *>;
static_assert(std::same_as<decltype((query::field<"PaycheckNum"> == 42) &&
                                    (query::field<"Date"> == "2026-06-22")),
                           query::logical_and<Left, Right>>);

int main() { return 0; }
