#pragma once

#include <cstddef>
#include <span>
#include <stdexcept>
#include <type_traits>
#include <vector>

#include "serialization/primitives.h"
#include "serialization/serializer.h"

namespace clunkydb::serialization {

template <typename T> struct is_vector : std::false_type {};

template <typename T, typename Alloc>
struct is_vector<std::vector<T, Alloc>> : std::true_type {};

template <typename T> inline constexpr bool is_vector_v = is_vector<T>::value;

template <typename T>
  requires is_vector_v<T>
struct Serializer<T> {
  static void serialize(const T &value, std::vector<std::byte> &output) {
    Serializer<std::size_t>::serialize(value.size(), output);

    for (const auto &element : value) {
      Serializer<std::remove_cvref_t<decltype(element)>>::serialize(element,
                                                                    output);
    }
  }

  static auto deserialize(std::span<const std::byte> input,
                          std::size_t &offset) -> T {
    auto size = Serializer<std::size_t>::deserialize(input, offset);

    T result;
    result.reserve(size);

    for (std::size_t i = 0; i < size; ++i) {
      auto element =
          Serializer<std::remove_cvref_t<typename T::value_type>>::deserialize(
              input, offset);
      result.push_back(element);
    }

    return result;
  }
};
} // namespace clunkydb::serialization
