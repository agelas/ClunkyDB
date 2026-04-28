#ifndef CLUNKYDB_SERIALIZATION_PRIMITIVES_H_
#define CLUNKYDB_SERIALIZATION_PRIMITIVES_H_

#include <cstddef>
#include <cstring>
#include <span>
#include <stdexcept>
#include <type_traits>
#include <vector>

#include "serialization/serializer.h"

namespace clunkydb::serialization {
template <typename T>
  requires std::is_arithmetic_v<T>
struct Serializer<T> {
  static void serialize(const T &value, std::vector<std::byte> &output) {
    const auto *raw = reinterpret_cast<const std::byte *>(&value);
    output.insert(output.end(), raw, raw + sizeof(T));
  }

  static auto deserialize(std::span<const std::byte> input,
                          std::size_t &offset) -> T {
    if (offset + sizeof(T) > input.size()) {
      throw std::runtime_error(
          "not enough bytes to deserialize arithmetic value");
    }

    T value{};
    std::memcpy(&value, input.data() + offset, sizeof(T));
    offset += sizeof(T);
    return value;
  }
};
} // namespace clunkydb::serialization

#endif
