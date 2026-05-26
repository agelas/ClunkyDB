#include <cassert>
#include <cstddef>
#include <span>
#include <stdexcept>
#include <type_traits>
#include <vector>

#include "budget/types.h"
#include "serialization/domain.h"
#include "serialization/primitives.h"
#include "serialization/serializer.h"

namespace budget = clunkydb::budget;
namespace serialization = clunkydb::serialization;

static_assert(serialization::Serializable<budget::Allocation>);
static_assert(serialization::Serializable<budget::ExpenseType>);
static_assert(serialization::Serializable<budget::ExpenseItem>);
static_assert(serialization::Serializable<budget::SavingsAccount>);
static_assert(serialization::Serializable<budget::PaycheckDoc>);

void assert_equal(const budget::Allocation &lhs,
                  const budget::Allocation &rhs) {
  assert(lhs.essential == rhs.essential);
  assert(lhs.non_essential == rhs.non_essential);
  assert(lhs.savings == rhs.savings);
}

void assert_equal(const budget::ExpenseItem &lhs,
                  const budget::ExpenseItem &rhs) {
  assert(lhs.type == rhs.type);
  assert(lhs.name == rhs.name);
  assert(lhs.cost == rhs.cost);
  assert(lhs.category == rhs.category);
  assert(lhs.recurring == rhs.recurring);
}

void assert_equal(const budget::SavingsAccount &lhs,
                  const budget::SavingsAccount &rhs) {
  assert(lhs.account_name == rhs.account_name);
  assert(lhs.type == rhs.type);
  assert(lhs.goal == rhs.goal);
  assert(lhs.current_value == rhs.current_value);
}

void testPaycheckDoc() {
  {
    const budget::PaycheckDoc value{
        .paycheck_num = 1,
        .date = "2026-05-22",
        .allocations = budget::Allocation{0.5, 0.3, 0.2},
        .expense_items = {budget::ExpenseItem{
                              .type = budget::ExpenseType::Essential,
                              .name = "Rent",
                              .cost = 2000,
                              .category = "Housing",
                              .recurring = true,
                          },
                          budget::ExpenseItem{
                              .type = budget::ExpenseType::NonEssential,
                              .name = "Thing",
                              .cost = 200,
                              .category = "Misc",
                              .recurring = false,
                          }},
        .savings_accounts = {budget::SavingsAccount{
                                 .account_name = "Some Savings",
                                 .type = "HYSA",
                                 .goal = 1000.00,
                                 .current_value = 600.05,
                             },
                             budget::SavingsAccount{
                                 .account_name = "Brokerage",
                                 .type = "Investment",
                                 .goal = 5000.00,
                                 .current_value = 2000.23,
                             }}};

    std::vector<std::byte> bytes;
    serialization::Serializer<budget::PaycheckDoc>::serialize(value, bytes);

    std::size_t offset = 0;
    const auto decoded =
        serialization::Serializer<budget::PaycheckDoc>::deserialize(bytes,
                                                                    offset);

    assert(decoded.paycheck_num == value.paycheck_num);
    assert(decoded.date == value.date);
    assert(decoded.amount == value.amount);
    assert_equal(decoded.allocations, value.allocations);

    assert(decoded.expense_items.size() == value.expense_items.size());
    assert(decoded.savings_accounts.size() == value.savings_accounts.size());

    assert_equal(decoded.expense_items[0], value.expense_items[0]);
    assert_equal(decoded.expense_items[1], value.expense_items[1]);

    assert_equal(decoded.savings_accounts[0], value.savings_accounts[0]);
    assert_equal(decoded.savings_accounts[1], value.savings_accounts[1]);

    assert(offset == bytes.size());
  }
}

int main() {
  {
    const budget::Allocation value{0.5, 0.3, 0.2};
    std::vector<std::byte> bytes;

    serialization::Serializer<budget::Allocation>::serialize(value, bytes);

    std::size_t offset = 0;
    const auto decoded =
        serialization::Serializer<budget::Allocation>::deserialize(bytes,
                                                                   offset);

    assert_equal(decoded, value);
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

  {
    // Deserialize respects a non-zero offset
    const double prefix = 95.0;
    const budget::Allocation value{0.5, 0.3, 0.2};
    std::vector<std::byte> bytes;

    serialization::Serializer<double>::serialize(prefix, bytes);
    const std::size_t allocation_start = bytes.size();
    serialization::Serializer<budget::Allocation>::serialize(value, bytes);

    std::size_t offset = allocation_start;
    const auto decoded =
        serialization::Serializer<budget::Allocation>::deserialize(bytes,
                                                                   offset);

    assert_equal(decoded, value);
    assert(offset == allocation_start + 3 * sizeof(double));
  }

  {
    const budget::ExpenseType value = budget::ExpenseType::Essential;
    std::vector<std::byte> bytes;

    serialization::Serializer<budget::ExpenseType>::serialize(value, bytes);

    std::size_t offset = 0;
    const budget::ExpenseType decoded =
        serialization::Serializer<budget::ExpenseType>::deserialize(bytes,
                                                                    offset);

    assert(decoded == value);
    assert(offset == sizeof(std::underlying_type_t<budget::ExpenseType>));
    assert(bytes.size() == sizeof(std::underlying_type_t<budget::ExpenseType>));
  }

  {
    const budget::ExpenseType value = budget::ExpenseType::NonEssential;
    std::vector<std::byte> bytes;

    serialization::Serializer<budget::ExpenseType>::serialize(value, bytes);

    std::size_t offset = 0;
    const budget::ExpenseType decoded =
        serialization::Serializer<budget::ExpenseType>::deserialize(bytes,
                                                                    offset);

    assert(decoded == value);
    assert(offset == sizeof(std::underlying_type_t<budget::ExpenseType>));
    assert(bytes.size() == sizeof(std::underlying_type_t<budget::ExpenseType>));
  }

  {
    // Explicit layout test. Serializer is really storing the underlying integer
    // representation, not something wonky
    const budget::ExpenseType value = budget::ExpenseType::NonEssential;
    std::vector<std::byte> bytes;

    serialization::Serializer<budget::ExpenseType>::serialize(value, bytes);

    std::size_t offset = 0;
    const auto raw = serialization::Serializer<
        std::underlying_type_t<budget::ExpenseType>>::deserialize(bytes,
                                                                  offset);

    assert(raw ==
           static_cast<std::underlying_type_t<budget::ExpenseType>>(value));
    assert(offset == bytes.size());
  }

  {
    const budget::ExpenseItem value{
        .type = budget::ExpenseType::Essential,
        .name = "Rent",
        .cost = 1000.42,
        .category = "Housing",
        .recurring = true,
    };
    std::vector<std::byte> bytes;

    serialization::Serializer<budget::ExpenseItem>::serialize(value, bytes);

    std::size_t offset = 0;
    const auto decoded =
        serialization::Serializer<budget::ExpenseItem>::deserialize(bytes,
                                                                    offset);
    assert_equal(decoded, value);
    assert(offset == bytes.size());
  }

  {
    const budget::ExpenseItem value{
        .type = budget::ExpenseType::NonEssential,
        .name = "Covfefe",
        .cost = 19.99,
        .category = "Entertainment",
        .recurring = false,
    };
    std::vector<std::byte> bytes;

    serialization::Serializer<budget::ExpenseItem>::serialize(value, bytes);

    std::size_t offset = 0;
    const auto type =
        serialization::Serializer<budget::ExpenseType>::deserialize(bytes,
                                                                    offset);
    const auto name =
        serialization::Serializer<std::string>::deserialize(bytes, offset);
    const auto cost =
        serialization::Serializer<double>::deserialize(bytes, offset);
    const auto category =
        serialization::Serializer<std::string>::deserialize(bytes, offset);
    const auto recurring =
        serialization::Serializer<bool>::deserialize(bytes, offset);

    assert(type == value.type);
    assert(name == value.name);
    assert(cost == value.cost);
    assert(category == value.category);
    assert(recurring == value.recurring);
    assert(offset == bytes.size());
  }

  {
    const std::int64_t prefix = 12345;
    const budget::ExpenseItem value{
        .type = budget::ExpenseType::Essential,
        .name = "Stuff",
        .cost = 123.45,
        .category = "Things",
        .recurring = false,
    };
    std::vector<std::byte> bytes;

    serialization::Serializer<std::int64_t>::serialize(prefix, bytes);
    const std::size_t item_start = bytes.size();
    serialization::Serializer<budget::ExpenseItem>::serialize(value, bytes);

    std::size_t offset = item_start;
    const auto decoded =
        serialization::Serializer<budget::ExpenseItem>::deserialize(bytes,
                                                                    offset);
    assert_equal(decoded, value);
    assert(offset == bytes.size());
  }

  {
    const budget::ExpenseItem value{
        .type = budget::ExpenseType::Essential,
        .name = "",
        .cost = 0.000,
        .category = "",
        .recurring = false,
    };
    std::vector<std::byte> bytes;

    serialization::Serializer<budget::ExpenseItem>::serialize(value, bytes);

    std::size_t offset = 0;
    const auto decoded =
        serialization::Serializer<budget::ExpenseItem>::deserialize(bytes,
                                                                    offset);
    assert_equal(decoded, value);
    assert(offset == bytes.size());
  }

  {
    const budget::SavingsAccount value{
        .account_name = "Some Savings",
        .type = "HYSA",
        .goal = 1000.00,
        .current_value = 420.00,
    };
    std::vector<std::byte> bytes;
    serialization::Serializer<budget::SavingsAccount>::serialize(value, bytes);

    std::size_t offset = 0;
    const auto decoded =
        serialization::Serializer<budget::SavingsAccount>::deserialize(bytes,
                                                                       offset);
    assert_equal(decoded, value);
    assert(offset == bytes.size());
  }

  {
    const budget::SavingsAccount value{
        .account_name = "Some Savings",
        .type = "HYSA",
        .goal = 1000.00,
        .current_value = 420.00,
    };
    std::vector<std::byte> bytes;
    serialization::Serializer<budget::SavingsAccount>::serialize(value, bytes);

    std::size_t offset = 0;
    const std::string account_name =
        serialization::Serializer<std::string>::deserialize(bytes, offset);
    const std::string type =
        serialization::Serializer<std::string>::deserialize(bytes, offset);
    const double goal =
        serialization::Serializer<double>::deserialize(bytes, offset);
    const double current_value =
        serialization::Serializer<double>::deserialize(bytes, offset);

    assert(account_name == value.account_name);
    assert(type == value.type);
    assert(goal == value.goal);
    assert(current_value == value.current_value);
    assert(offset == bytes.size());
  }

  {
    const double prefix = 44.0;
    const budget::SavingsAccount value{
        .account_name = "Some Savings",
        .type = "HYSA",
        .goal = 1000.00,
        .current_value = 420.00,
    };
    std::vector<std::byte> bytes;
    serialization::Serializer<double>::serialize(prefix, bytes);
    const std::size_t allocation_start = bytes.size();
    serialization::Serializer<budget::SavingsAccount>::serialize(value, bytes);

    std::size_t offset = allocation_start;
    const auto decoded =
        serialization::Serializer<budget::SavingsAccount>::deserialize(bytes,
                                                                       offset);
    assert_equal(decoded, value);
    assert(offset == bytes.size());
  }

  {
    const budget::SavingsAccount value{
        .account_name = "",
        .type = "",
        .goal = 0,
        .current_value = 0,
    };
    std::vector<std::byte> bytes;
    serialization::Serializer<budget::SavingsAccount>::serialize(value, bytes);

    std::size_t offset = 0;
    const auto decoded =
        serialization::Serializer<budget::SavingsAccount>::deserialize(bytes,
                                                                       offset);
    assert_equal(decoded, value);
    assert(offset == bytes.size());
  }

  {
    // Trunacted input failure
    const budget::SavingsAccount value{
        .account_name = "Bank Checking",
        .type = "Checking",
        .goal = 500,
        .current_value = 222.25,
    };
    std::vector<std::byte> bytes;
    serialization::Serializer<budget::SavingsAccount>::serialize(value, bytes);
    bytes.pop_back();

    std::size_t offset = 0;
    bool threw = false;

    try {
      (void)serialization::Serializer<budget::SavingsAccount>::deserialize(
          bytes, offset);
    } catch (const std::runtime_error &) {
      threw = true;
    }
    assert(threw);
  }

  testPaycheckDoc();

  return 0;
}
