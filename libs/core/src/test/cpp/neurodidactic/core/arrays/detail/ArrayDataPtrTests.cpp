#include <neurodidactic/core/arrays/detail/ArrayDataPtr.hpp>
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
  typedef ArrayDataPtr<uint32_t, UInt32Allocator> UInt32ArrayPtr;
}

TEST(ArrayDataPtr, CreateNull) {
  UInt32Allocator allocator("TEST_1");
  UInt32ArrayPtr p(allocator);

  EXPECT_EQ("TEST_1", p.allocator().name());
  EXPECT_EQ("TEST_1", ((const UInt32ArrayPtr&)p).allocator().name());
  EXPECT_EQ((UInt32Array*)0, p.get());
  EXPECT_EQ((UInt32Array*)0, p.operator->());
  EXPECT_FALSE(bool(p));
}

TEST(ArrayDataPtr, CreateFromArray) {
  UInt32Allocator allocator("TEST_1");
  UInt32Allocator arrayAllocator("TEST_2");
  UInt32Allocator dimensionAllocator("TEST_3");
  UInt32Array::DimensionListType dimensions{ { 10, 20, 2 },
                                             dimensionAllocator };
  UInt32Array* data =
      new UInt32Array(UInt32Array::DimensionListType(dimensions),
		      arrayAllocator);
  UInt32ArrayPtr p(data, allocator);

  EXPECT_EQ("TEST_1", p.allocator().name());
  ASSERT_EQ(data, p.get());

  EXPECT_TRUE(bool(p));
  EXPECT_EQ(data, &(*p));
  EXPECT_EQ(data, p.operator->());
  
  EXPECT_EQ(1, p->refCnt());
  EXPECT_EQ(400, p->size());
  EXPECT_EQ("TEST_2", p->allocator().name());
  EXPECT_EQ("TEST_3", p->dimensions().allocator().name());
  EXPECT_EQ(dimensions, p->dimensions());
}

TEST(ArrayDataPtr, CreateFromDimensionList) {
  UInt32Allocator allocator("TEST_1");
  UInt32Allocator dimensionAllocator("TEST_2");
  UInt32Array::DimensionListType dimensions{ { 10, 20, 2 },
                                             dimensionAllocator };
  UInt32ArrayPtr p(UInt32ArrayPtr::newData(
		       UInt32Array::DimensionListType(dimensions),
		       allocator));

  EXPECT_EQ("TEST_1", p.allocator().name());
  ASSERT_NE((UInt32Array*)0, p.get());
  EXPECT_TRUE(bool(p));

  EXPECT_EQ(1, (*p).refCnt());
  EXPECT_EQ(400, (*p).size());
  EXPECT_EQ("TEST_1", (*p).allocator().name());
  EXPECT_EQ("TEST_2", (*p).dimensions().allocator().name());
  EXPECT_EQ(dimensions, (*p).dimensions());
}

TEST(ArrayDataPtr, Copy) {
  UInt32Allocator allocator("TEST_1");
  UInt32Allocator dimensionAllocator("TEST_2");
  UInt32Array::DimensionListType dimensions{ { 10, 20, 2 },
                                             dimensionAllocator };
  UInt32ArrayPtr p(UInt32ArrayPtr::newData(
		       UInt32Array::DimensionListType(dimensions),
		       allocator));
  
  EXPECT_EQ("TEST_1", p.allocator().name());
  ASSERT_NE((UInt32Array*)0, p.get());
  EXPECT_EQ(1, p->refCnt());
  EXPECT_EQ(400, p->size());
  EXPECT_EQ(dimensions, p->dimensions());

  {
    UInt32ArrayPtr copy(p);

    EXPECT_EQ("TEST_1", copy.allocator().name());
    ASSERT_EQ(p.get(), copy.get());
    EXPECT_EQ(2, copy->refCnt());

    EXPECT_EQ("TEST_1", p.allocator().name());
    EXPECT_EQ(2, p->refCnt());
    EXPECT_EQ(400, p->size());
    EXPECT_EQ(dimensions, p->dimensions());
  }

  EXPECT_EQ("TEST_1", p.allocator().name());
  ASSERT_NE((UInt32Array*)0, p.get());
  EXPECT_EQ(1, p->refCnt());
  EXPECT_EQ(400, p->size());
  EXPECT_EQ(dimensions, p->dimensions());  
}

TEST(ArrayDataPtr, Move) {
  UInt32Allocator allocator("TEST_1");
  UInt32Allocator dimensionAllocator("TEST_2");
  UInt32Array::DimensionListType dimensions{ { 10, 20, 2 },
                                             dimensionAllocator };
  UInt32ArrayPtr p(UInt32ArrayPtr::newData(
		       UInt32Array::DimensionListType(dimensions),
		       allocator));
  
  EXPECT_EQ("TEST_1", p.allocator().name());
  ASSERT_NE((UInt32Array*)0, p.get());
  EXPECT_EQ(1, p->refCnt());
  EXPECT_EQ(400, p->size());
  EXPECT_EQ(dimensions, p->dimensions());

  UInt32Array* data = p.get();

  {
    UInt32ArrayPtr moved(std::move(p));

    EXPECT_EQ("TEST_1", moved.allocator().name());
    ASSERT_EQ(data, moved.get());
    EXPECT_EQ(1, moved->refCnt());
    EXPECT_EQ(400, moved->size());
    EXPECT_EQ(dimensions, moved->dimensions());

    EXPECT_EQ((UInt32Array*)0, p.get());
  }

  EXPECT_EQ((UInt32Array*)0, p.get());
}

TEST(ArrayDataPtr, CopyAssign) {
  UInt32Allocator allocator("TEST_1");
  UInt32Allocator dimensionAllocator("TEST_2");
  UInt32Array::DimensionListType dimensions{ { 10, 20, 2 },
                                             dimensionAllocator };
  UInt32ArrayPtr p(UInt32ArrayPtr::newData(
		       UInt32Array::DimensionListType(dimensions),
		       allocator));
  
  EXPECT_EQ("TEST_1", p.allocator().name());
  ASSERT_NE((UInt32Array*)0, p.get());
  EXPECT_EQ(1, p->refCnt());
  EXPECT_EQ(400, p->size());
  EXPECT_EQ(dimensions, p->dimensions());

  {
    UInt32Allocator copyAllocator("TEST_3");
    UInt32Array::DimensionListType copyDimensions{ { 5, 7 }, UInt32Allocator()};
    UInt32ArrayPtr copy(
	UInt32ArrayPtr::newData(UInt32Array::DimensionListType(copyDimensions),
				copyAllocator)
    );

    EXPECT_EQ("TEST_3", copy.allocator().name());
    ASSERT_NE((UInt32Array*)0, copy.get());
    ASSERT_NE(p.get(), copy.get());
    EXPECT_EQ(1, copy->refCnt());
    EXPECT_EQ(35, copy->size());
    EXPECT_EQ(copyDimensions, copy->dimensions());

    copy = p;

    EXPECT_EQ("TEST_1", copy.allocator().name());
    ASSERT_NE((UInt32Array*)0, copy.get());
    ASSERT_EQ(p.get(), copy.get());
    EXPECT_EQ(2, copy->refCnt());
    EXPECT_EQ(400, copy->size());
    EXPECT_EQ(dimensions, copy->dimensions());

    EXPECT_EQ("TEST_1", p.allocator().name());
    EXPECT_EQ(2, p->refCnt());
    EXPECT_EQ(400, p->size());
    EXPECT_EQ(dimensions, p->dimensions());
  }

  EXPECT_EQ("TEST_1", p.allocator().name());
  ASSERT_NE((UInt32Array*)0, p.get());
  EXPECT_EQ(1, p->refCnt());
  EXPECT_EQ(400, p->size());
  EXPECT_EQ(dimensions, p->dimensions());  
}

TEST(ArrayDataPtr, MoveAssign) {
  UInt32Allocator allocator("TEST_1");
  UInt32Allocator dimensionAllocator("TEST_2");
  UInt32Array::DimensionListType dimensions{ { 10, 20, 2 },
                                             dimensionAllocator };
  UInt32ArrayPtr p(UInt32ArrayPtr::newData(
		       UInt32Array::DimensionListType(dimensions),
		       allocator));
  
  EXPECT_EQ("TEST_1", p.allocator().name());
  ASSERT_NE((UInt32Array*)0, p.get());
  EXPECT_EQ(1, p->refCnt());
  EXPECT_EQ(400, p->size());
  EXPECT_EQ(dimensions, p->dimensions());

  UInt32Array* data = p.get();
  {
    UInt32Allocator movedAllocator("TEST_3");
    UInt32Array::DimensionListType movedDimensions{ { 5, 7 },
	                                            UInt32Allocator() };
    UInt32ArrayPtr moved(
	UInt32ArrayPtr::newData(UInt32Array::DimensionListType(movedDimensions),
				movedAllocator)
    );

    EXPECT_EQ("TEST_3", moved.allocator().name());
    ASSERT_NE((UInt32Array*)0, moved.get());
    ASSERT_NE(data, moved.get());
    EXPECT_EQ(1, moved->refCnt());
    EXPECT_EQ(35, moved->size());
    EXPECT_EQ(movedDimensions, moved->dimensions());

    moved = std::move(p);

    EXPECT_EQ("TEST_1", moved.allocator().name());
    ASSERT_NE((UInt32Array*)0, moved.get());
    ASSERT_EQ(data, moved.get());
    EXPECT_EQ(1, moved->refCnt());
    EXPECT_EQ(400, moved->size());
    EXPECT_EQ(dimensions, moved->dimensions());

    EXPECT_EQ((UInt32Array*)0, p.get());
  }

  EXPECT_EQ((UInt32Array*)0, p.get());
}

TEST(ArrayDataPtr, EqualityAndInequality) {
  UInt32Allocator allocator("TEST_1");
  UInt32Allocator dimensionAllocator("TEST_2");
  UInt32Array::DimensionListType dimensions{ { 10, 20, 2 },
                                             dimensionAllocator };
  UInt32ArrayPtr p(UInt32ArrayPtr::newData(
		       UInt32Array::DimensionListType(dimensions),
		       allocator));
  UInt32ArrayPtr same(p);
  UInt32ArrayPtr different(UInt32ArrayPtr::newData(
			       UInt32Array::DimensionListType(dimensions),
			       allocator));

  EXPECT_TRUE(p == same);
  EXPECT_FALSE(p == different);
  EXPECT_FALSE(p != same);
  EXPECT_TRUE(p != different);
}

TEST(ArrayDataPtr, Release) {
  UInt32Allocator allocator("TEST_1");
  UInt32Allocator dimensionAllocator("TEST_2");
  UInt32Array::DimensionListType dimensions{ { 10, 20, 2 },
                                             dimensionAllocator };
  UInt32ArrayPtr p(UInt32ArrayPtr::newData(
		       UInt32Array::DimensionListType(dimensions),
		       allocator));
  UInt32Array* data = p.get();
  
  EXPECT_EQ("TEST_1", p.allocator().name());
  ASSERT_NE((UInt32Array*)0, p.get());
  EXPECT_EQ(1, p->refCnt());
  EXPECT_EQ(400, p->size());
  EXPECT_EQ(dimensions, p->dimensions());

  
  UInt32Array* released = p.release();
  EXPECT_EQ(data, released);
  EXPECT_FALSE(bool(p));
  EXPECT_EQ((UInt32Array*)0, p.get());
  EXPECT_EQ("TEST_1", p.allocator().name());

  released->~UInt32Array();
  p.allocator().deallocate(released, sizeof(UInt32Array));
}

