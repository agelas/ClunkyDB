#include <cassert>
#include <cstddef>
#include <cstdint>
#include <span>
#include <type_traits>
#include <vector>

#include "budget/types.h"
#include "serializer.h"

namespace budget = clunkydb::budget;
namespace serialization = clunkydb::serialization;

static_assert(serialization::Serializable<double>);
static_assert(serialization::Serializable<std::int64_t>);

int main() {
  {
    const double value = 123.25;
    std::vector<std::byte> bytes;

    serialization::Serializer<double>::serialize(value, bytes);

    std::size_t offset = 0;
    const double decoded =
        serialization::Serializer<double>::deserialize(bytes, offset);

    assert(decoded == value);
    assert(offset == sizeof(double));
    assert(bytes.size() == sizeof(double));
  }

  {
    const std::int64_t value = 42;
    std::vector<std::byte> bytes;

    serialization::Serializer<std::int64_t>::serialize(value, bytes);

    std::size_t offset = 0;
    const std::int64_t decoded =
        serialization::Serializer<std::int64_t>::deserialize(bytes, offset);

    assert(decoded == value);
    assert(offset == sizeof(std::int64_t));
    assert(bytes.size() == sizeof(std::int64_t));
  }

  return 0;
}
