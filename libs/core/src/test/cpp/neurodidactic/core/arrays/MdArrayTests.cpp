#include <neurodidactic/core/arrays/MdArray.hpp>
#include <pistis/testing/Allocator.hpp>
#include <gtest/gtest.h>

#include <sstream>
#include <string>

using namespace neurodidactic::core::arrays;
namespace pt = pistis::testing;

namespace {
  typedef pt::Allocator<float> TestAllocator;
  typedef pt::Allocator<double> DoubleAllocator;
  typedef MdArray<1, float, TestAllocator> TestFloatVector;
  typedef MdArray<2, float, TestAllocator> TestFloatMatrix;
  typedef MdArray<3, float, TestAllocator> TestFloat3DArray;

  typedef MdArray<1, double, DoubleAllocator> TestDoubleVector;
  typedef MdArray<2, double, DoubleAllocator> TestDoubleMatrix;
  typedef MdArray<3, double, DoubleAllocator> TestDouble3DArray;

  typedef MdArray<1, float> FloatVector;
  typedef MdArray<2, float> FloatMatrix;
  typedef MdArray<3, float> Float3DArray;

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
      const Array& array
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
      if (fabsf(trueData[i] - data[i]) > 1e-06) {
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

TEST(MdArrayTests, Create3DArrayUninitialized) {
  TestAllocator allocator("TEST_1");
  TestFloat3DArray::DimensionListType dimensions({ 3, 2, 4 });
  TestFloat3DArray a({ 3, 2, 4 }, allocator);

  EXPECT_EQ(allocator.name(), a.allocator().name());
  EXPECT_EQ(dimensions, a.dimensions());
  EXPECT_EQ(3 * 2 * 4, a.size());
  EXPECT_EQ(2 * 4, a.leadingDimension());
  EXPECT_EQ(3 * 2 * 4, a.end() - a.begin());
  EXPECT_EQ(a.begin(), a.data());
}

TEST(MdArrayTests, Initialize3DArrayWithConstant) {
  const std::vector<float> TRUE_DATA{ 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
                                      1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
                                      1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
                                      1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f };
  const TestFloat3DArray::DimensionListType TRUE_DIMENSIONS({ 3, 2, 4 });
  TestAllocator allocator("TEST_1");
  TestFloat3DArray a({ 3, 2, 4 }, 1.0f, allocator);

  EXPECT_EQ(allocator.name(), a.allocator().name());
  EXPECT_TRUE(verifyArray(TRUE_DIMENSIONS, TRUE_DATA, a));
}

TEST(MdArrayTests, Initialize3DArrayFromIterator) {
  const std::vector<float> TRUE_DATA{
      -1.00f,  1.50f,  0.50f,  5.00f,  4.50f, -2.00f,
       1.00f, -4.00f, -1.50f,  0.25f,  1.75f, -1.75f,
       2.00f, -3.00f,  3.50f,  8.50f, -7.00f,  2.75f,
      -5.00f, -5.25f,  1.25f,  4.25f,  7.00f,  6.00f };
  const TestFloat3DArray::DimensionListType TRUE_DIMENSIONS({ 3, 2, 4 });
  TestAllocator allocator("TEST_1");
  TestFloat3DArray a({ 3, 2, 4 }, TRUE_DATA.begin(), allocator);

  EXPECT_EQ(allocator.name(), a.allocator().name());
  EXPECT_TRUE(verifyArray(TRUE_DIMENSIONS, TRUE_DATA, a));
}

TEST(MdArrayTests, Initialize3DArrayFromInitializerList) {
  const std::vector<float> TRUE_DATA{
      -1.00f,  1.50f,  0.50f,  5.00f,  4.50f, -2.00f,
       1.00f, -4.00f, -1.50f,  0.25f,  1.75f, -1.75f,
       2.00f, -3.00f,  3.50f,  8.50f, -7.00f,  2.75f,
      -5.00f, -5.25f,  1.25f,  4.25f,  7.00f,  6.00f };
  const TestFloat3DArray::DimensionListType TRUE_DIMENSIONS({ 3, 2, 4 });
  TestAllocator allocator("TEST_1");
  TestFloat3DArray a({ 3, 2, 4 },
		     { -1.00f,  1.50f,  0.50f,  5.00f,  4.50f, -2.00f,
			1.00f, -4.00f, -1.50f,  0.25f,  1.75f, -1.75f,
			2.00f, -3.00f,  3.50f,  8.50f, -7.00f,  2.75f,
		       -5.00f, -5.25f,  1.25f,  4.25f,  7.00f,  6.00f
		     },
		     allocator);

  EXPECT_EQ(allocator.name(), a.allocator().name());
  EXPECT_TRUE(verifyArray(TRUE_DIMENSIONS, TRUE_DATA, a));
}

TEST(MdArrayTests, Copy3DArrayOfOtherType) {
  const std::vector<float> TRUE_DATA{
      -1.00f,  1.50f,  0.50f,  5.00f,  4.50f, -2.00f,
       1.00f, -4.00f, -1.50f,  0.25f,  1.75f, -1.75f,
       2.00f, -3.00f,  3.50f,  8.50f, -7.00f,  2.75f,
      -5.00f, -5.25f,  1.25f,  4.25f,  7.00f,  6.00f };
  const TestFloat3DArray::DimensionListType TRUE_DIMENSIONS({ 3, 2, 4 });
  DoubleAllocator allocator("TEST_1");
  TestDouble3DArray src({ 3, 2, 4 }, TRUE_DATA.begin(), allocator);
  TestFloat3DArray copy(src);

  EXPECT_EQ(allocator.name(), copy.allocator().name());
  EXPECT_EQ(allocator.name(), src.allocator().name());
  EXPECT_TRUE(verifyArray(TRUE_DIMENSIONS, TRUE_DATA, copy));
}

TEST(MdArrayTests, Assign3DArray) {
  const std::vector<float> TRUE_DATA{
      -1.00f,  1.50f,  0.50f,  5.00f,  4.50f, -2.00f,
       1.00f, -4.00f, -1.50f,  0.25f,  1.75f, -1.75f,
       2.00f, -3.00f,  3.50f,  8.50f, -7.00f,  2.75f,
      -5.00f, -5.25f,  1.25f,  4.25f,  7.00f,  6.00f };
  const std::vector<float> INITIAL_DATA{
      -1.00f,  2.00f, -3.00f,  4.00f, -5.00f,  6.00f,
      -7.00f,  8.00f, -9.00f,  10.0f, -11.0f,  12.0f,
      -13.0f,  14.0f, -15.0f,  16.0f, -17.0f,  18.0f,
      -19.0f,  20.0f, -21.0f,  22.0f, -23.0f,  24.0f
  };
  const TestFloat3DArray::DimensionListType TRUE_DIMENSIONS({ 3, 2, 4 });
  TestAllocator allocator("TEST_1");
  TestAllocator copyAllocator("TEST_2");
  TestFloat3DArray src({ 3, 2, 4 }, TRUE_DATA.begin(), allocator);
  TestFloat3DArray copy({ 6, 2, 2 }, INITIAL_DATA.begin(), copyAllocator);

  EXPECT_EQ(allocator.name(), src.allocator().name());
  EXPECT_EQ(copyAllocator.name(), copy.allocator().name());
  EXPECT_TRUE(verifyArray({ 6, 2, 2 }, INITIAL_DATA, copy));
  EXPECT_TRUE(verifyArray(TRUE_DIMENSIONS, TRUE_DATA, src));

  copy = src;
  EXPECT_EQ(allocator.name(), copy.allocator().name());
  EXPECT_TRUE(verifyArray(TRUE_DIMENSIONS, TRUE_DATA, copy));
  EXPECT_TRUE(verifyArray(TRUE_DIMENSIONS, TRUE_DATA, src));
}

TEST(MdArrayTests, Assign3DArrayFromOtherType) {
  const std::vector<float> TRUE_DATA{
      -1.00f,  1.50f,  0.50f,  5.00f,  4.50f, -2.00f,
       1.00f, -4.00f, -1.50f,  0.25f,  1.75f, -1.75f,
       2.00f, -3.00f,  3.50f,  8.50f, -7.00f,  2.75f,
      -5.00f, -5.25f,  1.25f,  4.25f,  7.00f,  6.00f };
  const std::vector<float> INITIAL_DATA{
      -1.00f,  2.00f, -3.00f,  4.00f, -5.00f,  6.00f,
      -7.00f,  8.00f, -9.00f,  10.0f, -11.0f,  12.0f,
      -13.0f,  14.0f, -15.0f,  16.0f, -17.0f,  18.0f,
      -19.0f,  20.0f, -21.0f,  22.0f, -23.0f,  24.0f
  };
  const TestFloat3DArray::DimensionListType TRUE_DIMENSIONS({ 3, 2, 4 });
  DoubleAllocator allocator("TEST_1");
  TestAllocator copyAllocator("TEST_2");
  TestDouble3DArray src({ 3, 2, 4 }, TRUE_DATA.begin(), allocator);
  TestFloat3DArray copy({ 6, 2, 2 }, INITIAL_DATA.begin(), copyAllocator);

  EXPECT_EQ(allocator.name(), src.allocator().name());
  EXPECT_EQ(copyAllocator.name(), copy.allocator().name());
  EXPECT_TRUE(verifyArray({ 6, 2, 2 }, INITIAL_DATA, copy));
  EXPECT_TRUE(verifyArray(TRUE_DIMENSIONS, TRUE_DATA, src));

  copy = src;
  EXPECT_EQ(copyAllocator.name(), copy.allocator().name());
  EXPECT_TRUE(verifyArray(TRUE_DIMENSIONS, TRUE_DATA, copy));
  EXPECT_TRUE(verifyArray(TRUE_DIMENSIONS, TRUE_DATA, src));
}

TEST(MdArrayTests, Add3DArrays) {
  const Float3DArray::DimensionListType TRUE_DIMENSIONS({ 3, 2, 4 });
  const std::vector<float> DATA1{
      -1.00f,  1.50f,  0.50f,  5.00f,  4.50f, -2.00f,
       1.00f, -4.00f, -1.50f,  0.25f,  1.75f, -1.75f,
       2.00f, -3.00f,  3.50f,  8.50f, -7.00f,  2.75f,
      -5.00f, -5.25f,  1.25f,  4.25f,  7.00f,  6.00f };
  const std::vector<float> DATA2{
      -1.00f,  2.00f, -3.00f,  4.00f, -5.00f,  6.00f,
      -7.00f,  8.00f, -9.00f,  10.0f, -11.0f,  12.0f,
      -13.0f,  14.0f, -15.0f,  16.0f, -17.0f,  18.0f,
      -19.0f,  20.0f, -21.0f,  22.0f, -23.0f,  24.0f
  };
  const std::vector<float> SUM{
      -2.00f,  3.50f, -2.50f,  9.00f, -0.50f,  4.00f,
      -6.00f,  4.00f, -10.5f,  10.25f, -9.25f, 10.25f,
      -11.0f,  11.0f, -11.5f,  24.5f, -24.0f,  20.75f,
      -24.0f,  14.75f, -19.75f,  26.25f, -16.0f,  30.0f
  };
  Float3DArray a({3, 2, 4}, DATA1.begin());
  Float3DArray b({3, 2, 4}, DATA2.begin());
  Float3DArray c = a.add(b);

  EXPECT_TRUE(verifyArray(TRUE_DIMENSIONS, DATA1, a));
  EXPECT_TRUE(verifyArray(TRUE_DIMENSIONS, DATA2, b));
  EXPECT_TRUE(verifyArray(TRUE_DIMENSIONS, SUM, c));
}

TEST(MdArrayTests, Add3DArraysInPlace) {
  const Float3DArray::DimensionListType TRUE_DIMENSIONS({ 3, 2, 4 });
  const std::vector<float> DATA1{
      -1.00f,  1.50f,  0.50f,  5.00f,  4.50f, -2.00f,
       1.00f, -4.00f, -1.50f,  0.25f,  1.75f, -1.75f,
       2.00f, -3.00f,  3.50f,  8.50f, -7.00f,  2.75f,
      -5.00f, -5.25f,  1.25f,  4.25f,  7.00f,  6.00f };
  const std::vector<float> DATA2{
      -1.00f,  2.00f, -3.00f,  4.00f, -5.00f,  6.00f,
      -7.00f,  8.00f, -9.00f,  10.0f, -11.0f,  12.0f,
      -13.0f,  14.0f, -15.0f,  16.0f, -17.0f,  18.0f,
      -19.0f,  20.0f, -21.0f,  22.0f, -23.0f,  24.0f
  };
  const std::vector<float> SUM{
      -2.00f,  3.50f, -2.50f,  9.00f, -0.50f,  4.00f,
      -6.00f,  4.00f, -10.5f,  10.25f, -9.25f, 10.25f,
      -11.0f,  11.0f, -11.5f,  24.5f, -24.0f,  20.75f,
      -24.0f,  14.75f, -19.75f,  26.25f, -16.0f,  30.0f
  };
  Float3DArray a({3, 2, 4}, DATA1.begin());
  Float3DArray b({3, 2, 4}, DATA2.begin());
  Float3DArray& c = a.addInPlace(b);

  EXPECT_EQ(&a, &c);
  EXPECT_TRUE(verifyArray(TRUE_DIMENSIONS, SUM, a));
  EXPECT_TRUE(verifyArray(TRUE_DIMENSIONS, DATA2, b));
}

TEST(MdArrayTests, ScaleAndAdd3DArrays) {
  const Float3DArray::DimensionListType TRUE_DIMENSIONS({ 3, 2, 4 });
  const std::vector<float> DATA1{
      -1.00f,  1.50f,  0.50f,  5.00f,  4.50f, -2.00f,
       1.00f, -4.00f, -1.50f,  0.25f,  1.75f, -1.75f,
       2.00f, -3.00f,  3.50f,  8.50f, -7.00f,  2.75f,
      -5.00f, -5.25f,  1.25f,  4.25f,  7.00f,  6.00f };
  const std::vector<float> DATA2{
      -1.00f,  2.00f, -3.00f,  4.00f, -5.00f,  6.00f,
      -7.00f,  8.00f, -9.00f,  10.0f, -11.0f,  12.0f,
      -13.0f,  14.0f, -15.0f,  16.0f, -17.0f,  18.0f,
      -19.0f,  20.0f, -21.0f,  22.0f, -23.0f,  24.0f
  };
  const std::vector<float> SUM{
      -3.00f,  5.50f, -5.50f,  13.0f, -5.50f,  10.0f,
      -13.0f,  12.0f, -19.5f, 20.25f, -20.25f, 22.25f,
      -24.0f,  25.0f, -26.5f,  40.5f, -41.0f,  38.75f,
      -43.0f, 34.75f, -40.75f, 48.25f, -39.0f, 54.0f     
  };
  Float3DArray a({3, 2, 4}, DATA1.begin());
  Float3DArray b({3, 2, 4}, DATA2.begin());
  Float3DArray c = a.scaleAndAdd(2.0, b);

  EXPECT_TRUE(verifyArray(TRUE_DIMENSIONS, DATA1, a));
  EXPECT_TRUE(verifyArray(TRUE_DIMENSIONS, DATA2, b));
  EXPECT_TRUE(verifyArray(TRUE_DIMENSIONS, SUM, c));
}

TEST(MdArrayTests, ScaleAndAdd3DArraysIntoResult) {
  const Float3DArray::DimensionListType TRUE_DIMENSIONS({ 3, 2, 4 });
  const std::vector<float> DATA1{
      -1.00f,  1.50f,  0.50f,  5.00f,  4.50f, -2.00f,
       1.00f, -4.00f, -1.50f,  0.25f,  1.75f, -1.75f,
       2.00f, -3.00f,  3.50f,  8.50f, -7.00f,  2.75f,
      -5.00f, -5.25f,  1.25f,  4.25f,  7.00f,  6.00f };
  const std::vector<float> DATA2{
      -1.00f,  2.00f, -3.00f,  4.00f, -5.00f,  6.00f,
      -7.00f,  8.00f, -9.00f,  10.0f, -11.0f,  12.0f,
      -13.0f,  14.0f, -15.0f,  16.0f, -17.0f,  18.0f,
      -19.0f,  20.0f, -21.0f,  22.0f, -23.0f,  24.0f
  };
  const std::vector<float> SUM{
      -3.00f,  5.50f, -5.50f,  13.0f, -5.50f,  10.0f,
      -13.0f,  12.0f, -19.5f, 20.25f, -20.25f, 22.25f,
      -24.0f,  25.0f, -26.5f,  40.5f, -41.0f,  38.75f,
      -43.0f, 34.75f, -40.75f, 48.25f, -39.0f, 54.0f     
  };
  Float3DArray a({3, 2, 4}, DATA1.begin());
  Float3DArray b({3, 2, 4}, DATA2.begin());
  Float3DArray c({3, 2, 4}, 1.0);
  Float3DArray& d = a.scaleAndAdd(2.0, b, c);

  EXPECT_EQ(&c, &d);
  EXPECT_TRUE(verifyArray(TRUE_DIMENSIONS, DATA1, a));
  EXPECT_TRUE(verifyArray(TRUE_DIMENSIONS, DATA2, b));
  EXPECT_TRUE(verifyArray(TRUE_DIMENSIONS, SUM, c));
}

TEST(MdArrayTests, ScaleBothAndAdd3DArrays) {
  const Float3DArray::DimensionListType TRUE_DIMENSIONS({ 3, 2, 4 });
  const std::vector<float> DATA1{
      -1.00f,  1.50f,  0.50f,  5.00f,  4.50f, -2.00f,
       1.00f, -4.00f, -1.50f,  0.25f,  1.75f, -1.75f,
       2.00f, -3.00f,  3.50f,  8.50f, -7.00f,  2.75f,
      -5.00f, -5.25f,  1.25f,  4.25f,  7.00f,  6.00f };
  const std::vector<float> DATA2{
      -1.00f,  2.00f, -3.00f,  4.00f, -5.00f,  6.00f,
      -7.00f,  8.00f, -9.00f,  10.0f, -11.0f,  12.0f,
      -13.0f,  14.0f, -15.0f,  16.0f, -17.0f,  18.0f,
      -19.0f,  20.0f, -21.0f,  22.0f, -23.0f,  24.0f
  };
  const std::vector<float> SUM{
       -2.5f,  4.75f, -5.75f,  10.5f, -7.75f,  11.0f,
       -13.5f, 14.0f, -18.75f, 20.125f, -21.125f, 23.125f,
       -25.0f, 26.5f, -28.25f, 36.25f, -37.5f, 37.375f,
       -40.5f, 37.375f, -41.375f, 46.125f, -42.5f, 51.0f  
  };
  Float3DArray a({3, 2, 4}, DATA1.begin());
  Float3DArray b({3, 2, 4}, DATA2.begin());
  Float3DArray c = a.scaleAndAdd(0.5, 2.0, b);

  EXPECT_TRUE(verifyArray(TRUE_DIMENSIONS, DATA1, a));
  EXPECT_TRUE(verifyArray(TRUE_DIMENSIONS, DATA2, b));
  EXPECT_TRUE(verifyArray(TRUE_DIMENSIONS, SUM, c));
}

TEST(MdArrayTests, ScaleBothAndAdd3DArraysIntoResult) {
  const Float3DArray::DimensionListType TRUE_DIMENSIONS({ 3, 2, 4 });
  const std::vector<float> DATA1{
      -1.00f,  1.50f,  0.50f,  5.00f,  4.50f, -2.00f,
       1.00f, -4.00f, -1.50f,  0.25f,  1.75f, -1.75f,
       2.00f, -3.00f,  3.50f,  8.50f, -7.00f,  2.75f,
      -5.00f, -5.25f,  1.25f,  4.25f,  7.00f,  6.00f };
  const std::vector<float> DATA2{
      -1.00f,  2.00f, -3.00f,  4.00f, -5.00f,  6.00f,
      -7.00f,  8.00f, -9.00f,  10.0f, -11.0f,  12.0f,
      -13.0f,  14.0f, -15.0f,  16.0f, -17.0f,  18.0f,
      -19.0f,  20.0f, -21.0f,  22.0f, -23.0f,  24.0f
  };
  const std::vector<float> SUM{
       -2.5f,  4.75f, -5.75f,  10.5f, -7.75f,  11.0f,
       -13.5f, 14.0f, -18.75f, 20.125f, -21.125f, 23.125f,
       -25.0f, 26.5f, -28.25f, 36.25f, -37.5f, 37.375f,
       -40.5f, 37.375f, -41.375f, 46.125f, -42.5f, 51.0f  
  };
  Float3DArray a({3, 2, 4}, DATA1.begin());
  Float3DArray b({3, 2, 4}, DATA2.begin());
  Float3DArray c({3, 2, 4}, 1.0);
  Float3DArray& d = a.scaleAndAdd(0.5, 2.0, b, c);

  EXPECT_EQ(&c, &d);
  EXPECT_TRUE(verifyArray(TRUE_DIMENSIONS, DATA1, a));
  EXPECT_TRUE(verifyArray(TRUE_DIMENSIONS, DATA2, b));
  EXPECT_TRUE(verifyArray(TRUE_DIMENSIONS, SUM, c));
}

TEST(MdArrayTests, Subtract3DArrays) {
  const Float3DArray::DimensionListType TRUE_DIMENSIONS({ 3, 2, 4 });
  const std::vector<float> DATA1{
      -1.00f,  1.50f,  0.50f,  5.00f,  4.50f, -2.00f,
       1.00f, -4.00f, -1.50f,  0.25f,  1.75f, -1.75f,
       2.00f, -3.00f,  3.50f,  8.50f, -7.00f,  2.75f,
      -5.00f, -5.25f,  1.25f,  4.25f,  7.00f,  6.00f };
  const std::vector<float> DATA2{
      -1.00f,  2.00f, -3.00f,  4.00f, -5.00f,  6.00f,
      -7.00f,  8.00f, -9.00f,  10.0f, -11.0f,  12.0f,
      -13.0f,  14.0f, -15.0f,  16.0f, -17.0f,  18.0f,
      -19.0f,  20.0f, -21.0f,  22.0f, -23.0f,  24.0f
  };
  const std::vector<float> DIFFERENCE{
       0.00f, -0.50f,  3.50f,  1.00f,  9.50f,  -8.0f,
       8.00f, -12.0f,  7.50f, -9.75f, 12.75f, -13.75f,
       15.0f, -17.0f , 18.5f, -7.50f,  10.0f, -15.25f,
       14.0f, -25.25f,  22.25f, -17.75f, 30.0f, -18.0f
  };
  Float3DArray a({3, 2, 4}, DATA1.begin());
  Float3DArray b({3, 2, 4}, DATA2.begin());
  Float3DArray c = a.subtract(b);

  EXPECT_TRUE(verifyArray(TRUE_DIMENSIONS, DATA1, a));
  EXPECT_TRUE(verifyArray(TRUE_DIMENSIONS, DATA2, b));
  EXPECT_TRUE(verifyArray(TRUE_DIMENSIONS, DIFFERENCE, c));
}

TEST(MdArrayTests, Subtract3DArraysInPlace) {
  const Float3DArray::DimensionListType TRUE_DIMENSIONS({ 3, 2, 4 });
  const std::vector<float> DATA1{
      -1.00f,  1.50f,  0.50f,  5.00f,  4.50f, -2.00f,
       1.00f, -4.00f, -1.50f,  0.25f,  1.75f, -1.75f,
       2.00f, -3.00f,  3.50f,  8.50f, -7.00f,  2.75f,
      -5.00f, -5.25f,  1.25f,  4.25f,  7.00f,  6.00f };
  const std::vector<float> DATA2{
      -1.00f,  2.00f, -3.00f,  4.00f, -5.00f,  6.00f,
      -7.00f,  8.00f, -9.00f,  10.0f, -11.0f,  12.0f,
      -13.0f,  14.0f, -15.0f,  16.0f, -17.0f,  18.0f,
      -19.0f,  20.0f, -21.0f,  22.0f, -23.0f,  24.0f
  };
  const std::vector<float> DIFFERENCE{
       0.00f, -0.50f,  3.50f,  1.00f,  9.50f,  -8.0f,
       8.00f, -12.0f,  7.50f, -9.75f, 12.75f, -13.75f,
       15.0f, -17.0f , 18.5f, -7.50f,  10.0f, -15.25f,
       14.0f, -25.25f,  22.25f, -17.75f, 30.0f, -18.0f
  };
  Float3DArray a({3, 2, 4}, DATA1.begin());
  Float3DArray b({3, 2, 4}, DATA2.begin());
  Float3DArray& c = a.subtractInPlace(b);

  EXPECT_EQ(&a, &c);
  EXPECT_TRUE(verifyArray(TRUE_DIMENSIONS, DIFFERENCE, a));
  EXPECT_TRUE(verifyArray(TRUE_DIMENSIONS, DATA2, b));
}

TEST(MdArrayTests, Multiply3DArraysByScalar) {
  const Float3DArray::DimensionListType TRUE_DIMENSIONS({ 3, 2, 4 });
  const std::vector<float> DATA1{
      -1.00f,  1.50f,  0.50f,  5.00f,  4.50f, -2.00f,
       1.00f, -4.00f, -1.50f,  0.25f,  1.75f, -1.75f,
       2.00f, -3.00f,  3.50f,  8.50f, -7.00f,  2.75f,
      -5.00f, -5.25f,  1.25f,  4.25f,  7.00f,  6.00f };
  const std::vector<float> PRODUCT{
      -2.00f,  3.00f,  1.00f,  10.0f,  9.00f, -4.00f,
       2.00f, -8.00f, -3.00f,  0.50f,  3.50f, -3.50f,
       4.00f, -6.00f,  7.00f,  17.0f, -14.0f,  5.50f,
      -10.0f, -10.5f,  2.50f,  8.50f,  14.0f,  12.0f
  };
  Float3DArray a({3, 2, 4}, DATA1.begin());
  Float3DArray b = a.multiply(2.0);

  EXPECT_TRUE(verifyArray(TRUE_DIMENSIONS, DATA1, a));
  EXPECT_TRUE(verifyArray(TRUE_DIMENSIONS, PRODUCT, b));
}

TEST(MdArrayTests, Multiply3DArrayByScalarIntoResultArray) {
  const Float3DArray::DimensionListType TRUE_DIMENSIONS({ 3, 2, 4 });
  const std::vector<float> DATA1{
      -1.00f,  1.50f,  0.50f,  5.00f,  4.50f, -2.00f,
       1.00f, -4.00f, -1.50f,  0.25f,  1.75f, -1.75f,
       2.00f, -3.00f,  3.50f,  8.50f, -7.00f,  2.75f,
      -5.00f, -5.25f,  1.25f,  4.25f,  7.00f,  6.00f };
  const std::vector<float> DATA2{
      -1.00f,  2.00f, -3.00f,  4.00f, -5.00f,  6.00f,
      -7.00f,  8.00f, -9.00f,  10.0f, -11.0f,  12.0f,
      -13.0f,  14.0f, -15.0f,  16.0f, -17.0f,  18.0f,
      -19.0f,  20.0f, -21.0f,  22.0f, -23.0f,  24.0f
  };
  const std::vector<float> PRODUCT{
      -2.00f,  3.00f,  1.00f,  10.0f,  9.00f, -4.00f,
       2.00f, -8.00f, -3.00f,  0.50f,  3.50f, -3.50f,
       4.00f, -6.00f,  7.00f,  17.0f, -14.0f,  5.50f,
      -10.0f, -10.5f,  2.50f,  8.50f,  14.0f,  12.0f
  };
  Float3DArray a({3, 2, 4}, DATA1.begin());
  Float3DArray b({3, 2, 4}, DATA2.begin());

  EXPECT_TRUE(verifyArray({3, 2, 4}, DATA2, b));

  Float3DArray& c = a.multiply(2.0, b);

  EXPECT_EQ(&b, &c);
  EXPECT_TRUE(verifyArray(TRUE_DIMENSIONS, DATA1, a));
  EXPECT_TRUE(verifyArray(TRUE_DIMENSIONS, PRODUCT, b));

}

TEST(MdArrayTests, Multiply3DArraysElementWise) {
  const Float3DArray::DimensionListType TRUE_DIMENSIONS({ 3, 2, 4 });
  const std::vector<float> DATA1{
      -1.00f,  1.50f,  0.50f,  5.00f,  4.50f, -2.00f,
       1.00f, -4.00f, -1.50f,  0.25f,  1.75f, -1.75f,
       2.00f, -3.00f,  3.50f,  8.50f, -7.00f,  2.75f,
      -5.00f, -5.25f,  1.25f,  4.25f,  7.00f,  6.00f };
  const std::vector<float> DATA2{
      -1.00f,  2.00f, -3.00f,  4.00f, -5.00f,  6.00f,
      -7.00f,  8.00f, -9.00f,  10.0f, -11.0f,  12.0f,
      -13.0f,  14.0f, -15.0f,  16.0f, -17.0f,  18.0f,
      -19.0f,  20.0f, -21.0f,  22.0f, -23.0f,  24.0f
  };
  const std::vector<float> PRODUCT{
      1.00f,  3.00f,  -1.50f,  20.0f, -22.5f, -12.0f,
      -7.0f, -32.0f,   13.5f,   2.5f, -19.25f, -21.0f,
     -26.0f, -42.0f,  -52.5f, 136.0f, 119.0f,  49.5f,
      95.0f, -105.0f, -26.25f, 93.5f, -161.0f,  144.0f
  };
  Float3DArray a({3, 2, 4}, DATA1.begin());
  Float3DArray b({3, 2, 4}, DATA2.begin());
  Float3DArray c = a.multiply(b);

  EXPECT_TRUE(verifyArray(TRUE_DIMENSIONS, DATA1, a));
  EXPECT_TRUE(verifyArray(TRUE_DIMENSIONS, DATA2, b));
  EXPECT_TRUE(verifyArray(TRUE_DIMENSIONS, PRODUCT, c));
}

TEST(MdArrayTests, Multiply3DArraysElementWiseInPlace) {
  const Float3DArray::DimensionListType TRUE_DIMENSIONS({ 3, 2, 4 });
  const std::vector<float> DATA1{
      -1.00f,  1.50f,  0.50f,  5.00f,  4.50f, -2.00f,
       1.00f, -4.00f, -1.50f,  0.25f,  1.75f, -1.75f,
       2.00f, -3.00f,  3.50f,  8.50f, -7.00f,  2.75f,
      -5.00f, -5.25f,  1.25f,  4.25f,  7.00f,  6.00f };
  const std::vector<float> DATA2{
      -1.00f,  2.00f, -3.00f,  4.00f, -5.00f,  6.00f,
      -7.00f,  8.00f, -9.00f,  10.0f, -11.0f,  12.0f,
      -13.0f,  14.0f, -15.0f,  16.0f, -17.0f,  18.0f,
      -19.0f,  20.0f, -21.0f,  22.0f, -23.0f,  24.0f
  };
  const std::vector<float> PRODUCT{
      1.00f,  3.00f,  -1.50f,  20.0f, -22.5f, -12.0f,
      -7.0f, -32.0f,   13.5f,   2.5f, -19.25f, -21.0f,
     -26.0f, -42.0f,  -52.5f, 136.0f, 119.0f,  49.5f,
      95.0f, -105.0f, -26.25f, 93.5f, -161.0f,  144.0f
  };
  Float3DArray a({3, 2, 4}, DATA1.begin());
  Float3DArray b({3, 2, 4}, DATA2.begin());
  Float3DArray& c = a.multiplyInPlace(b);

  EXPECT_EQ(&a, &c);
  EXPECT_TRUE(verifyArray(TRUE_DIMENSIONS, PRODUCT, a));
  EXPECT_TRUE(verifyArray(TRUE_DIMENSIONS, DATA2, b));
}

TEST(MdArrayTests, Divide3DArraysElementWise) {
  const Float3DArray::DimensionListType TRUE_DIMENSIONS({ 3, 2, 4 });
  const std::vector<float> DATA1{
      -1.00f,  1.50f,  0.50f,  5.00f,  4.50f, -2.00f,
       1.00f, -4.00f, -1.50f,  0.25f,  1.75f, -1.75f,
       2.00f, -3.00f,  3.50f,  8.50f, -7.00f,  2.75f,
      -5.00f, -5.25f,  1.25f,  4.25f,  7.00f,  6.00f };
  const std::vector<float> DATA2{
      -1.00f,  2.00f, -3.00f,  4.00f, -5.00f,  6.00f,
      -7.00f,  8.00f, -9.00f,  10.0f, -11.0f,  12.0f,
      -13.0f,  14.0f, -15.0f,  16.0f, -17.0f,  18.0f,
      -19.0f,  20.0f, -21.0f,  22.0f, -23.0f,  24.0f
  };
  const std::vector<float> QUOTIENT{
       1.00000000,  0.75000000, -0.16666667,  1.25000000,
      -0.90000000, -0.33333333, -0.14285714, -0.50000000,
       0.16666667,  0.02500000, -0.15909091, -0.14583333,
      -0.15384615, -0.21428571, -0.23333333,  0.53125000,
       0.41176471,  0.15277778,  0.26315789, -0.26250000,
      -0.05952381,  0.19318182, -0.30434783,  0.25000000 
  };
  Float3DArray a({3, 2, 4}, DATA1.begin());
  Float3DArray b({3, 2, 4}, DATA2.begin());
  Float3DArray c = a.divide(b);

  EXPECT_TRUE(verifyArray(TRUE_DIMENSIONS, DATA1, a));
  EXPECT_TRUE(verifyArray(TRUE_DIMENSIONS, DATA2, b));
  EXPECT_TRUE(verifyArray(TRUE_DIMENSIONS, QUOTIENT, c));
}

TEST(MdArrayTests, Divide3DArraysElementWiseInPlace) {
  const Float3DArray::DimensionListType TRUE_DIMENSIONS({ 3, 2, 4 });
  const std::vector<float> DATA1{
      -1.00f,  1.50f,  0.50f,  5.00f,  4.50f, -2.00f,
       1.00f, -4.00f, -1.50f,  0.25f,  1.75f, -1.75f,
       2.00f, -3.00f,  3.50f,  8.50f, -7.00f,  2.75f,
      -5.00f, -5.25f,  1.25f,  4.25f,  7.00f,  6.00f };
  const std::vector<float> DATA2{
      -1.00f,  2.00f, -3.00f,  4.00f, -5.00f,  6.00f,
      -7.00f,  8.00f, -9.00f,  10.0f, -11.0f,  12.0f,
      -13.0f,  14.0f, -15.0f,  16.0f, -17.0f,  18.0f,
      -19.0f,  20.0f, -21.0f,  22.0f, -23.0f,  24.0f
  };
  const std::vector<float> QUOTIENT{
       1.00000000,  0.75000000, -0.16666667,  1.25000000,
      -0.90000000, -0.33333333, -0.14285714, -0.50000000,
       0.16666667,  0.02500000, -0.15909091, -0.14583333,
      -0.15384615, -0.21428571, -0.23333333,  0.53125000,
       0.41176471,  0.15277778,  0.26315789, -0.26250000,
      -0.05952381,  0.19318182, -0.30434783,  0.25000000 
  };
  Float3DArray a({3, 2, 4}, DATA1.begin());
  Float3DArray b({3, 2, 4}, DATA2.begin());
  Float3DArray& c = a.divideInPlace(b);

  EXPECT_EQ(&a, &c);
  EXPECT_TRUE(verifyArray(TRUE_DIMENSIONS, QUOTIENT, a));
  EXPECT_TRUE(verifyArray(TRUE_DIMENSIONS, DATA2, b));
}

TEST(MdArrayTests, InnerProductWith3DArray) {
  const std::vector<float> DATA1{
      -1.00f,  1.50f,  0.50f,  5.00f,  4.50f, -2.00f,
       1.00f, -4.00f, -1.50f,  0.25f,  1.75f, -1.75f,
       2.00f, -3.00f,  3.50f,  8.50f, -7.00f,  2.75f,
      -5.00f, -5.25f,  1.25f,  4.25f,  7.00f,  6.00f
  };
  const std::vector<float> DATA2{ 2.0f, 0.5f, -0.5f, 3.0f };
  const std::vector<float> TRUTH{
    13.5f, -4.5f, -9.0f, 26.25f, -25.875f, 19.125f
  };
  Float3DArray a({3, 2, 4}, DATA1.begin());
  FloatVector v({4}, DATA2.begin());
  FloatMatrix result = a.innerProduct(v);

  EXPECT_TRUE(verifyArray({3, 2, 4}, DATA1, a));
  EXPECT_TRUE(verifyArray({4}, DATA2, v));
  EXPECT_TRUE(verifyArray({3, 2}, TRUTH, result));
}

TEST(MdArrayTests, InnerProductWithMatrix) {
  const std::vector<float> DATA1{
      -1.00f,  1.50f,  0.50f,  5.00f,
       4.50f, -2.00f,  1.00f, -4.00f,
  };
  const std::vector<float> DATA2{ 2.0f, 0.5f, -0.5f, 3.0f };
  const std::vector<float> TRUTH{
    13.5f, -4.5f
  };
  FloatMatrix a({2, 4}, DATA1.begin());
  FloatVector v({4}, DATA2.begin());
  FloatVector result = a.innerProduct(v);

  EXPECT_TRUE(verifyArray({2, 4}, DATA1, a));
  EXPECT_TRUE(verifyArray({4}, DATA2, v));
  EXPECT_TRUE(verifyArray({2}, TRUTH, result));
}

TEST(MdArrayTests, InnerProductWithVector) {
  const std::vector<float> DATA1{
      -1.00f,  1.50f,  0.50f,  5.00f,
  };
  const std::vector<float> DATA2{ 2.0f, 0.5f, -0.5f, 3.0f };
  FloatVector a({4}, DATA1.begin());
  FloatVector v({4}, DATA2.begin());
  float result = a.innerProduct(v);

  EXPECT_TRUE(verifyArray({4}, DATA1, a));
  EXPECT_TRUE(verifyArray({4}, DATA2, v));
  EXPECT_EQ(13.5f, result);
}

TEST(MdArrayTests, MatrixProductWith3DArray) {
  const std::vector<float> DATA1{
      -1.00f,  1.50f,  0.50f,  5.00f,  4.50f, -2.00f,
       1.00f, -4.00f, -1.50f,  0.25f,  1.75f, -1.75f,
       2.00f, -3.00f,  3.50f,  8.50f, -7.00f,  2.75f,
      -5.00f, -5.25f,  1.25f,  4.25f,  7.00f,  6.00f
  };
  const std::vector<float> DATA2{ 2.0f, 0.5f, -0.5f, 3.0f,
                                  1.5f, -1.5f, 1.0f, 2.5f };
  const std::vector<float> TRUTH{
      3.000f,  15.750f,   7.500f, -15.250f,
     -2.250f,  -7.000f,  19.250f,   8.000f,
    -28.125f,  -0.875f,  16.875f,  17.875f
  };
  Float3DArray a({3, 2, 4}, DATA1.begin());
  FloatMatrix v({4, 2}, DATA2.begin());
  Float3DArray result = a.matrixProduct(v);

  EXPECT_TRUE(verifyArray({3, 2, 4}, DATA1, a));
  EXPECT_TRUE(verifyArray({4, 2 }, DATA2, v));
  EXPECT_TRUE(verifyArray({3, 2, 2}, TRUTH, result));
}

TEST(MdArrayTests, MatrixProductWithVector) {
  const std::vector<float> DATA1{
      -1.00f,  1.50f,  0.50f,  5.00f,
  };
  const std::vector<float> DATA2{ 2.0f, 0.5f, -0.5f, 3.0f,
                                  1.5f, -1.5f, 1.0f, 2.5f };
  const std::vector<float> TRUTH{
      3.000f,  15.750f
  };
  FloatVector a({4}, DATA1.begin());
  FloatMatrix v({4, 2}, DATA2.begin());
  FloatVector result = a.matrixProduct(v);

  EXPECT_TRUE(verifyArray({4}, DATA1, a));
  EXPECT_TRUE(verifyArray({4, 2}, DATA2, v));
  EXPECT_TRUE(verifyArray({2}, TRUTH, result));
}

TEST(MdArrayTests, Slice) {
  const std::vector<float> DATA{
      -1.00f,  1.50f,  0.50f,  5.00f,  4.50f, -2.00f,
       1.00f, -4.00f, -1.50f,  0.25f,  1.75f, -1.75f,
       2.00f, -3.00f,  3.50f,  8.50f, -7.00f,  2.75f,
      -5.00f, -5.25f,  1.25f,  4.25f,  7.00f,  6.00f
  };
  TestAllocator allocator("TEST_1");
  TestFloat3DArray a({3, 2, 4}, DATA.begin(), allocator);

  for (size_t i = 0; i < a.dimensions()[0]; ++i) {
    std::vector<float> truth(DATA.begin() + i * a.leadingDimension(),
			     DATA.begin() + (i + 1) * a.leadingDimension());
    TestFloat3DArray::SliceType slice = a[i];
    EXPECT_TRUE(verifyArray({ 2, 4 }, truth, slice));
  }

  for (size_t i = 0; i < a.dimensions()[0]; ++i) {
    for (size_t j = 0; j < a.dimensions()[1]; ++j) {
      for (size_t k = 0; k < a.dimensions()[2]; ++k) {
  	size_t o = ((i * a.dimensions()[1] + j) * a.dimensions()[2]) + k;
  	EXPECT_EQ(DATA[o], a[i][j][k]);
      }
    }
  }
}
