#ifndef __NEURODIDACTIC__TESTING__MDARRAYVERIFICATION_HPP__
#define __NEURODIDACTIC__TESTING__MDARRAYVERIFICATION_HPP__

#include <gtest/gtest.h>
#include <sstream>

namespace neurodidactic {
  namespace testing {

    template <typename Array>
    std::string mdArrayDataToString(const Array& a, size_t index = -1) {
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
    ::testing::AssertionResult verifyMdArray(
	const typename Array::DimensionListType& trueDimensions,
	const std::vector<typename Array::FieldType>& trueData,
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
	      << "Array.data() is " << mdArrayDataToString(array, i)
	      << ", but it should be " << mdArrayDataToString(trueData, i)
	      << " -- elements at index " << i << " differ ("
	      << trueData[i] << " vs. " << data[i] << ", delta = "
	      << (trueData[i] - data[i]);
      }
    }

    return ::testing::AssertionSuccess();
  }

    
  }
}

#endif
