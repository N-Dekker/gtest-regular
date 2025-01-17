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
// Tests the macro EXPECT_REGULAR(example_value1, example_value2), using
// GoogleTest.

#include "example_implementation/gtest-regular.h"  // For EXPECT_REGULAR

// GoogleTest header file:
#include <gtest/gtest.h>

// Standard library header files:
#include <climits>  // For INT_MAX.
#include <cmath>    // For isnan.
#include <initializer_list>
#include <string>
#include <vector>

GTEST_TEST(TestRegular, ExpectIntIsRegular) {
  const int example_value1{1};
  const int example_value2{INT_MAX};
  EXPECT_REGULAR(example_value1, example_value2);
}

GTEST_TEST(TestRegular, ExpectStdVectorIsRegular) {
  const std::vector<int> example_value1(1);
  const std::vector<int> example_value2{1, 2, 3};
  EXPECT_REGULAR(example_value1, example_value2);
}

GTEST_TEST(TestRegular, ExpectStdStringIsRegular) {
  const std::string example_value1("0123456789");
  const std::string example_value2("ABCDEFGHIJKLMNOPQRSTUVXWYZ");
  EXPECT_REGULAR(example_value1, example_value2);
}

GTEST_TEST(TestRegular, SupportExplicitConstructors) {
  class ExplicitlyConstructible {
   public:
    explicit ExplicitlyConstructible() = default;
    explicit ExplicitlyConstructible(const ExplicitlyConstructible&) = default;
    explicit ExplicitlyConstructible(ExplicitlyConstructible&&) = default;
    ExplicitlyConstructible& operator=(const ExplicitlyConstructible&) =
        default;
    ExplicitlyConstructible& operator=(ExplicitlyConstructible&&) = default;
    ~ExplicitlyConstructible() = default;

    explicit ExplicitlyConstructible(const int arg) : data_{arg} {}

    bool operator==(const ExplicitlyConstructible& arg) const {
      return data_ == arg.data_;
    }
    bool operator!=(const ExplicitlyConstructible& arg) const {
      return !(*this == arg);
    }

   private:
    int data_{0};
  };

  const ExplicitlyConstructible example_value1(1);
  const ExplicitlyConstructible example_value2(2);
  EXPECT_REGULAR(example_value1, example_value2);
}

GTEST_TEST(TestRegular, IrregularEqual) {
  struct IrregularType {
    int first;
    int second;

    // Potential bug in user code: an inconsistency between the equality
    // operator and the inequality operator.
    bool operator==(const IrregularType& arg) const {
      return first == arg.first;
    }
    bool operator!=(const IrregularType& arg) const {
      return second != arg.second;
    }
  };

  EXPECT_REGULAR((IrregularType{0, 1}), (IrregularType{1, 1}));
}

GTEST_TEST(TestRegular, IrregularUnequal) {
  struct IrregularType {
    int data;

    bool operator==(const IrregularType& arg) const { return data == arg.data; }

    // Potential bug in user code: inequality operator incorrect.
    bool operator!=(const IrregularType& arg) const { return *this == arg; }
  };

  EXPECT_REGULAR(IrregularType{1}, IrregularType{2});
}

GTEST_TEST(TestRegular, IrregularValueInitialization) {
  class IrregularType {
   public:
    IrregularType() = default;
    IrregularType(const IrregularType&) = default;
    IrregularType(IrregularType&&) = default;
    IrregularType& operator=(const IrregularType&) = default;
    IrregularType& operator=(IrregularType&&) = default;
    ~IrregularType() = default;

    explicit IrregularType(const unsigned arg) : data_{arg} {}

    bool operator==(const IrregularType& arg) const {
      return data_ == arg.data_;
    }
    bool operator!=(const IrregularType& arg) const { return !(*this == arg); }

   private:
    static unsigned GetNextValue() {
      static unsigned value{};
      return ++value;
    }
    unsigned data_ = GetNextValue();
  };

  EXPECT_REGULAR(IrregularType(0), IrregularType(UINT_MAX));
}

GTEST_TEST(TestRegular, IrregularCopyConstruction) {
  class IrregularType {
   public:
    IrregularType() = default;
    IrregularType(IrregularType&&) = default;
    IrregularType& operator=(const IrregularType&) = default;
    IrregularType& operator=(IrregularType&&) = default;
    ~IrregularType() = default;

    explicit IrregularType(const int arg) : data_{arg} {}

    IrregularType(const IrregularType&) {
      // Potential bug in user code: copy-constructor does not copy all (or any)
      // data.
    }

    bool operator==(const IrregularType& arg) const {
      return data_ == arg.data_;
    }
    bool operator!=(const IrregularType& arg) const { return !(*this == arg); }

   private:
    int data_{0};
  };

  EXPECT_REGULAR(IrregularType(1), IrregularType(2));
}

GTEST_TEST(TestRegular, IrregularMoveConstruction) {
  class IrregularType {
   public:
    IrregularType() = default;
    IrregularType(const IrregularType&) = default;
    IrregularType& operator=(const IrregularType&) = default;
    IrregularType& operator=(IrregularType&&) = default;
    ~IrregularType() = default;

    explicit IrregularType(std::initializer_list<int> arg) : data_(arg) {}

    IrregularType(IrregularType&&) noexcept {
      // Potential bug in user code: move-constructor does not move all (or any)
      // data.
    }

    bool operator==(const IrregularType& arg) const {
      return data_ == arg.data_;
    }
    bool operator!=(const IrregularType& arg) const { return !(*this == arg); }

   private:
    std::vector<int> data_;
  };

  EXPECT_REGULAR(IrregularType{1}, IrregularType({0, 1, 2}));
}

GTEST_TEST(TestRegular, IrregularIncompleteCopyAssignment) {
  class IrregularType {
   public:
    IrregularType() = default;
    IrregularType(const IrregularType&) = default;
    IrregularType(IrregularType&&) = default;
    IrregularType& operator=(IrregularType&&) = default;
    ~IrregularType() = default;

    explicit IrregularType(const int arg) : data_{arg} {}

    IrregularType& operator=(const IrregularType&) {
      // Potential bug in user code: copy-assignment does not copy all (or any)
      // data.
      return *this;
    }
    bool operator==(const IrregularType& arg) const {
      return data_ == arg.data_;
    }
    bool operator!=(const IrregularType& arg) const { return !(*this == arg); }

   private:
    int data_{0};
  };

  EXPECT_REGULAR(IrregularType(1), IrregularType(2));
}

GTEST_TEST(TestRegular, IrregularSourceModifyingAssignment) {
  class IrregularType {
   public:
    IrregularType() = default;
    IrregularType(const IrregularType&) = default;
    IrregularType(IrregularType&&) = default;
    IrregularType& operator=(IrregularType&&) = default;
    ~IrregularType() = default;

    explicit IrregularType(const int arg) : data_{arg} {}

    IrregularType& operator=(const IrregularType& arg) {
      // Potential bug in user code: copy-assignment modifies the source object.
      data_ = arg.data_;
      arg.data_ = 0;
      return *this;
    }

    bool operator==(const IrregularType& arg) const {
      return data_ == arg.data_;
    }
    bool operator!=(const IrregularType& arg) const { return !(*this == arg); }

   private:
    mutable int data_{0};
  };

  EXPECT_REGULAR(IrregularType(1), IrregularType(2));
}

GTEST_TEST(TestRegular, IrregularMoveAssignment) {
  class IrregularType {
   public:
    IrregularType() = default;
    IrregularType(const IrregularType&) = default;
    IrregularType(IrregularType&&) = default;
    IrregularType& operator=(const IrregularType&) = default;
    ~IrregularType() = default;

    explicit IrregularType(std::initializer_list<int> arg) : data_(arg) {}

    IrregularType& operator=(IrregularType&&) noexcept {
      // Potential bug in user code: move-assignment does not move all (or any)
      // data.
      return *this;
    }

    bool operator==(const IrregularType& arg) const {
      return data_ == arg.data_;
    }
    bool operator!=(const IrregularType& arg) const { return !(*this == arg); }

   private:
    std::vector<int> data_;
  };

  EXPECT_REGULAR(IrregularType{1}, IrregularType({0, 1, 2}));
}

GTEST_TEST(TestRegular, IrregularSelfAssignment) {
  class IrregularType {
   public:
    IrregularType() = default;
    IrregularType(IrregularType&&) = default;
    IrregularType& operator=(IrregularType&&) = default;
    ~IrregularType() = default;

    explicit IrregularType(std::initializer_list<int> arg)
        : data_{new std::vector<int>(arg)} {}

    IrregularType(const IrregularType& arg)
        : data_{(arg.data_ == nullptr) ? nullptr
                                       : new std::vector<int>(*arg.data_)} {}

    IrregularType& operator=(const IrregularType& arg) {
      // Potential bug in user code: when starting an assignment by resetting
      // this object, one may forget to support self-assignment correctly.
      data_.reset();

      if (arg.data_ != nullptr) {
        data_.reset(new std::vector<int>(*arg.data_));
      }
      return *this;
    }

    bool operator==(const IrregularType& arg) const {
      return (data_ == arg.data_) ||
             ((data_ != nullptr) && (arg.data_ != nullptr) &&
              (*data_ == *arg.data_));
    }

    bool operator!=(const IrregularType& arg) const { return !(*this == arg); }

   private:
    std::unique_ptr<std::vector<int>> data_;
  };

  EXPECT_REGULAR(IrregularType{1}, IrregularType({0, 1, 2}));
}

GTEST_TEST(TestRegular, IrregularSelfMoveAssignment) {
  class IrregularType {
   public:
    IrregularType() = default;
    IrregularType(const IrregularType&) = default;
    IrregularType(IrregularType&&) = default;
    IrregularType& operator=(const IrregularType&) = default;
    ~IrregularType() = default;

    explicit IrregularType(const double arg) : data_{arg} {
      // Precondition.
      assert(!std::isnan(arg));
    }

    IrregularType& operator=(IrregularType&& arg) noexcept {
      // Potential bug in user code: self-move-assignment invalidates the
      // object.
      data_ = std::numeric_limits<double>::quiet_NaN();
      data_ = arg.data_;
      return *this;
    }

    bool operator==(const IrregularType& arg) const {
      // Do not do "data_ == arg.data_", to avoid a warning like "comparing
      // floating point with == or != is unsafe [-Wfloat-equal]".
      return (data_ <= arg.data_) && (data_ >= arg.data_);
    }
    bool operator!=(const IrregularType& arg) const { return !(*this == arg); }

   private:
    double data_{0.0};
  };

  EXPECT_REGULAR(IrregularType(1.0), IrregularType(2.0));
}

GTEST_TEST(TestRegular, IrregularShallowCopyConstruction) {
  class IrregularType {
   public:
    IrregularType() = default;
    IrregularType(const IrregularType&) = default;
    IrregularType(IrregularType&&) = default;
    ~IrregularType() = default;

    explicit IrregularType(std::initializer_list<int> arg)
        : data_{std::make_shared<std::vector<int>>(arg)} {}

    // Potential bug in user code: the assignment operators do a deep copy, or a
    // deep move, while the defaulted copy-constructor does a shallow copy.

    IrregularType& operator=(const IrregularType& arg) {
      if ((data_ == nullptr) || (arg.data_ == nullptr)) {
        data_ = arg.data_;
      } else {
        *data_ = *arg.data_;
      }
      return *this;
    }

    IrregularType& operator=(IrregularType&& arg) noexcept {
      if ((data_ == nullptr) || (arg.data_ == nullptr)) {
        data_ = arg.data_;
      } else {
        *data_ = std::move(*arg.data_);
      }
      return *this;
    }

    bool operator==(const IrregularType& arg) const {
      return *data_ == *arg.data_;
    }
    bool operator!=(const IrregularType& arg) const { return !(*this == arg); }

   private:
    std::shared_ptr<std::vector<int>> data_{
        std::make_shared<std::vector<int>>()};
  };

  EXPECT_REGULAR(IrregularType{1}, IrregularType({0, 1, 2}));
}

GTEST_TEST(TestRegular, IrregularShallowCopyAssignment) {
  class IrregularType {
   public:
    IrregularType() = default;
    IrregularType(IrregularType&&) = default;
    IrregularType& operator=(const IrregularType&) = default;
    ~IrregularType() = default;

    explicit IrregularType(std::initializer_list<int> arg)
        : data_{std::make_shared<std::vector<int>>(arg)} {}

    // Potential bug in user code: copy-constructor and move-assignment do a
    // deep copy, or a deep move, while the defaulted copy-assignment and
    // move-constructor do a shallow copy.

    IrregularType(const IrregularType& arg)
        : data_{std::make_shared<std::vector<int>>(*arg.data_)} {}

    IrregularType& operator=(IrregularType&& arg) noexcept {
      *data_ = std::move(*arg.data_);
      return *this;
    }

    bool operator==(const IrregularType& arg) const {
      return *data_ == *arg.data_;
    }
    bool operator!=(const IrregularType& arg) const { return !(*this == arg); }

   private:
    std::shared_ptr<std::vector<int>> data_{
        std::make_shared<std::vector<int>>()};
  };

  EXPECT_REGULAR(IrregularType{1}, IrregularType({0, 1, 2}));
}

GTEST_TEST(TestRegular, IrregularSharedCopyAndDeepMove) {
  class IrregularType {
   public:
    IrregularType() = default;
    IrregularType(const IrregularType&) = default;
    IrregularType& operator=(const IrregularType&) = default;
    ~IrregularType() = default;

    explicit IrregularType(std::initializer_list<int> arg)
        : data_{std::make_shared<std::vector<int>>(arg)} {}

    // Irregularity in user code: The copy member functions (which are
    // defaulted) share the data (the std::vector<int>), while the move member
    // functions (implemented below here) do a "deep" move.

    IrregularType(IrregularType&& arg) noexcept
        // Honestly `noexcept` is arguable here, as `make_shared` might throw.
        : data_{std::make_shared<std::vector<int>>(std::move(*arg.data_))} {}

    IrregularType& operator=(IrregularType&& arg) noexcept {
      *data_ = std::move(*arg.data_);
      return *this;
    }

    bool operator==(const IrregularType& arg) const {
      return *data_ == *arg.data_;
    }
    bool operator!=(const IrregularType& arg) const { return !(*this == arg); }

   private:
    std::shared_ptr<std::vector<int>> data_{
        std::make_shared<std::vector<int>>()};
  };

  EXPECT_REGULAR(IrregularType{1}, IrregularType({0, 1, 2}));
}

GTEST_TEST(TestRegular, IrregularReferenceLikeClass) {
  class IrregularType {
   public:
    IrregularType() = default;
    IrregularType(const IrregularType&) = default;
    IrregularType(IrregularType&&) = default;
    IrregularType& operator=(IrregularType&&) = default;
    ~IrregularType() = default;

    explicit IrregularType(std::vector<int>& arg) : data_{&arg} {}

    IrregularType& operator=(const IrregularType& arg) {
      if ((data_ == nullptr) || (arg.data_ == nullptr)) {
        data_ = arg.data_;
      } else {
        // Potential bug in user code (or irregularity): copy-assignment does a
        // deep copy, while the copy-constructor does a shallow copy.
        *data_ = *arg.data_;
      }
      return *this;
    }

    bool operator==(const IrregularType& arg) const {
      return (data_ == arg.data_) ||
             ((data_ != nullptr) && (arg.data_ != nullptr) &&
              (*data_ == *arg.data_));
    }
    bool operator!=(const IrregularType& arg) const { return !(*this == arg); }

   private:
    std::vector<int>* data_{nullptr};
  };

  std::vector<int> vector1(1);
  std::vector<int> vector2{1, 2, 3};
  EXPECT_REGULAR(IrregularType(vector1), IrregularType(vector2));
}

GTEST_TEST(TestRegular, IrregularUniquePtrWrapper) {
  class IrregularType {
   public:
    IrregularType() = default;
    IrregularType(IrregularType&&) = default;
    IrregularType& operator=(IrregularType&&) = default;
    ~IrregularType() = default;

    explicit IrregularType(std::initializer_list<int> arg)
        : data_{new std::vector<int>(arg)} {}

    IrregularType(const IrregularType& arg)
        : data_((arg.data_ == nullptr) ? nullptr
                                       : new std::vector<int>(*arg.data_)) {}

    IrregularType& operator=(const IrregularType& arg) {
      if (arg.data_ == nullptr) {
        data_.reset();
      } else {
        if (data_ != nullptr) {
          // Potential bug in user code (or irregularity): copy-assignment
          // does not do its job properly when the target (this) was
          // previously moved-from (data_ == nullptr).
          *data_ = *arg.data_;
        }
      }
      return *this;
    }

    bool operator==(const IrregularType& arg) const {
      return (data_ == arg.data_) ||
             ((data_ != nullptr) && (arg.data_ != nullptr) &&
              (*data_ == *arg.data_));
    }
    bool operator!=(const IrregularType& arg) const { return !(*this == arg); }

   private:
    std::unique_ptr<std::vector<int>> data_{new std::vector<int>()};
  };

  EXPECT_REGULAR(IrregularType{1}, IrregularType({0, 1, 2}));
}
