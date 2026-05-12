#pragma once

#include <cstddef>
#include <span>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

#include "budget/types.h"
#include "serialization/containers.h"
#include "serialization/primitives.h"
#include "serialization/serializer.h"
#include "serialization/string.h"

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

using budget::ExpenseItem;
using budget::ExpenseType;
template <> struct Serializer<ExpenseItem> {
  static void serialize(const ExpenseItem &value,
                        std::vector<std::byte> &output) {
    Serializer<ExpenseType>::serialize(value.type, output);
    Serializer<std::string>::serialize(value.name, output);
    Serializer<double>::serialize(value.cost, output);
    Serializer<std::string>::serialize(value.category, output);
    Serializer<bool>::serialize(value.recurring, output);
  }

  static auto deserialize(std::span<const std::byte> input,
                          std::size_t &offset) -> ExpenseItem {
    ExpenseItem value{};
    value.type = Serializer<ExpenseType>::deserialize(input, offset);
    value.name = Serializer<std::string>::deserialize(input, offset);
    value.cost = Serializer<double>::deserialize(input, offset);
    value.category = Serializer<std::string>::deserialize(input, offset);
    value.recurring = Serializer<bool>::deserialize(input, offset);

    return value;
  }
};
} // namespace clunkydb::serialization
