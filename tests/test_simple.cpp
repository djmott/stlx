#include "gtest/gtest.h"
#include "stlx/stlx.hpp"
#include "stlx/xstring.hpp"

using namespace stlx;

TEST(BasicTest, HelloWorld) {
    EXPECT_EQ(1 + 1, 2);
}

TEST(STLXTest, PrintString) {
    print_string("Hello, World!");
    EXPECT_TRUE(true);
}

TEST(XStringSimpleTest, BasicFunctionality) {
    string s("Hello");
    EXPECT_EQ(s, "Hello");
    EXPECT_TRUE(s.ends_with("lo"));
}
