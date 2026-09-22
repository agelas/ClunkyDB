#pragma once

#include "query/query_fields.h"
#include "schema/field_lookup.h"

namespace clunkydb::query {
template <typename Schema, typename Fields> struct all_fields_exist_in_schema;

template <typename Schema, typename... Fields>
struct all_fields_exist_in_schema<Schema, type_list<Fields...>>
    : std::bool_constant<(schema::has_field_named_v<Schema, Fields::name> &&
                          ...)> {};

template <typename Schema, typename Query>
concept ValidQuery =
    all_fields_exist_in_schema<Schema, referenced_fields_t<Query>>::value;

} // namespace clunkydb::query
