#include <gtest/gtest.h>

// Main test file - this will be linked with gtest_main
// Additional test files can be added here

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
