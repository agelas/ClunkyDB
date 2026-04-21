#ifndef CLUNKYDB_BUDGET_PAYCHECK_SCHEMA_H_
#define CLUNKYDB_BUDGET_PAYCHECK_SCHEMA_H_

#include <cstdint>
#include <string>
#include <vector>

#include "budget/types.h"
#include "schema/document.h"

namespace clunkydb::budget {

using PaycheckSchema = schema::document<
    schema::field<"PaycheckNum", std::int64_t>,
    schema::field<"Date", std::string>,
    schema::field<"PaycheckAmount", double>,
    schema::field<"Allocations", Allocation>,
    schema::field<"ExpenseItems", std::vector<ExpenseItem>>,
    schema::field<"SavingsAccounts", std::vector<SavingsAccount>>>;

static_assert(schema::DocumentSchema<PaycheckSchema>);

}  // namespace clunkydb::budget

#endif  // CLUNKYDB_BUDGET_PAYCHECK_SCHEMA_H_
