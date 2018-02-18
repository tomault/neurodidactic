#include <neurodidactic/core/layers/Nonlinearities.hpp>

#include <neurodidactic/core/arrays/MdArray.hpp>
#include <gtest/gtest.h>
#include <sstream>
#include <string>

using namespace neurodidactic::core::arrays;
using namespace neurodidactic::core::layers;
namespace nl = neurodidactic::core::layers::nonlinearities;

namespace {
  typedef MdArray<1, float> FloatVector;

  // TODO: Move this into a header file
  template <typename Array>
  std::string arrayDataToString(const Array& a, size_t index = -1) {
    std::ostringstream msg;
    auto p = a.begin();
    
    msg << "[";
    for (size_t i = 0; i < a.size(); ++i, ++p) {
      msg << ", " << *p;
      if (i == index) {
	msg << "**";
      }
    }
    msg << " ]";
    return msg.str();
  }
  
  template <typename Array>
  ::testing::AssertionResult verifyArray(
      const typename Array::DimensionListType& trueDimensions,
      const std::vector<float>& trueData,
      const Array& array,
      float tol = 1e-06
  ) {
    auto trueLeadingDimension =
        trueDimensions.numElements() / trueDimensions[0];

    if (trueDimensions.numElements() != trueData.size()) {
      return ::testing::AssertionFailure()
	  << "trueData is inconsistent with trueDimensions -- "
	  << "trueData.size() == " << trueData.size()
	  << ", but trueDimensions.numElements() == "
	  << trueDimensions.numElements();
    }
    
    if (trueDimensions != array.dimensions()) {
      return ::testing::AssertionFailure()
	  << "Array dimensions are " << array.dimensions()
	  << ", but they should be " << trueDimensions;
    }
    
    if (trueDimensions.numElements() != array.size()) {
      return ::testing::AssertionFailure()
	<< "Array.size() is " << array.size() << " but it should be "
	  << trueDimensions.numElements();
    }
    
    if (trueLeadingDimension != array.leadingDimension()) {
      return ::testing::AssertionFailure()
	  << "Array.leadingDimension() is " << array.leadingDimension()
	  << ", but it should be " << trueLeadingDimension;
    }
    
    if (trueDimensions.numElements() != (array.end() - array.begin())) {
      return ::testing::AssertionFailure()
	  << "Difference between array.end() and array.begin is "
	  << (array.end() - array.begin()) << ", but it should be "
	  << trueDimensions.numElements();
    }

    auto data = array.data();
    for (size_t i = 0; i < trueDimensions.numElements(); ++i) {
      if (fabsf(trueData[i] - data[i]) > tol) {
	return ::testing::AssertionFailure()
	    << "Array.data() is " << arrayDataToString(array, i)
	    << ", but it should be " << arrayDataToString(trueData, i)
	    << " -- elements at index " << i << " differ ("
	    << trueData[i] << " vs. " << data[i] << ", delta = "
	    << (trueData[i] - data[i]);
      }
    }

    return ::testing::AssertionSuccess();
  }
}

TEST(NonlinearitiesTests, Identity) {
  const std::vector<float> TRUTH = { 1.0f, -2.0f, 3.0f };
  const std::vector<float> TRUE_GRADIENT = { 1.0f, 1.0f, 1.0f };
  const nl::Identity F;
  FloatVector u({3}, TRUTH.begin());

  EXPECT_TRUE(verifyArray({3}, TRUTH, F(u)));
  EXPECT_TRUE(verifyArray({3}, TRUE_GRADIENT, F.gradient(u)));
}

TEST(NonlinearitiesTests, ReLU) {
  const std::vector<float> INPUT = { 2.0f, -2.0f, 1.5f };
  const std::vector<float> TRUTH = { 2.0f,  0.0f, 1.5f };
  const std::vector<float> TRUE_GRADIENT = { 1.0f, 0.0f, 1.0f };
  const nl::ReLU F;
  FloatVector u({3}, INPUT.begin());

  EXPECT_TRUE(verifyArray({3}, TRUTH, F(u)));
  EXPECT_TRUE(verifyArray({3}, TRUE_GRADIENT, F.gradient(u)));
}

TEST(NonlinearitiesTests, Sigmoid) {
  const std::vector<float> INPUT = { 0.0f, -0.5f, 1.0f };
  const std::vector<float> TRUTH = { 0.5f,  0.377541f, 0.731059f };
  const std::vector<float> TRUE_GRADIENT = { 0.25f, 0.235004f, 0.196612f };
  const nl::Sigmoid F;
  FloatVector u({3}, INPUT.begin());

  EXPECT_TRUE(verifyArray({3}, TRUTH, F(u)));
  EXPECT_TRUE(verifyArray({3}, TRUE_GRADIENT, F.gradient(u)));
}

TEST(NonlinearitiesTests, TanH) {
  const std::vector<float> INPUT = { 0.0f, -0.5f, 1.0f };
  const std::vector<float> TRUTH = { 0.0f,  -0.462117f, 0.761594f };
  const std::vector<float> TRUE_GRADIENT = { 1.0f, 0.786448f, 0.419974f };
  const nl::TanH F;
  FloatVector u({3}, INPUT.begin());

  EXPECT_TRUE(verifyArray({3}, TRUTH, F(u)));
  EXPECT_TRUE(verifyArray({3}, TRUE_GRADIENT, F.gradient(u)));
}

