#ifndef CLUNKYDB_BUDGET_TYPES_H_
#define CLUNKYDB_BUDGET_TYPES_H_

#include <cstdint>
#include <string>
#include <vector>

namespace clunkydb::budget {

struct Allocation {
    double essential{};
    double non_essential{};
    double savings{};
};

enum class ExpenseType {
    Essential,
    NonEssential,
};

struct ExpenseItem {
    ExpenseType type{};
    std::string name;
    double cost{};
    std::string category;
    bool recurring{};
};

struct SavingsAccount {
    std::string account_name;
    std::string type;
    double goal{};
    double current_value{};
};

struct PaycheckDoc {
    std::int64_t paycheck_num{};
    std::string date;
    double amount{};
    Allocation allocations;
    std::vector<ExpenseItem> expense_items;
    std::vector<SavingsAccount> savings_accounts;
};

}  // namespace clunkydb::budget

#endif  // CLUNKYDB_BUDGET_TYPES_H_
