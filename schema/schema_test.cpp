#include <cassert>
#include <concepts>
#include <cstdint>
#include <string>
#include <tuple>
#include <type_traits>
#include <vector>

#include "budget/paycheck_schema.h"
#include "schema/document.h"
#include "schema/fixed_string.h"

namespace schema = clunkydb::schema;
namespace budget = clunkydb::budget;

namespace {

using TestField = schema::field<"MeaningOfLife", std::int64_t>;
using TestDocument = schema::document<
    schema::field<"Alpha", int>,
    schema::field<"Beta", double>>;

struct MissingValueType {
    static constexpr auto name = schema::fixed_string{"Broken"};
};

static_assert(schema::fixed_string{"Alpha"} == schema::fixed_string{"Alpha"});
static_assert(schema::fixed_string{"Alpha"} != schema::fixed_string{"Beta"});
static_assert(TestField::name == schema::fixed_string{"MeaningOfLife"});
static_assert(std::same_as<TestField::value_type, std::int64_t>);
static_assert(schema::FieldSpec<TestField>);
static_assert(!schema::FieldSpec<MissingValueType>);
static_assert(schema::DocumentSchema<TestDocument>);
static_assert(schema::DocumentSchema<budget::PaycheckSchema>);
static_assert(budget::PaycheckSchema::field_count == 6);
static_assert(std::same_as<
              std::tuple_element_t<0, budget::PaycheckSchema::fields>::value_type,
              std::int64_t>);
static_assert(std::tuple_element_t<1, budget::PaycheckSchema::fields>::name ==
              schema::fixed_string{"Date"});
static_assert(std::same_as<
              std::tuple_element_t<4, budget::PaycheckSchema::fields>::value_type,
              std::vector<budget::ExpenseItem>>);

}  // namespace

int main() {
    constexpr auto paycheck_num_name =
        std::tuple_element_t<0, budget::PaycheckSchema::fields>::name;
    constexpr auto date_name =
        std::tuple_element_t<1, budget::PaycheckSchema::fields>::name;

    assert(paycheck_num_name.view() == "PaycheckNum");
    assert(date_name.view() == "Date");

    return 0;
}
