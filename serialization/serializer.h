#ifndef CLUNKYDB_SERIALIZATION_SERIALIZER_H_
#define CLUNKYDB_SERIALIZATION_SERIALIZER_H_

#include <cstddef>
#include <span>
#include <vector>

namespace clunkydb::serialization {

template <typename T> struct Serializer;

template <typename T>
concept Serializable =
    requires(const T &value, std::vector<std::byte> &output,
             std::span<const std::byte> input, std::size_t &offset) {
      { Serializer<T>::serialize(value, output) } -> std::same_as<void>;
      { Serializer<T>::deserialize(input, offset) } -> std::same_as<T>;
    };
} // namespace clunkydb::serialization

#endif // CLUNKYDB_SERIALIZATION_SERIALIZER_H_
