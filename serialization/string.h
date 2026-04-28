#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <span>
#include <stdexcept>
#include <string>
#include <vector>

#include "serialization/primitives.h"
#include "serialization/serializer.h"

namespace clunkydb::serialization {

template <> struct Serializer<std::string> {
  static void serialize(const std::string &value,
                        std::vector<std::byte> &output) {
    Serializer<std::uint64_t>::serialize(value.size(), output);

    const auto *raw = reinterpret_cast<const std::byte *>(value.data());
    output.insert(output.end(), raw, raw + value.size());
  }

  static auto deserialize(std::span<const std::byte> input,
                          std::size_t &offset) -> std::string {
    auto size = Serializer<std::uint64_t>::deserialize(input, offset);

    if (offset + size > input.size()) {
      throw std::runtime_error("not enough bytes to deserialize string");
    }

    std::string result(size, '\0');
    std::memcpy(result.data(), input.data() + offset, size);
    offset += size;
    return result;
  }
};
} // namespace clunkydb::serialization
