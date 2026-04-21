#include <iostream>

#include "budget/paycheck_schema.h"

int main() {
    static_assert(clunkydb::schema::DocumentSchema<clunkydb::budget::PaycheckSchema>);
    std::cout << "ClunkyDB lives.\n";
    return 0;
}
