#include <neurodidactic/core/optimizers/ForwardStateMap.hpp>

#include <neurodidactic/core/arrays/MdArray.hpp>
#include <gtest/gtest.h>

#include <algorithm>

using namespace neurodidactic::core::arrays;
using namespace neurodidactic::core::optimizers;
namespace ex = pistis::exceptions;

namespace {
  typedef MdArray<1, float> FloatVector;
  typedef MdArray<2, float> FloatMatrix;
  typedef MdArray<3, float> Float3DArray;
  typedef ForwardStateMap<float, FloatVector::AllocatorType>
          FloatForwardState;
}

TEST(ForwardStateMapTests, Create) {
  FloatForwardState forwardState;

  EXPECT_EQ(0, forwardState.numInputs());
  EXPECT_TRUE(forwardState.inputIds().empty());
  EXPECT_EQ(0, forwardState.numActivations());
  EXPECT_TRUE(forwardState.activationIds().empty());
}

TEST(ForwardStateMapTests, SetInput) {
  static const size_t V_ID = 3;
  static const size_t M_ID = 9;
  static const std::vector<size_t> IDS{ V_ID, M_ID };
  FloatForwardState forwardState;
  FloatVector v{{ 5 }, { 3.0f, -1.5f, 2.0f, 2.5f, -0.5f }};
  FloatMatrix m{{ 3, 2 }, { 0.5f -1.0f, 1.5f, 2.0f, -2.5f, 3.0f }};
  std::vector<size_t> ids;

  forwardState.setInputs(V_ID, v.ref());
  forwardState.setInputs(M_ID, m.ref());

  EXPECT_EQ(2, forwardState.numInputs());
  ids = forwardState.inputIds();
  std::sort(ids.begin(), ids.end());
  EXPECT_EQ(IDS, ids);

  FloatVector::RefType vRef = forwardState.inputs(V_ID).cast<1>();
  EXPECT_TRUE(vRef.refersTo(v));

  FloatMatrix::RefType mRef = forwardState.inputs(M_ID).cast<2>();
  EXPECT_TRUE(mRef.refersTo(m));

  FloatVector v2{ { 3 }, { -9.0f, 18.0f, 27.0f } };
  forwardState.setInputs(V_ID, v2.ref());
  EXPECT_TRUE(forwardState.inputs(V_ID).cast<1>().refersTo(v2));
}

TEST(ForwardStateMapTests, SetActivation) {
  static const size_t V_ID = 3;
  static const size_t M_ID = 9;
  static const std::vector<size_t> IDS{ V_ID, M_ID };
  FloatForwardState forwardState;
  FloatVector v{{ 5 }, { 3.0f, -1.5f, 2.0f, 2.5f, -0.5f }};
  FloatMatrix m{{ 3, 2 }, { 0.5f -1.0f, 1.5f, 2.0f, -2.5f, 3.0f }};
  std::vector<size_t> ids;

  forwardState.setActivations(V_ID, v.ref());
  forwardState.setActivations(M_ID, m.ref());

  EXPECT_EQ(2, forwardState.numActivations());
  ids = forwardState.activationIds();
  std::sort(ids.begin(), ids.end());
  EXPECT_EQ(IDS, ids);

  FloatVector::RefType vRef = forwardState.activations(V_ID).cast<1>();
  EXPECT_TRUE(vRef.refersTo(v));

  FloatMatrix::RefType mRef = forwardState.activations(M_ID).cast<2>();
  EXPECT_TRUE(mRef.refersTo(m));

  FloatVector v2{ { 3 }, { -9.0f, 18.0f, 27.0f } };
  forwardState.setActivations(V_ID, v2.ref());
  EXPECT_TRUE(forwardState.activations(V_ID).cast<1>().refersTo(v2));
}

TEST(ForwardStateMapTests, Reset) {
  static const size_t V_ID = 3;
  static const size_t M_ID = 9;
  static const std::vector<size_t> IDS{ V_ID, M_ID };
  FloatForwardState forwardState;
  FloatVector v{{ 5 }, { 3.0f, -1.5f, 2.0f, 2.5f, -0.5f }};
  FloatMatrix m{{ 3, 2 }, { 0.5f -1.0f, 1.5f, 2.0f, -2.5f, 3.0f }};
  std::vector<size_t> ids;

  forwardState.setInputs(V_ID, v.ref());
  forwardState.setActivations(M_ID, m.ref());

  EXPECT_EQ(1, forwardState.numInputs());
  EXPECT_EQ(1, forwardState.numActivations());

  forwardState.reset();
  EXPECT_EQ(0, forwardState.numInputs());
  EXPECT_EQ(0, forwardState.numActivations());
}

TEST(ForwardStateMapTests, AccessNonexistentInputs) {
  FloatForwardState forwardState;

  EXPECT_THROW(forwardState.inputs(1), ex::NoSuchItem);
}

TEST(ForwardStateMapTests, AccessNonexistentActivations) {
  FloatForwardState forwardState;

  EXPECT_THROW(forwardState.inputs(1), ex::NoSuchItem);
}

