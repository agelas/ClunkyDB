#ifndef CLUNKYDB_STORAGE_COLLECTION_H_
#define CLUNKYDB_STORAGE_COLLECTION_H_

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <ios>
#include <span>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "schema/document.h"
#include "serialization/primitives.h"
#include "serialization/serializer.h"

namespace clunkydb::storage {

namespace detail {

inline constexpr std::array<std::byte, 4> kFileMagic{
    std::byte{0x43}, std::byte{0x44}, std::byte{0x42}, std::byte{0x31}};

inline auto read_all_bytes(const std::filesystem::path &path)
    -> std::vector<std::byte> {
  std::ifstream input{path, std::ios::binary};
  if (!input) {
    throw std::runtime_error("failed to open collection file for reading: " +
                             path.string());
  }

  input.seekg(0, std::ios::end);
  const auto file_size = input.tellg();
  if (file_size == std::ifstream::pos_type(-1)) {
    throw std::runtime_error("failed to determine collection file size: " +
                             path.string());
  }

  const auto file_size_as_size_t =
      static_cast<std::size_t>(static_cast<std::streamoff>(file_size));
  std::vector<std::byte> bytes(file_size_as_size_t);
  input.seekg(0, std::ios::beg);
  if (!bytes.empty()) {
    input.read(reinterpret_cast<char *>(bytes.data()),
               static_cast<std::streamsize>(bytes.size()));
  }

  if (!input) {
    throw std::runtime_error("failed to read collection file: " +
                             path.string());
  }

  return bytes;
}

inline void append_bytes(std::ofstream &output,
                         std::span<const std::byte> bytes) {
  output.write(reinterpret_cast<const char *>(bytes.data()),
               static_cast<std::streamsize>(bytes.size()));
}

inline auto starts_with_magic(std::span<const std::byte> bytes) -> bool {
  return bytes.size() >= kFileMagic.size() &&
         std::equal(kFileMagic.begin(), kFileMagic.end(), bytes.begin());
}

} // namespace detail

template <typename T, typename Schema>
  requires schema::DocumentSchema<Schema> && serialization::Serializable<T>
class Collection {
public:
  explicit Collection(std::filesystem::path file_path)
      : file_path_{std::move(file_path)} {
    ensure_file_exists();
    records_ = scan_file();
  }

  void insert(const T &document) {
    std::vector<std::byte> payload;
    serialization::Serializer<T>::serialize(document, payload);

    std::ofstream output{file_path_, std::ios::binary | std::ios::app};
    if (!output) {
      throw std::runtime_error("failed to open collection file for append: " +
                               file_path_.string());
    }

    serialization::Serializer<std::uint64_t>::serialize(
        static_cast<std::uint64_t>(payload.size()), write_buffer_);
    detail::append_bytes(output, write_buffer_);
    detail::append_bytes(output, payload);
    write_buffer_.clear();

    if (!output) {
      throw std::runtime_error("failed to append record to collection file: " +
                               file_path_.string());
    }

    records_.push_back(document);
  }

  [[nodiscard]] auto load_all() const -> std::vector<T> { return records_; }

private:
  void ensure_file_exists() const {
    if (std::filesystem::exists(file_path_)) {
      return;
    }

    std::ofstream output{file_path_, std::ios::binary};
    if (!output) {
      throw std::runtime_error("failed to create collection file: " +
                               file_path_.string());
    }
    detail::append_bytes(output, detail::kFileMagic);
  }

  [[nodiscard]] auto scan_file() const -> std::vector<T> {
    const auto bytes = detail::read_all_bytes(file_path_);
    if (!detail::starts_with_magic(bytes)) {
      throw std::runtime_error("collection file has invalid magic header: " +
                               file_path_.string());
    }

    std::vector<T> records;
    std::size_t offset = detail::kFileMagic.size();
    while (offset < bytes.size()) {
      const auto payload_size =
          serialization::Serializer<std::uint64_t>::deserialize(bytes, offset);

      if (payload_size > bytes.size() - offset) {
        throw std::runtime_error("collection file contains a truncated record: " +
                                 file_path_.string());
      }

      const auto payload_size_as_size_t =
          static_cast<std::size_t>(payload_size);
      const std::span<const std::byte> payload{bytes.data() + offset,
                                               payload_size_as_size_t};
      std::size_t payload_offset = 0;
      auto record = serialization::Serializer<T>::deserialize(payload,
                                                              payload_offset);

      if (payload_offset != payload.size()) {
        throw std::runtime_error(
            "record payload contained trailing bytes after deserialize: " +
            file_path_.string());
      }

      records.push_back(std::move(record));
      offset += payload_size_as_size_t;
    }

    return records;
  }

  std::filesystem::path file_path_;
  std::vector<T> records_;
  std::vector<std::byte> write_buffer_;
};

} // namespace clunkydb::storage

#endif // CLUNKYDB_STORAGE_COLLECTION_H_
