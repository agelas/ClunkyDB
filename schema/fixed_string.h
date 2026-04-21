#ifndef CLUNKYDB_SCHEMA_FIXED_STRING_H_
#define CLUNKYDB_SCHEMA_FIXED_STRING_H_

#include <cstddef>
#include <string_view>

namespace clunkydb::schema {

template <std::size_t N>
struct fixed_string {
    char value[N]{};

    constexpr fixed_string(const char (&input)[N]) {
        for (std::size_t i = 0; i < N; ++i) {
            value[i] = input[i];
        }
    }

    [[nodiscard]] constexpr auto size() const -> std::size_t {
        return N;
    }

    [[nodiscard]] constexpr auto view() const -> std::string_view {
        return std::string_view{value, N - 1};
    }
};

template <std::size_t N>
fixed_string(const char (&)[N]) -> fixed_string<N>;

template <std::size_t N, std::size_t M>
[[nodiscard]] constexpr auto operator==(const fixed_string<N>& lhs,
                                        const fixed_string<M>& rhs) -> bool {
    if constexpr (N != M) {
        return false;
    } else {
        for (std::size_t i = 0; i < N; ++i) {
            if (lhs.value[i] != rhs.value[i]) {
                return false;
            }
        }
        return true;
    }
}

template <std::size_t N, std::size_t M>
[[nodiscard]] constexpr auto operator!=(const fixed_string<N>& lhs,
                                        const fixed_string<M>& rhs) -> bool {
    return !(lhs == rhs);
}

}  // namespace clunkydb::schema

#endif  // CLUNKYDB_SCHEMA_FIXED_STRING_H_
