#include <cassert>
#include <cstddef>
#include <span>
#include <vector>

#include "budget/types.h"
#include "serialization/domain.h"
#include "serialization/primitives.h"
#include "serialization/serializer.h"

namespace budget = clunkydb::budget;
namespace serialization = clunkydb::serialization;

static_assert(serialization::Serializable<budget::Allocation>);

int main() { return 0; }
