#include <gtest/gtest.h>
#include <stlx/xstring.hpp>
#include <locale>
#include <vector>

using namespace stlx;

class XStringTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Set up test data
        test_string = "Hello World";
        empty_string = "";
        whitespace_string = "   \t\n  ";
        mixed_whitespace = "  Hello World  \t\n";
    }

    string test_string;
    string empty_string;
    string whitespace_string;
    string mixed_whitespace;
};

// Test basic construction and inheritance
TEST_F(XStringTest, Construction) {
    string s1;
    EXPECT_TRUE(s1.empty());
    
    string s2("Hello");
    EXPECT_EQ(s2, "Hello");
    
    string s3(5, 'A');
    EXPECT_EQ(s3, "AAAAA");
    
    string s4(s2);
    EXPECT_EQ(s4, "Hello");
    
    string s5(std::move(s2));
    EXPECT_EQ(s5, "Hello");
    EXPECT_TRUE(s2.empty());
}

// Test ends_with functionality
TEST_F(XStringTest, EndsWith) {
    string s("Hello World");
    
    EXPECT_TRUE(s.ends_with("World"));
    EXPECT_TRUE(s.ends_with(""));
    EXPECT_FALSE(s.ends_with("Hello"));
    EXPECT_FALSE(s.ends_with("world")); // case sensitive
    EXPECT_FALSE(s.ends_with("Hello World Extra"));
    
    string empty;
    EXPECT_TRUE(empty.ends_with(""));
    EXPECT_FALSE(empty.ends_with("test"));
}

// Test format functionality
TEST_F(XStringTest, Format) {
    // Test empty format
    string result = string::format();
    EXPECT_EQ(result, "");
    
    // Test single argument
    result = string::format("Hello");
    EXPECT_EQ(result, "Hello");
    
    // Test multiple arguments
    result = string::format("Hello", " ", "World");
    EXPECT_EQ(result, "Hello World");
    
    // Test with numbers
    result = string::format("Number: ", 42);
    EXPECT_EQ(result, "Number: 42");
    
    // Test with mixed types
    result = string::format("Value: ", 3.14, " (", 42, ")");
    EXPECT_EQ(result, "Value: 3.14 (42)");
}

// Test reverse functionality
TEST_F(XStringTest, Reverse) {
    string s("Hello");
    s.reverse();
    EXPECT_EQ(s, "olleH");
    
    string empty;
    empty.reverse();
    EXPECT_TRUE(empty.empty());
    
    string single("A");
    single.reverse();
    EXPECT_EQ(single, "A");
    
    string palindrome("racecar");
    palindrome.reverse();
    EXPECT_EQ(palindrome, "racecar");
}

// Test case conversion
TEST_F(XStringTest, CaseConversion) {
    std::locale loc;
    
    string s("Hello World");
    s.to_lower(loc);
    EXPECT_EQ(s, "hello world");
    
    s.to_upper(loc);
    EXPECT_EQ(s, "HELLO WORLD");
    
    // Test with special characters
    string special("Hello123!@#");
    special.to_lower(loc);
    EXPECT_EQ(special, "hello123!@#");
}

// Test trimming functionality
TEST_F(XStringTest, Trim) {
    // Test ltrim
    string s1("   Hello");
    s1.ltrim();
    EXPECT_EQ(s1, "Hello");
    
    string s2("Hello   ");
    s2.ltrim();
    EXPECT_EQ(s2, "Hello   "); // Should not change
    
    string s3("   ");
    s3.ltrim();
    EXPECT_TRUE(s3.empty());
    
    // Test rtrim
    string s4("Hello   ");
    s4.rtrim();
    EXPECT_EQ(s4, "Hello");
    
    string s5("   Hello");
    s5.rtrim();
    EXPECT_EQ(s5, "   Hello"); // Should not change
    
    string s6("   ");
    s6.rtrim();
    EXPECT_TRUE(s6.empty());
    
    // Test trim (both)
    string s7("   Hello World   ");
    s7.trim();
    EXPECT_EQ(s7, "Hello World");
    
    string s8("   ");
    s8.trim();
    EXPECT_TRUE(s8.empty());
    
    string s9("Hello");
    s9.trim();
    EXPECT_EQ(s9, "Hello");
}

// Test replace functionality
TEST_F(XStringTest, Replace) {
    // Test character replacement
    string s1("Hello World");
    s1.replace({'l', 'o'}, 'X');
    EXPECT_EQ(s1, "HeXXX WXrXd");
    
    string s2("abcabc");
    s2.replace({'a', 'b'}, 'X');
    EXPECT_EQ(s2, "XXcXXc");
    
    // Test substring replacement
    string s3("Hello World Hello");
    s3.replace("Hello", 'X');
    EXPECT_EQ(s3, "X World X");
    
    string s4("ababab");
    s4.replace("ab", 'X');
    EXPECT_EQ(s4, "XXX");
}

// Test remove functionality
TEST_F(XStringTest, Remove) {
    string s1("Hello World");
    s1.remove({'l', 'o'});
    EXPECT_EQ(s1, "He Wrd");
    
    string s2("abcabc");
    s2.remove({'a', 'b'});
    EXPECT_EQ(s2, "cc");
    
    string s3("Hello");
    s3.remove({'x', 'y', 'z'}); // Characters not in string
    EXPECT_EQ(s3, "Hello");
}

// Test find functionality
TEST_F(XStringTest, Find) {
    string s("Hello World");
    
    // Test find_first_of with initializer list
    size_t pos = s.find_first_of({'l', 'o'});
    EXPECT_EQ(pos, 2); // First 'l' at position 2
    
    pos = s.find_first_of({'x', 'y'});
    EXPECT_EQ(pos, string::npos);
    
    pos = s.find_first_of({'l', 'o'}, 5);
    EXPECT_EQ(pos, 7); // 'o' at position 7
    
    // Test find_first_of with function
    pos = s.find_first_of([](char c) { return c == 'l' || c == 'o'; });
    EXPECT_EQ(pos, 2);
    
    pos = s.find_first_of([](char c) { return c == 'x'; });
    EXPECT_EQ(pos, string::npos);
    
    // Test find_last_of
    pos = s.find_last_of({'l', 'o'});
    EXPECT_EQ(pos, 9); // Last 'l' at position 9
    
    pos = s.find_last_of({'x', 'y'});
    EXPECT_EQ(pos, string::npos);
}

// Test split functionality
TEST_F(XStringTest, Split) {
    // Test split with character delimiters
    string s1("Hello,World,Test");
    auto result1 = s1.split({','});
    EXPECT_EQ(result1.size(), 3);
    EXPECT_EQ(result1[0], "Hello");
    EXPECT_EQ(result1[1], "World");
    EXPECT_EQ(result1[2], "Test");
    
    // Test split with multiple delimiters
    string s2("Hello World,Test");
    auto result2 = s2.split({' ', ','});
    EXPECT_EQ(result2.size(), 3);
    EXPECT_EQ(result2[0], "Hello");
    EXPECT_EQ(result2[1], "World");
    EXPECT_EQ(result2[2], "Test");
    
    // Test split with trimEmpty
    string s3("Hello,,World");
    auto result3 = s3.split({','}, true);
    EXPECT_EQ(result3.size(), 2);
    EXPECT_EQ(result3[0], "Hello");
    EXPECT_EQ(result3[1], "World");
    
    auto result4 = s3.split({','}, false);
    EXPECT_EQ(result4.size(), 3);
    EXPECT_EQ(result4[0], "Hello");
    EXPECT_EQ(result4[1], "");
    EXPECT_EQ(result4[2], "World");
    
    // Test split with string delimiter
    string s4("Hello::World::Test");
    auto result5 = s4.split("::");
    EXPECT_EQ(result5.size(), 3);
    EXPECT_EQ(result5[0], "Hello");
    EXPECT_EQ(result5[1], "World");
    EXPECT_EQ(result5[2], "Test");
    
    // Test split with function
    string s5("Hello World Test");
    auto result6 = s5.split(std::function<bool(char)>([](char c) { return c == ' '; }));
    EXPECT_EQ(result6.size(), 3);
    EXPECT_EQ(result6[0], "Hello");
    EXPECT_EQ(result6[1], "World");
    EXPECT_EQ(result6[2], "Test");
}

// Test edge cases
TEST_F(XStringTest, EdgeCases) {
    // Test empty string operations
    string empty;
    EXPECT_TRUE(empty.empty());
    EXPECT_TRUE(empty.ends_with(""));
    EXPECT_FALSE(empty.ends_with("test"));
    
    empty.reverse();
    EXPECT_TRUE(empty.empty());
    
    empty.trim();
    EXPECT_TRUE(empty.empty());
    
    auto split_result = empty.split({','});
    EXPECT_EQ(split_result.size(), 1);
    EXPECT_TRUE(split_result[0].empty());
    
    // Test single character
    string single("A");
    EXPECT_TRUE(single.ends_with("A"));
    EXPECT_FALSE(single.ends_with("B"));
    
    single.reverse();
    EXPECT_EQ(single, "A");
    
    // Test all whitespace
    string all_ws("   \t\n   ");
    all_ws.trim();
    EXPECT_TRUE(all_ws.empty());
}

// Test wide string functionality
TEST_F(XStringTest, WideString) {
    wstring ws(L"Hello World");
    EXPECT_EQ(ws.size(), 11);
    
    ws.reverse();
    EXPECT_EQ(ws, L"dlroW olleH");
    
    std::locale loc;
    ws.to_lower(loc);
    EXPECT_EQ(ws, L"dlrow olleh");
}

// Test type aliases
TEST_F(XStringTest, TypeAliases) {
    string s("Hello");
    wstring ws(L"World");
    
    EXPECT_EQ(s, "Hello");
    EXPECT_EQ(ws, L"World");
    
    // Test tstring (should be char on most systems)
    tstring ts("Test");
    EXPECT_EQ(ts, "Test");
}

// Test format with different types
TEST_F(XStringTest, FormatTypes) {
    // Test integer types
    string result1 = string::format("Int32: ", int32_t(42));
    EXPECT_EQ(result1, "Int32: 42");
    
    string result2 = string::format("UInt32: ", uint32_t(42));
    EXPECT_EQ(result2, "UInt32: 42");
    
    string result3 = string::format("Int64: ", int64_t(42));
    EXPECT_EQ(result3, "Int64: 42");
    
    string result4 = string::format("UInt64: ", uint64_t(42));
    EXPECT_EQ(result4, "UInt64: 42");
    
    // Test pointer types
    void* ptr = reinterpret_cast<void*>(0x12345678);
    string result5 = string::format("Pointer: ", ptr);
    EXPECT_TRUE(result5.find("Pointer: ") == 0);
}

// Test complex scenarios
TEST_F(XStringTest, ComplexScenarios) {
    // Test chaining operations
    string s("  Hello World  ");
    s.trim().to_lower(std::locale()).reverse();
    EXPECT_EQ(s, "dlrow olleh");
    
    // Test multiple replacements
    string s2("Hello World Hello");
    s2.replace("Hello", 'X').replace({'l', 'o'}, 'Y');
    EXPECT_EQ(s2, "X WYrYd X");
    
    // Test split and join simulation
    string s3("a,b,c,d,e");
    auto parts = s3.split({','});
    string reconstructed = string::format();
    for (size_t i = 0; i < parts.size(); ++i) {
        if (i > 0) reconstructed += ",";
        reconstructed += parts[i];
    }
    EXPECT_EQ(reconstructed, "a,b,c,d,e");
}

// Test performance characteristics
TEST_F(XStringTest, Performance) {
    // Test large string operations
    string large(1000, 'A');
    large.replace({'A'}, 'B');
    EXPECT_EQ(large, string(1000, 'B'));
    
    // Test many small operations
    string s("Hello");
    for (int i = 0; i < 100; ++i) {
        s.reverse();
    }
    EXPECT_EQ(s, "Hello"); // Should be back to original after even number of reverses
}
