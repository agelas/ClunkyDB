#pragma once

#include <cstddef>
#include <span>
#include <stdexcept>
#include <type_traits>
#include <vector>

#include "budget/types.h"
#include "serialization/containers.h"
#include "serialization/primitives.h"
#include "serialization/serializer.h"

namespace clunkydb::serialization {
using budget::Allocation;
template <> struct Serializer<Allocation> {
  static void serialize(const Allocation &value,
                        std::vector<std::byte> &output) {
    Serializer<double>::serialize(value.essential, output);
    Serializer<double>::serialize(value.non_essential, output);
    Serializer<double>::serialize(value.savings, output);
  }

  static auto deserialize(std::span<const std::byte> input,
                          std::size_t &offset) -> Allocation {
    Allocation value{};
    value.essential = Serializer<double>::deserialize(input, offset);
    value.non_essential = Serializer<double>::deserialize(input, offset);
    value.savings = Serializer<double>::deserialize(input, offset);
    return value;
  }
};
} // namespace clunkydb::serialization
