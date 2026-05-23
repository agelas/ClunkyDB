#pragma once

#include <cstddef>
#include <span>
#include <string>
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

using budget::SavingsAccount;
template <> struct Serializer<SavingsAccount> {
  static void serialize(const SavingsAccount &value,
                        std::vector<std::byte> &output) {
    Serializer<std::string>::serialize(value.account_name, output);
    Serializer<std::string>::serialize(value.type, output);
    Serializer<double>::serialize(value.goal, output);
    Serializer<double>::serialize(value.current_value, output);
  }

  static auto deserialize(std::span<const std::byte> input,
                          std::size_t &offset) -> SavingsAccount {
    SavingsAccount value{};
    value.account_name = Serializer<std::string>::deserialize(input, offset);
    value.type = Serializer<std::string>::deserialize(input, offset);
    value.goal = Serializer<double>::deserialize(input, offset);
    value.current_value = Serializer<double>::deserialize(input, offset);

    return value;
  }
};

using budget::PaycheckDoc;
template <> struct Serializer<PaycheckDoc> {
  static void serializer(const PaycheckDoc &value,
                         std::vector<std::byte> &output) {
    Serializer<std::int64_t>::serialize(value.paycheck_num, output);
    Serializer<std::string>::serialize(value.date, output);
    Serializer<double>::serialize(value.amount, output);
    Serializer<Allocation>::serialize(value.allocations, output);
    Serializer<std::vector<ExpenseItem>>::serialize(value.expense_items,
                                                    output);
    Serializer<std::vector<SavingsAccount>>::serialize(value.savings_accounts,
                                                       output);
  }

  static auto deserialize(std::span<const std::byte> input,
                          std::size_t &offset) -> PaycheckDoc {
    PaycheckDoc value{};
    value.paycheck_num = Serializer<std::int64_t>::deserialize(input, offset);
    value.date = Serializer<std::string>::deserialize(input, offset);
    value.amount = Serializer<double>::deserialize(input, offset);
    value.allocations = Serializer<Allocation>::deserialize(input, offset);
    value.expense_items =
        Serializer<std::vector<ExpenseItem>>::deserialize(input, offset);
    value.savings_accounts =
        Serializer<std::vector<SavingsAccount>>::deserialize(input, offset);

    return value;
  }
};
} // namespace clunkydb::serialization
