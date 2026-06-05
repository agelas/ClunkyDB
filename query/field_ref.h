#pragma once

#include "schema/fixed_string.h"

namespace clunkydb::query {
template <schema::fixed_string Name> struct field_ref {
  static constexpr auto name = Name;
};

template <schema::fixed_string Name> inline constexpr field_ref<Name> field{};
} // namespace clunkydb::query
