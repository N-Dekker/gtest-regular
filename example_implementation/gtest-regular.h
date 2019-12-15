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
// This header file defines the macro's EXPECT_REGULAR(example_value1,
// example_value2) and ASSERT_REGULAR(example_value1, example_value2).

#ifndef GTEST_INCLUDE_GTEST_REGULAR_H_
#define GTEST_INCLUDE_GTEST_REGULAR_H_

#include <string>
#include <utility>  // For pair and move.

#include "gtest/gtest-message.h"             // For Message.
#include "gtest/gtest-printers.h"            // For PrintToString.
#include "gtest/gtest-test-part.h"           // For TestPartResult.
#include "gtest/gtest.h"                     // For AssertHelper.
#include "gtest/internal/gtest-type-util.h"  // For GetTypeName.

// TODO Move from "example_implementation_by_niels_dekker" to
// "testing::internal".
namespace example_implementation_by_niels_dekker {

// Helper function for implementing EXPECT_REGULAR.
//
// INTERNAL IMPLEMENTATION - DO NOT USE IN A USER PROGRAM.

template <typename T>
class RegularTypeChecker {
 public:
  class Example {
   public:
    Example(const T& value, const char* const expression)
        : value_(value),
          value_as_string_(::testing::PrintToString(value)),
          expression_(expression) {
      // Note: the string representation of the value (value_as_string_) is
      // generated at construction time, in order to be ahead of any possibly
      // changes of value_ during the test.
    }

    const T& GetValue() const { return value_; }
    const char* GetExpression() const { return expression_; }

    std::string ToString() const {
      std::string result(expression_);

      if (value_as_string_ != expression_) {
        result.append("\n    Which is: ").append(value_as_string_);
      }
      return result;
    }

   private:
    const T& value_;
    const std::string value_as_string_;
    const char* const expression_;
  };

  RegularTypeChecker(const T& example_value1,
                     const char* const example_expression1,
                     const T& example_value2,
                     const char* const example_expression2,
                     std::string& message)
      : examples_{Example(example_value1, example_expression1),
                  Example(example_value2, example_expression2)},
        message_(message) {}

  bool Check() const {
    return CheckEqualToSelf<0>() && CheckEqualToSelf<1>() &&
           CheckUnequal<0>() && CheckUnequal<1>() &&
           CheckValueInitialization() && CheckCopyAndMoveConstruct<0>() &&
           CheckCopyAndMoveConstruct<1>() &&
           CheckAssigningDifferentValue<0>() &&
           CheckAssigningDifferentValue<1>() &&
           CheckAssigningItsOriginalValue<0>() &&
           CheckAssigningItsOriginalValue<1>() && CheckSelfAssignment<0>() &&
           CheckSelfAssignment<1>() && CheckCopyValue<0>() &&
           CheckCopyValue<1>();
  }

 private:
  std::pair<Example, Example> examples_;  // Two different examples values of T.
  std::string& message_;

  template <unsigned example_index>
  const Example& GetExample() const {
    return std::get<example_index>(examples_);
  }

  template <unsigned example_index>
  const T& GetExampleValue() const {
    return GetExample<example_index>().GetValue();
  }

  template <unsigned example_index>
  bool CheckEqualToExample(const T& value,
                           const std::string& short_message) const {
    const Example& example = GetExample<example_index>();

    if (value != example.GetValue()) {
      using namespace ::testing;

      message_.append(short_message)
          .append("\n    Failed for: ")
          .append(example.ToString());
      return false;
    }
    return true;
  }

  bool CheckValueInitialization() const {
    const T& value_initialized1 = T();
    const T& value_initialized2 = T();
    if (value_initialized1 != value_initialized2) {
      using namespace ::testing;
      message_
          .append(
              "Value-initialization should always yield the same value"
              "\n    Value-initialized object 1: ")
          .append(PrintToString(value_initialized1))
          .append("\n    Value-initialized object 2: ")
          .append(PrintToString(value_initialized2));

      return false;
    }
    return true;
  }

  template <unsigned example_index>
  bool CheckEqualToSelf() const {
    const Example& example = GetExample<example_index>();
    const T& value = example.GetValue();
    if (value == value) {
      if (!(value != value)) {
        return true;
      }
      message_.append("Object should not compare unequal to itself!");
    } else {
      message_.append("Object should compare equal to itself!");
    }
    message_.append("\n    Value: ").append(example.ToString());
    return false;
  }

  template <unsigned example_index>
  bool CheckUnequal() const {
    const Example& left_example = GetExample<example_index>();
    const Example& right_example = GetExample<1 - example_index>();
    const T& left_operand = left_example.GetValue();
    const T& right_operand = right_example.GetValue();

    if (left_operand == right_operand) {
      message_.append("The two examples should not compare equal!");
    } else {
      if (left_operand != right_operand) {
        return true;
      }
      message_.append("The two examples should compare unequal!");
    }

    message_.append("\n    Left operand: ")
        .append(left_example.ToString())
        .append("\n    Right operand: ")
        .append(right_example.ToString());
    return false;
  }

  template <unsigned example_index>
  bool CheckCopyAndMoveConstruct() const {
    const T& example_value = GetExampleValue<example_index>();
    const T copied_value = example_value;

    if (CheckEqualToExample<example_index>(
            copied_value,
            "A copy-constructed object must have a value equal to the "
            "original.")) {
      T non_const_lvalue = example_value;
      const T moved_value = std::move(non_const_lvalue);
      return CheckEqualToExample<example_index>(
          moved_value,
          "A move-constructed object must have a value equal to the original.");
    }
    return false;
  }

  template <unsigned example_index>
  bool CheckAssigningDifferentValue() const {
    const T& initial_target_value = GetExampleValue<1 - example_index>();
    const T source = GetExampleValue<example_index>();
    T copy_assign_target = initial_target_value;
    copy_assign_target = source;
    if (CheckEqualToExample<example_index>(
            copy_assign_target,
            "A copy-assigned-to object must have a value equal to the "
            "source object.")) {
      if (CheckEqualToExample<example_index>(
              source,
              "The source object of a copy-assignment must preserve its "
              "value.")) {
        T move_assign_target(initial_target_value);

        // Note that T(value) is an rvalue, that can be moved from.
        move_assign_target = T(source);

        return CheckEqualToExample<example_index>(
            move_assign_target,
            "The value of a move-assigned-to object must be equal to the "
            "original value of the source object.");
      }
    }
    return false;
  }

  template <unsigned example_index>
  bool CheckCopyValue() const {
    const T& source = GetExample<example_index>().GetValue();

    // Workaround for GCC warning: variable set but not used
    // [-Werror=unused-but-set-variable]
    const auto DoNotUse = [](const T&) {};

    if (source != T()) {
      T copy_construct_target = source;
      DoNotUse(copy_construct_target);
      copy_construct_target = T();
      DoNotUse(copy_construct_target);

      if (source == T()) {
        message_ +=
            "Assigning a new value to a copy-constructed object should not "
            "affect the source of the copy-construction.";
        return false;
      }
      T assign_target;
      assign_target = source;
      DoNotUse(assign_target);
      assign_target = T();
      DoNotUse(assign_target);

      if (source == T()) {
        message_ +=
            "Assigning a new value to a copy-assigned-to object should not "
            "affect the source of the previous assignment.";
        return false;
      }
    }
    return true;
  }

  template <unsigned example_index>
  bool CheckSelfAssignment() const {
    const Example& example = GetExample<example_index>();

    T value(example.GetValue());
    const T& const_ref = value;
    // Note that a simple `value = value` statement might cause a compile
    // warning ("explicitly assigning value of variable of type 'T' to itself
    // [-Wself-assign-overloaded]"), which appears avoided by using `const_ref`.

    value = const_ref;
    if (CheckEqualToExample<example_index>(
            value,
            "A self-assigned object must have the same value as before.")) {
      value = std::move(value);

      if (value == value) {
        return true;
      }
      // "A self-move-assigned object must (still) be equal to itself."
      using namespace ::testing;
      message_
          .append(
              "A self-move-assigned object must (still) be equal to itself.")
          .append("\n    Failed for: ")
          .append(example.ToString());
    }
    return false;
  }

  template <unsigned example_index>
  bool CheckAssigningItsOriginalValue() const {
    const T& example_value = GetExampleValue<example_index>();
    T value(example_value);
    value = example_value;
    if (CheckEqualToExample<example_index>(
            value,
            "The value of an object must be equal to its original value, when "
            "it is copy-assigned the same value.")) {
      T same_value(example_value);
      value = std::move(same_value);

      return CheckEqualToExample<example_index>(
          value,
          "The value of an object must be equal to its original value, when it "
          "is move-assigned the same value.");
    }
    return false;
  }

};  // namespace example_implementation_by_niels_dekker

template <bool is_failure_fatal, typename T>
void CheckRegularType(const char* const file, int line, const T& example_value1,
                      const char* const example_expression1,
                      const T& example_value2,
                      const char* const example_expression2) {
  std::string message;
  const RegularTypeChecker<T> checker(example_value1, example_expression1,
                                      example_value2, example_expression2,
                                      message);
  if (!checker.Check()) {
    using namespace ::testing;
    constexpr TestPartResult::Type result_type{
        is_failure_fatal ? TestPartResult::kFatalFailure
                         : TestPartResult::kNonFatalFailure};
    // Assign Message() to enable streaming; see AssertHelper::operator=.
    ::testing::internal::AssertHelper(
        result_type, file, line,
        ("Type expected to be regular: '" +
         testing::internal::GetTypeName<T>() + "'\n  " + message)
            .c_str()) = Message();
  }
}

#define EXPECT_REGULAR(example_value1, example_value2)                     \
  ::example_implementation_by_niels_dekker::CheckRegularType<false>(       \
      __FILE__, __LINE__, example_value1, #example_value1, example_value2, \
      #example_value2)

#define ASSERT_REGULAR(example_value1, example_value2)                     \
  ::example_implementation_by_niels_dekker::CheckRegularType<true>(        \
      __FILE__, __LINE__, example_value1, #example_value1, example_value2, \
      #example_value2)

}  // namespace example_implementation_by_niels_dekker

#endif  // GTEST_INCLUDE_GTEST_REGULAR_H_
