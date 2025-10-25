#include <gtest/gtest.h>
#include <stlx/stlx.hpp>
#include <string>
#include <vector>

// Test fixture for STLX utilities
class StlxTest : public ::testing::Test {
protected:
  void SetUp() override {
    test_string = "  Hello, World!  ";
    empty_string = "";
    whitespace_string = "   \t\n\r   ";
  }

  std::string test_string;
  std::string empty_string;
  std::string whitespace_string;
};

// String utility tests
TEST_F(StlxTest, StringViewConversion) {
  EXPECT_EQ(stlx::to_string_view(test_string), "  Hello, World!  ");
  EXPECT_EQ(stlx::to_string_view("literal"), "literal");
}

TEST_F(StlxTest, StringStartsWith) {
  EXPECT_TRUE(stlx::starts_with(test_string, "  Hello"));
  EXPECT_TRUE(stlx::starts_with("Hello World", "Hello"));
  EXPECT_FALSE(stlx::starts_with(test_string, "World"));
  EXPECT_FALSE(stlx::starts_with(empty_string, "anything"));
}

TEST_F(StlxTest, StringEndsWith) {
  EXPECT_TRUE(stlx::ends_with(test_string, "World!  "));
  EXPECT_TRUE(stlx::ends_with("Hello World", "World"));
  EXPECT_FALSE(stlx::ends_with(test_string, "Hello"));
  EXPECT_FALSE(stlx::ends_with(empty_string, "anything"));
}

TEST_F(StlxTest, StringContains) {
  EXPECT_TRUE(stlx::contains(test_string, "Hello"));
  EXPECT_TRUE(stlx::contains(test_string, "World"));
  EXPECT_FALSE(stlx::contains(test_string, "Goodbye"));
  EXPECT_FALSE(stlx::contains(empty_string, "anything"));
}

TEST_F(StlxTest, StringTrimLeft) {
  EXPECT_EQ(stlx::trim_left(test_string), "Hello, World!  ");
  EXPECT_EQ(stlx::trim_left(whitespace_string), "");
  EXPECT_EQ(stlx::trim_left("no_spaces"), "no_spaces");
}

TEST_F(StlxTest, StringTrimRight) {
  EXPECT_EQ(stlx::trim_right(test_string), "  Hello, World!");
  EXPECT_EQ(stlx::trim_right(whitespace_string), "");
  EXPECT_EQ(stlx::trim_right("no_spaces"), "no_spaces");
}

TEST_F(StlxTest, StringTrim) {
  EXPECT_EQ(stlx::trim(test_string), "Hello, World!");
  EXPECT_EQ(stlx::trim(whitespace_string), "");
  EXPECT_EQ(stlx::trim("no_spaces"), "no_spaces");
  EXPECT_EQ(stlx::trim("  single  "), "single");
}

// Numeric utility tests
TEST_F(StlxTest, Clamp) {
  EXPECT_EQ(stlx::clamp(5, 0, 10), 5);
  EXPECT_EQ(stlx::clamp(-5, 0, 10), 0);
  EXPECT_EQ(stlx::clamp(15, 0, 10), 10);
  EXPECT_EQ(stlx::clamp(5.5, 0.0, 10.0), 5.5);
  EXPECT_EQ(stlx::clamp(-1.5, 0.0, 10.0), 0.0);
}

TEST_F(StlxTest, InRange) {
  EXPECT_TRUE(stlx::in_range(5, 0, 10));
  EXPECT_TRUE(stlx::in_range(0, 0, 10));
  EXPECT_TRUE(stlx::in_range(10, 0, 10));
  EXPECT_FALSE(stlx::in_range(-1, 0, 10));
  EXPECT_FALSE(stlx::in_range(11, 0, 10));
}

// Format utility tests
TEST_F(StlxTest, FormatString) {
  std::string result = stlx::format_string("Hello {}!", "World");
  EXPECT_EQ(result, "Hello World!");

  result = stlx::format_string("Value: {}, Count: {}", 42, 3);
  EXPECT_EQ(result, "Value: 42, Count: 3");
}

// Compile-time hash tests
TEST_F(StlxTest, HashString) {
  constexpr auto hash1 = stlx::hash_string("test");
  constexpr auto hash2 = stlx::hash_string("test");
  constexpr auto hash3 = stlx::hash_string("different");

  EXPECT_EQ(hash1, hash2);
  EXPECT_NE(hash1, hash3);
  EXPECT_NE(hash2, hash3);
}

// Scope exit tests
TEST_F(StlxTest, ScopeExit) {
  bool cleanup_called = false;

  {
    auto cleanup =
        stlx::make_scope_exit([&cleanup_called]() { cleanup_called = true; });

    EXPECT_FALSE(cleanup_called);
  }

  EXPECT_TRUE(cleanup_called);
}

// Edge cases and error conditions
TEST_F(StlxTest, EdgeCases) {
  // Empty string operations
  EXPECT_EQ(stlx::trim(""), "");
  EXPECT_EQ(stlx::trim_left(""), "");
  EXPECT_EQ(stlx::trim_right(""), "");

  // Single character
  EXPECT_EQ(stlx::trim("a"), "a");
  EXPECT_EQ(stlx::trim(" a"), "a");
  EXPECT_EQ(stlx::trim("a "), "a");
  EXPECT_EQ(stlx::trim(" a "), "a");

  // All whitespace
  EXPECT_EQ(stlx::trim("   "), "");
  EXPECT_EQ(stlx::trim("\t\n\r"), "");
}

// Performance tests (basic sanity checks)
TEST_F(StlxTest, PerformanceSanity) {
  const std::string large_string(1000, 'a');

  // These should not crash or take excessive time
  EXPECT_EQ(stlx::trim(large_string), large_string);
  EXPECT_TRUE(stlx::contains(large_string, "a"));
  EXPECT_FALSE(stlx::contains(large_string, "b"));
}
