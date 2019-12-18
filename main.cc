// Copyright (c) 2019, Niels Dekker (LKEB, Leiden University Medical Center)
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
// this
//    list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its
//    contributors may be used to endorse or promote products derived from
//    this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
//
// Implements the main(argc, argv) function of the gtest-regular example.

// GoogleTest header file:
#include <gtest/gtest.h>

// Standard library header files:
#include <cstdlib>   // For EXIT_SUCCESS and EXIT_FAILURE
#include <cstring>   // For strncmp
#include <iostream>  // For cerr.

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);

  auto* const unit_test = testing::UnitTest::GetInstance();

  if (unit_test == nullptr) {
    std::cerr << "UnitTest::GetInstance() failed\n";
    return EXIT_FAILURE;
  }

  if (unit_test->Run() != 1) {
    // Run() returns 1 when there are any test failures.
    std::cerr << "UnitTest::Run() should return 1, as this program has "
                 "intended test failures!\n";
    return EXIT_FAILURE;
  }

  // The following is adapted from the example of using the UnitTest reflection
  // API (Copyright 2009 Google Inc.), from
  // https://github.com/google/googletest/blob/release-1.10.0/googletest/samples/sample9_unittest.cc#L135-L149

  const int test_suite_count{unit_test->total_test_suite_count()};

  for (int i = 0; i < test_suite_count; ++i) {
    const testing::TestSuite* const test_suite = unit_test->GetTestSuite(i);
    if (test_suite == nullptr) {
      std::cerr << "UnitTest::GetTestSuite(i) unexpectedly returned null!\n";
      return EXIT_FAILURE;
    }
    const int test_count = test_suite->total_test_count();
    for (int j = 0; j < test_count; ++j) {
      const testing::TestInfo* const test_info = test_suite->GetTestInfo(j);

      if (test_info == nullptr) {
        std::cerr << "TestSuite::GetTestInfo(j) unexpectedly returned null!\n";
        return EXIT_FAILURE;
      }

      const char* const test_name = test_info->name();
      const testing::TestResult* const test_result = test_info->result();

      if ((test_name == nullptr) || (test_result == nullptr)) {
        std::cerr
            << "TestInfo name() or result() unexpectedly returned null!\n";
        return EXIT_FAILURE;
      }

      const char prefix_of_tests_that_should_fail[] = "Irregular";
      const bool should_test_fail =
          std::strncmp(test_name, prefix_of_tests_that_should_fail,
                       sizeof(prefix_of_tests_that_should_fail) - 1) == 0;

      if (test_result->Failed() != should_test_fail) {
        std::cerr << (should_test_fail
                          ? "A test unexpectedly passed successfully: "
                          : "A test unexpectedly failed: ")
                  << test_name << '\n';
        return EXIT_FAILURE;
      }
    }
  }

  return EXIT_SUCCESS;
}
