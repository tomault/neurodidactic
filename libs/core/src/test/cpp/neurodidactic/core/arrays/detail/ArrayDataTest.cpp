#include <neurodidactic/core/arrays/detail/ArrayData.hpp>
#include <neurodidactic/core/arrays/DimensionList.hpp>
#include <pistis/testing/Allocator.hpp>
#include <gtest/gtest.h>
#include <stdint.h>

using namespace neurodidactic::core::arrays;
using namespace neurodidactic::core::arrays::detail;
namespace pt = pistis::testing;

namespace {
  typedef pt::Allocator<uint32_t> UInt32Allocator;
  typedef ArrayData<uint32_t, UInt32Allocator> UInt32Array;
}



TEST(ArrayDataTests, Create) {
  UInt32Allocator allocator("TEST_1");
  UInt32Allocator dimensionAllocator("TEST_2");
  UInt32Array::DimensionListType dimensions{{ 10, 20, 2 },
                                            dimensionAllocator};
  UInt32Array array(UInt32Array::DimensionListType(dimensions), allocator);
  const UInt32Array& carray = array;

  EXPECT_EQ("TEST_1", array.allocator().name());
  EXPECT_EQ("TEST_1", carray.allocator().name());
  EXPECT_EQ(dimensions, array.dimensions());
  EXPECT_EQ("TEST_2", array.dimensions().allocator().name());
  EXPECT_EQ(400, array.size());
  EXPECT_NE((uint32_t*)0, array.data());
  EXPECT_NE((uint32_t*)0, carray.data());
  EXPECT_EQ(0, array.refCnt());
}

TEST(ArrayDataTests, ElementAccess) {
  UInt32Array array(UInt32Array::DimensionListType(
                        { 10, 20, 2 },
			UInt32Array::DimensionListType::AllocatorType()),
		    UInt32Allocator());
  const UInt32Array& carray = array;

  for (uint32_t i = 0; i < 400; ++i) {
    array[i] = i + 1;
    EXPECT_EQ(i + 1, array[i]);
    EXPECT_EQ(i + 1, carray[i]);
  }
}

TEST(ArrayDataTests, AddAndRemoveReference) {
  UInt32Array array(UInt32Array::DimensionListType(
			{ 10, 20, 2 },
			UInt32Array::DimensionListType::AllocatorType()),
		    UInt32Allocator());

  EXPECT_EQ(0, array.refCnt());
  EXPECT_EQ(&array, &array.addRef());
  EXPECT_EQ(1, array.refCnt());
  EXPECT_EQ(0, array.removeRef());
  EXPECT_EQ(0, array.refCnt());
}
