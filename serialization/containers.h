#pragma once

#include <cstddef>
#include <span>
#include <stdexcept>
#include <type_traits>
#include <vector>

#include "serialization/serializer.h"

namespace clunkydb::serialization {

template <typename T> struct is_vector : std::false_type {};

template <typename T, typename Alloc>
struct is_vector<std::vector<T, Alloc>> : std::true_type {};

template <typename T> inline constexpr bool is_vector_v = is_vector<T>::value;

template <typename T>
  requires is_vector_v<T>
struct Serializer<T> {
  static void serialize(const T &value, std::vector<std::byte> &output) {}

  static auto deserialize(std::span<const std::byte> input,
                          std::size_t &offset) -> T {}
};
} // namespace clunkydb::serialization
