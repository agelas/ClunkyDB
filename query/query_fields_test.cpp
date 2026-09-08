#include <cassert>

#include "query/query_fields.h"

namespace query = clunkydb::query;

using Query = decltype((query::field<"PaycheckNum"> == 22) &&
                       (query::field<"Date"> == "2026-07-13"));

using Expected =
    query::type_list<query::field_ref<"PaycheckNum">, query::field_ref<"Date">>;

static_assert(std::same_as<query::referenced_fields_t<Query>, Expected>);

int main() { return 0; }
