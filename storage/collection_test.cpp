#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <span>
#include <stdexcept>
#include <string>
#include <vector>

#include "budget/paycheck_schema.h"
#include "budget/types.h"
#include "serialization/domain.h"
#include "serialization/primitives.h"
#include "storage/collection.h"

namespace budget = clunkydb::budget;
namespace serialization = clunkydb::serialization;
namespace storage = clunkydb::storage;

namespace {

using PaycheckCollection =
    storage::Collection<budget::PaycheckDoc, budget::PaycheckSchema>;

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

void assert_equal(const budget::PaycheckDoc &lhs,
                  const budget::PaycheckDoc &rhs) {
  assert(lhs.paycheck_num == rhs.paycheck_num);
  assert(lhs.date == rhs.date);
  assert(lhs.amount == rhs.amount);
  assert_equal(lhs.allocations, rhs.allocations);

  assert(lhs.expense_items.size() == rhs.expense_items.size());
  for (std::size_t i = 0; i < lhs.expense_items.size(); ++i) {
    assert_equal(lhs.expense_items[i], rhs.expense_items[i]);
  }

  assert(lhs.savings_accounts.size() == rhs.savings_accounts.size());
  for (std::size_t i = 0; i < lhs.savings_accounts.size(); ++i) {
    assert_equal(lhs.savings_accounts[i], rhs.savings_accounts[i]);
  }
}

auto sample_paycheck_one() -> budget::PaycheckDoc {
  return budget::PaycheckDoc{
      .paycheck_num = 101,
      .date = "2026-05-15",
      .amount = 3250.75,
      .allocations = budget::Allocation{
          .essential = 0.55,
          .non_essential = 0.20,
          .savings = 0.25,
      },
      .expense_items =
          {
              budget::ExpenseItem{
                  .type = budget::ExpenseType::Essential,
                  .name = "Rent",
                  .cost = 1850.00,
                  .category = "Housing",
                  .recurring = true,
              },
              budget::ExpenseItem{
                  .type = budget::ExpenseType::NonEssential,
                  .name = "Concert",
                  .cost = 86.50,
                  .category = "Entertainment",
                  .recurring = false,
              },
          },
      .savings_accounts =
          {
              budget::SavingsAccount{
                  .account_name = "SoFi Savings",
                  .type = "HYSA",
                  .goal = 10000.00,
                  .current_value = 4200.25,
              },
          },
  };
}

auto sample_paycheck_two() -> budget::PaycheckDoc {
  return budget::PaycheckDoc{
      .paycheck_num = 102,
      .date = "2026-05-29",
      .amount = 3291.10,
      .allocations = budget::Allocation{
          .essential = 0.50,
          .non_essential = 0.25,
          .savings = 0.25,
      },
      .expense_items =
          {
              budget::ExpenseItem{
                  .type = budget::ExpenseType::Essential,
                  .name = "Internet",
                  .cost = 70.00,
                  .category = "Utilities",
                  .recurring = true,
              },
              budget::ExpenseItem{
                  .type = budget::ExpenseType::NonEssential,
                  .name = "Coffee",
                  .cost = 18.25,
                  .category = "Dining",
                  .recurring = false,
              },
          },
      .savings_accounts =
          {
              budget::SavingsAccount{
                  .account_name = "Checking",
                  .type = "Checking",
                  .goal = 3000.00,
                  .current_value = 2550.00,
              },
              budget::SavingsAccount{
                  .account_name = "Brokerage",
                  .type = "Investment",
                  .goal = 25000.00,
                  .current_value = 8400.00,
              },
          },
  };
}

auto test_file_path(const std::string &name) -> std::filesystem::path {
  return std::filesystem::temp_directory_path() / name;
}

void write_bytes(std::ofstream &output, std::span<const std::byte> bytes) {
  output.write(reinterpret_cast<const char *>(bytes.data()),
               static_cast<std::streamsize>(bytes.size()));
}

void test_insert_and_load_all() {
  const auto path = test_file_path("clunkydb_collection_roundtrip_test.cdb");
  std::filesystem::remove(path);

  const auto first = sample_paycheck_one();
  const auto second = sample_paycheck_two();

  {
    PaycheckCollection collection{path};
    assert(collection.load_all().empty());

    collection.insert(first);
    collection.insert(second);

    const auto loaded = collection.load_all();
    assert(loaded.size() == 2);
    assert_equal(loaded[0], first);
    assert_equal(loaded[1], second);
  }

  {
    PaycheckCollection reopened{path};
    const auto loaded = reopened.load_all();
    assert(loaded.size() == 2);
    assert_equal(loaded[0], first);
    assert_equal(loaded[1], second);
  }

  std::filesystem::remove(path);
}

void test_rejects_invalid_magic_header() {
  const auto path = test_file_path("clunkydb_collection_bad_magic_test.cdb");
  std::filesystem::remove(path);

  {
    std::ofstream output{path, std::ios::binary};
    const std::array<std::byte, 4> bad_magic{
        std::byte{0x42}, std::byte{0x41}, std::byte{0x44}, std::byte{0x21}};
    write_bytes(output, bad_magic);
  }

  bool threw = false;
  try {
    (void)PaycheckCollection{path};
  } catch (const std::runtime_error &) {
    threw = true;
  }

  assert(threw);
  std::filesystem::remove(path);
}

void test_rejects_truncated_record() {
  const auto path = test_file_path("clunkydb_collection_truncated_test.cdb");
  std::filesystem::remove(path);

  {
    std::ofstream output{path, std::ios::binary};
    const std::array<std::byte, 4> magic{
        std::byte{0x43}, std::byte{0x44}, std::byte{0x42}, std::byte{0x31}};
    write_bytes(output, magic);

    std::vector<std::byte> length;
    serialization::Serializer<std::uint64_t>::serialize(128, length);
    write_bytes(output, length);
  }

  bool threw = false;
  try {
    (void)PaycheckCollection{path};
  } catch (const std::runtime_error &) {
    threw = true;
  }

  assert(threw);
  std::filesystem::remove(path);
}

} // namespace

int main() {
  test_insert_and_load_all();
  test_rejects_invalid_magic_header();
  test_rejects_truncated_record();

  return 0;
}
