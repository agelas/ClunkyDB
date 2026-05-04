#include <cassert>
#include <cstddef>
#include <cstdint>
#include <span>
#include <type_traits>
#include <vector>

#include "budget/types.h"
#include "containers.h"
#include "primitives.h"
#include "serializer.h"
#include "string.h"

namespace budget = clunkydb::budget;
namespace serialization = clunkydb::serialization;

static_assert(serialization::Serializable<double>);
static_assert(serialization::Serializable<std::int64_t>);
static_assert(serialization::Serializable<std::string>);
static_assert(serialization::Serializable<std::vector<std::int64_t>>);
static_assert(serialization::Serializable<std::vector<double>>);
static_assert(serialization::Serializable<std::vector<std::string>>);

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

  {
    const std::string value = "hello";
    std::vector<std::byte> bytes;

    serialization::Serializer<std::string>::serialize(value, bytes);

    std::size_t offset = 0;
    const std::string decoded =
        serialization::Serializer<std::string>::deserialize(bytes, offset);

    assert(decoded == value);
    assert(offset == sizeof(std::uint64_t) + value.size());
    assert(bytes.size() == sizeof(std::uint64_t) + value.size());
  }

  {
    const std::vector<std::int64_t> value = {1, 2, 3, 4, 5};
    std::vector<std::byte> bytes;

    serialization::Serializer<std::vector<std::int64_t>>::serialize(value,
                                                                    bytes);

    std::size_t offset = 0;
    const std::vector<std::int64_t> decoded =
        serialization::Serializer<std::vector<std::int64_t>>::deserialize(
            bytes, offset);

    assert(decoded == value);
    assert(offset ==
           sizeof(std::size_t) + (value.size() * sizeof(std::int64_t)));
    assert(bytes.size() ==
           sizeof(std::size_t) + (value.size() * sizeof(std::int64_t)));
  }

  {
    const std::vector<double> value = {1.1, 2.2, 3.3, 4.45, 5.678};
    std::vector<std::byte> bytes;

    serialization::Serializer<std::vector<double>>::serialize(value, bytes);

    std::size_t offset = 0;
    const std::vector<double> decoded =
        serialization::Serializer<std::vector<double>>::deserialize(bytes,
                                                                    offset);

    assert(decoded == value);
    assert(offset == sizeof(std::size_t) + (value.size() * sizeof(double)));
    assert(bytes.size() ==
           sizeof(std::size_t) + (value.size() * sizeof(double)));
  }

  {
    const std::vector<std::string> value = {"hello", "world", "string", "test",
                                            "123"};
    std::vector<std::byte> bytes;

    serialization::Serializer<std::vector<std::string>>::serialize(value,
                                                                   bytes);

    std::size_t offset = 0;
    const std::vector<std::string> decoded =
        serialization::Serializer<std::vector<std::string>>::deserialize(
            bytes, offset);
    assert(decoded == value);
    // Calculate expected size: (3 bytes for size) + (5 + 5 + 6 + 4 +3) + (5*8)
    // bytes for string sizes
    std::size_t expected_size = sizeof(std::size_t);
    for (const auto &str : value) {
      expected_size += sizeof(std::uint64_t) + str.size();
    }
    assert(offset == expected_size);
    assert(bytes.size() == expected_size);
  }

  // Test empty vector
  {
    const std::vector<std::int64_t> value = {};
    std::vector<std::byte> bytes;

    serialization::Serializer<std::vector<std::int64_t>>::serialize(value,
                                                                    bytes);

    std::size_t offset = 0;
    const std::vector<std::int64_t> decoded =
        serialization::Serializer<std::vector<std::int64_t>>::deserialize(
            bytes, offset);

    assert(decoded == value);
    assert(offset == sizeof(std::size_t));
    assert(bytes.size() == sizeof(std::size_t));
  }

  // Test vector with single element
  {

    const std::vector<std::int64_t> value = {1};
    std::vector<std::byte> bytes;

    serialization::Serializer<std::vector<std::int64_t>>::serialize(value,
                                                                    bytes);

    std::size_t offset = 0;
    const std::vector<std::int64_t> decoded =
        serialization::Serializer<std::vector<std::int64_t>>::deserialize(
            bytes, offset);

    assert(decoded == value);
    assert(offset == sizeof(std::size_t) + sizeof(std::int64_t));
    assert(bytes.size() == sizeof(std::size_t) + sizeof(std::int64_t));
  }

  return 0;
}
