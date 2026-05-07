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

int main() {
  {
    const budget::Allocation value{0.5, 0.3, 0.2};
    std::vector<std::byte> bytes;

    serialization::Serializer<budget::Allocation>::serialize(value, bytes);

    std::size_t offset = 0;
    const auto decoded =
        serialization::Serializer<budget::Allocation>::deserialize(bytes,
                                                                   offset);

    assert(decoded.essential == value.essential);
    assert(decoded.non_essential == value.non_essential);
    assert(decoded.savings == value.savings);
    assert(bytes.size() == 3 * sizeof(double));
    assert(offset == 3 * sizeof(double));
  }

  {
    // Verify field order explicitly
    const budget::Allocation value{0.5, 0.3, 0.2};
    std::vector<std::byte> bytes;

    serialization::Serializer<budget::Allocation>::serialize(value, bytes);

    std::size_t offset = 0;
    const double essential =
        serialization::Serializer<double>::deserialize(bytes, offset);
    const double non_essential =
        serialization::Serializer<double>::deserialize(bytes, offset);
    const double savings =
        serialization::Serializer<double>::deserialize(bytes, offset);

    assert(essential == value.essential);
    assert(non_essential == value.non_essential);
    assert(savings == value.savings);
    assert(bytes.size() == 3 * sizeof(double));
    assert(offset == 3 * sizeof(double));
  }
  return 0;
}
