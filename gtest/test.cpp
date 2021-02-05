/**
*** Author: R-CO
*** E-mail: daniel1820kobe@gmail.com
*** Date: 2021-02-05
**/
// C++ standard library
#include <cstdlib>

// GoogleTest framework
#include <gtest/gtest.h>

int main(int argc, char *argv[]) {
  testing::InitGoogleTest(&argc, argv);
  if (RUN_ALL_TESTS() != 0) {
    // some test-cases failed
  }

  return EXIT_SUCCESS;
}
