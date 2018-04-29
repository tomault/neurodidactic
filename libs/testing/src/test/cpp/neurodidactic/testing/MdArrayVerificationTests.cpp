#include <neurodidactic/testing/MdArrayVerification.hpp>

#include <gtest/gtest.h>
#include <algorithm>
#include <vector>

namespace {
  class MockDimensionList {
  public:
    explicit MockDimensionList(const std::vector<size_t>& dimensions):
        dimensions_(dimensions) {
    }
    MockDimensionList(const std::initializer_list<size_t>& dimensions):
        dimensions_(dimensions) {
    }
    MockDimensionList(const MockDimensionList&) = default;
    MockDimensionList(MockDimensionList&&) = default;

    size_t size() const { return dimensions_.size(); }
    auto begin() const { return dimensions_.begin(); }
    auto end() const { return dimensions_.end(); }
    size_t numElements() const {
      if (dimensions_.empty()) {
	return 0;
      } else {
	size_t n = 1;
	for (auto i : dimensions_) {
	  n *= i;
	}
	return n;
      }
    }
    
    MockDimensionList& operator=(const MockDimensionList&) = default;
    MockDimensionList& operator=(MockDimensionList&&) = default;
    
    size_t operator[](size_t n) const { return dimensions_[n]; }
    bool operator==(const MockDimensionList& other) const {
      return (size() == other.size()) &&
	  std::equal(begin(), end(), other.begin());
    }

    bool operator!=(const MockDimensionList& other) const {
      return (size() != other.size()) ||
	  !std::equal(begin(), end(), other.begin());
    }

  private:
    std::vector<size_t> dimensions_;
  };

  std::ostream& operator<<(std::ostream& out, const MockDimensionList& l) {
    out << "[";
    for (auto i = l.begin(); i != l.end(); ++i) {
      if (i != l.begin()) {
	out << ",";
      }
      out << " " << *i;
    }
    return out << " ]";
  }
  
  class MockMdArray {
  public:
    typedef float FieldType;
    typedef MockDimensionList DimensionListType;
    
  public:
    template <typename Iterator>
    MockMdArray(const MockDimensionList& dimensions, Iterator data):
        dimensions_(dimensions), data_(data, data + dimensions.numElements()) {
    }
    MockMdArray(const MockMdArray&) = default;
    MockMdArray(MockMdArray&&) = default;

    const MockDimensionList& dimensions() const { return dimensions_; }
    const size_t size() const { return data_.size(); }
    const size_t leadingDimension() const {
      return dimensions_.numElements()/dimensions_[0];
    }
    auto begin() const { return data_.begin(); }
    auto end() const { return data_.end(); }
    auto data() const { return data_.begin(); }

    float operator[](size_t n) const { return data_[n]; }

    MockMdArray& operator=(const MockMdArray&) = default;
    MockMdArray& operator=(MockMdArray&&) = default;
    
  private:
    MockMdArray dimensions_;
    std::vector<float>  data_;
  };
}

using namespace neurodidactic::testing;

TEST(MdArrayVerificationTests, VerificationPasses) {
  MockDimensionList DIMENSIONS{3, 2, 4};
  const std::vector<float> DATA{
      1.0f,  3.0f, -2.5f,  4.0f,
     -1.0f,  2.0f,  1.5f, -3.5f,
      0.5f,  2.5f,  4.5f, -4.5f,
      3.5f, -0.5f, -4.0f, -2.0f,
      6.0f, -6.0f, -5.0f,  5.0f,
     -5.5f,  5.5f,  4.5f,  6.5f
  };
  MockMdArray array(DIMENSIONS, DATA.begin());

  EXPECT_TRUE(verifyMdArray({3, 2, 4}, DATA, array));
}
