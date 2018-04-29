#include <neurodidactic/core/arrays/AnyMdArrayRef.hpp>
#include <neurodidactic/core/arrays/MdArray.hpp>
#include <gtest/gtest.h>
#include <vector>

using namespace neurodidactic::core::arrays;
namespace ex = pistis::exceptions;

namespace {
  typedef MdArray<1, float> FloatVector;
}

TEST(AnyMdArrayRefTests, Create) {
  const std::vector<float> DATA{ -0.5f, 1.0f, 0.5f };
  const FloatVector v({3}, DATA.begin());
  const FloatVector::DimensionListType dimensions = v.dimensions();
  const float* const startOfData = v.data();
  const float* const endOfData = v.end();
  FloatVector::RefType ref = v.ref();
  AnyMdArrayRef<float, FloatVector::AllocatorType> any(ref);

  FloatVector::RefType recovered = any.cast<1>();
  EXPECT_TRUE(recovered.refersTo(v));
  EXPECT_EQ(dimensions, recovered.dimensions());
  EXPECT_EQ(startOfData, recovered.data());
  EXPECT_EQ(endOfData, recovered.end());
}

TEST(AnyMdArrayRefTests, GetWithWrongOrder) {
  const std::vector<float> DATA{ -0.5f, 1.0f, 0.5f };
  const FloatVector v({3}, DATA.begin());
  AnyMdArrayRef<float, FloatVector::AllocatorType> any(v.ref());

  EXPECT_THROW(any.cast<2>(), ex::IllegalValueError);
}

TEST(AnyMdArrayRefTests, EqualityAndInequality) {
  const std::vector<float> DATA1{ -0.5f, 1.0f, 0.5f };
  const std::vector<float> DATA2{  0.5f, 0.1f, 0.2f };
  const FloatVector u({3}, DATA1.begin());
  const FloatVector v({3}, DATA2.begin());
  AnyMdArrayRef<float, FloatVector::AllocatorType> any(u.ref());
  AnyMdArrayRef<float, FloatVector::AllocatorType> anySame(u.ref());
  AnyMdArrayRef<float, FloatVector::AllocatorType> anyDifferent(v.ref());

  EXPECT_TRUE(any == anySame);
  EXPECT_FALSE(any == anyDifferent);
  EXPECT_FALSE(any != anySame);
  EXPECT_TRUE(any != anyDifferent);
}

