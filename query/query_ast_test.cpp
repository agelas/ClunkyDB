#include <cassert>

#include "query/field_ref.h"
#include "schema/fixed_string.h"

namespace schema = clunkydb::schema;
namespace query = clunkydb::query;

static_assert(query::field_ref<"name">::name == schema::fixed_string{"name"});

int main() { return 0; }
