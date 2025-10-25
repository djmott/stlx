#pragma once

#include <concepts>
#include <format>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>
#include <type_traits>

// C++23 std::print support (if available)
#ifdef __cpp_lib_print
#include <print>
#endif

namespace stlx {

// C++23 concepts for type checking
template <typename T>
concept StringLike = std::convertible_to<T, std::string_view>;

template <typename T>
concept Numeric = std::integral<T> || std::floating_point<T>;

// Constexpr string utilities
template <StringLike T>
constexpr std::string_view to_string_view(const T &str) noexcept {
  return std::string_view(str);
}

template <StringLike T>
constexpr bool starts_with(const T &str, std::string_view prefix) noexcept {
  return to_string_view(str).starts_with(prefix);
}

template <StringLike T>
constexpr bool ends_with(const T &str, std::string_view suffix) noexcept {
  return to_string_view(str).ends_with(suffix);
}

template <StringLike T>
constexpr bool contains(const T &str, std::string_view substr) noexcept {
  return to_string_view(str).contains(substr);
}

// Constexpr string manipulation
template <StringLike T> constexpr std::string trim_left(const T &str) {
  std::string_view sv = to_string_view(str);
  auto start = sv.find_first_not_of(" \t\n\r\f\v");
  return start == std::string_view::npos ? std::string{}
                                         : std::string{sv.substr(start)};
}

template <StringLike T> constexpr std::string trim_right(const T &str) {
  std::string_view sv = to_string_view(str);
  auto end = sv.find_last_not_of(" \t\n\r\f\v");
  return end == std::string_view::npos ? std::string{}
                                       : std::string{sv.substr(0, end + 1)};
}

template <StringLike T> constexpr std::string trim(const T &str) {
  return trim_left(trim_right(str));
}

// Numeric utilities
template <Numeric T> constexpr T clamp(T value, T min_val, T max_val) noexcept {
  return value < min_val ? min_val : (value > max_val ? max_val : value);
}

template <Numeric T>
constexpr bool in_range(T value, T min_val, T max_val) noexcept {
  return value >= min_val && value <= max_val;
}

// Type-safe enum utilities
template <typename Enum>
concept ScopedEnum = std::is_enum_v<Enum> &&
                     !std::is_convertible_v<Enum, std::underlying_type_t<Enum>>;

template <ScopedEnum Enum> constexpr auto to_underlying(Enum e) noexcept {
  return static_cast<std::underlying_type_t<Enum>>(e);
}

// Format utilities - simplified version for compatibility
template <StringLike T> std::string format_string(const T &fmt) {
  return std::string(fmt);
}

template <StringLike T, typename Arg, typename... Args>
std::string format_string(const T &fmt, Arg &&arg, Args &&...args) {
  std::ostringstream oss;
  std::string_view fmt_view(fmt);

  // Simple placeholder replacement for {} patterns
  std::string result(fmt_view);
  size_t pos = 0;
  size_t arg_index = 0;

  while ((pos = result.find("{}", pos)) != std::string::npos) {
    if (arg_index == 0) {
      std::ostringstream arg_oss;
      arg_oss << arg;
      result.replace(pos, 2, arg_oss.str());
      pos += arg_oss.str().length();
    } else {
      break; // Only replace first {} for now
    }
    arg_index++;
  }

  // Recursively handle remaining arguments
  if constexpr (sizeof...(args) > 0) {
    return format_string(result, std::forward<Args>(args)...);
  }

  return result;
}

// Print utilities using C++23 std::print (if available)
template <StringLike T, typename... Args>
void print_string(const T &fmt, Args &&...args) {
#ifdef __cpp_lib_print
  if constexpr (requires {
                  std::print(to_string_view(fmt), std::forward<Args>(args)...);
                }) {
    std::print(to_string_view(fmt), std::forward<Args>(args)...);
  } else {
    std::cout << format_string(fmt, std::forward<Args>(args)...);
  }
#else
  // Fallback for compilers without std::print
  std::cout << format_string(fmt, std::forward<Args>(args)...);
#endif
}

template <StringLike T, typename... Args>
void print_line(const T &fmt, Args &&...args) {
  print_string(fmt, std::forward<Args>(args)...);
  std::cout << '\n';
}

// RAII utilities
template <typename F> class scope_exit {
  F f;

public:
  explicit scope_exit(F &&func) : f(std::forward<F>(func)) {}
  ~scope_exit() { f(); }

  scope_exit(const scope_exit &) = delete;
  scope_exit &operator=(const scope_exit &) = delete;
  scope_exit(scope_exit &&) = default;
  scope_exit &operator=(scope_exit &&) = default;
};

template <typename F> scope_exit<F> make_scope_exit(F &&f) {
  return scope_exit<F>(std::forward<F>(f));
}

// Compile-time string hashing
constexpr std::size_t hash_string(std::string_view str) noexcept {
  std::size_t hash = 5381;
  for (char c : str) {
    hash = ((hash << 5) + hash) + static_cast<std::size_t>(c);
  }
  return hash;
}

} // namespace stlx
