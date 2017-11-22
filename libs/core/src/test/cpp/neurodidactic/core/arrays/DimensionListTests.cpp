#include <neurodidactic/core/arrays/DimensionList.hpp>
#include <pistis/testing/Allocator.hpp>
#include <gtest/gtest.h>

using namespace neurodidactic::core::arrays;
namespace pt = pistis::testing;

TEST(DimensionListTests, CreateFromData) {
  const uint32_t[] DATA = { 5, 16, 11, 4, 2 };
  const uint32_t NUM_DATA = sizeof(DATA)/sizeof(uint32_t);
  const pt::Allocator<uint32_t> allocator("TEST_1");
  DimensionList< pt::Allocator<uint32_t> > l(NUM_DATA, DATA, allocator);

  EXPECT_EQ(NUM_DATA, l.size());
  EXPECT_EQ(NUM_DATA, l.end() - l.begin());
  EXPECT_EQ("TEST_1", l.allocator().name());
  EXPECT_EQ("TEST_1",
	    ((const DimensionList< pt::Allocator<uint32_t> >&)l).allocator()
	                                                        .name()).

  std::vector<uint32_t> truth(DATA, DATA + NUM_DATA);
  std::vector<uint32_t> dimensions(l.begin(), l.end());
  EXPECT_EQ(truth, dimensions);
}

TEST(DimensionListTests, CreateFromInitializerList) {
  const pt::Allocator<uint32_t> allocator("TEST_1");
  DimensionList< pt::Allocator<uint32_t> > l{{ 5, 3, 2, 7 }, allocator};
  const DimensionList< pt::Allocator<uint32_t> >& cl = l;

  EXPECT_EQ(4, l.size());
  EXPECT_EQ(4, l.end() - l.begin());
  EXPECT_EQ("TEST_1", l.allocator().name());
  EXPECT_EQ("TEST_1", cl.allocator().name());

  std::vector<uint32_t> truth{ 5, 3, 2, 7 };
  std::vector<uint32_t> dimensions(l.begin(), l.end());
  EXPECT_EQ(truth, dimensions);
}

TEST(DimensionListTests, Copy) {
  const std::vector<uint32_t> TRUTH{ 5, 3, 2, 7 };
  const pt::Allocator<uint32_t> allocator("TEST_1");
  DimensionList< pt::Allocator<uint32_t> > l(TRUTH.size(), TRUTH.begin(),
					     allocator);

  EXPECT_EQ(TRUTH.size(), l.size());
  EXPECT_EQ("TEST_1", l.allocator().name());
  std::vector<uint32_t> dimensions(l.begin(), l.end());
  EXPECT_EQ(TRUTH, dimensions);

  {
    DimensionList< pt::Alllocator<uint32_t> > copy(l);

    EXPECT_EQ(TRUTH.size(), copy.size());
    EXPECT_EQ("TRUTH_1", copy.allocator().name());

    std::vector<uint32_t> copyDimensions(copy.begin(), copy.end());
    EXPECT_EQ(TRUTH, copyDimensions);
  }

  EXPECT_EQ(TRUTH.size(), l.size());
  EXPECT_EQ("TEST_1", l.allocator().name());
  dimensions = std::vector<uint32_t>(l.begin(), l.end());
  EXPECT_EQ(TRUTH, dimensions);
}

TEST(DimensionListTests, CopyAssignment) {
  const std::vector<uint32_t> TRUTH{ 5, 3, 2, 7 };
  const pt::Allocator<uint32_t> allocator("TEST_1");
  DimensionList< pt::Allocator<uint32_t> > l(TRUTH.size(), TRUTH.begin(),
					     allocator);

  EXPECT_EQ(TRUTH.size(), l.size());
  EXPECT_EQ("TEST_1", l.allocator().name());
  std::vector<uint32_t> dimensions(l.begin(), l.end());
  EXPECT_EQ(TRUTH, dimensions);

  {
    const std::vector<uint32_t> COPY_INITIAL{ 3, 2, 4 };
    const pt::Allocator<uint32_t> copyAllocator("TEST_2");
    DimensionList< pt::Allocator<uint32_t> > copy(COPY_INITIAL.size(),
						  COPY_INITIAL.begin(),
						  copyAllocator);
    std::vector<uint32_t> copyDimensions(copy.begin(), copy.end());

    EXPECT_EQ("TEST_2", copy.allocator().name());
    EXPECT_EQ(COPY_INITIAL, copyDimensions);

    copy = l;
    copyDimensions = std::vector<uint32_t>(copy.begin(), copy.end());

    EXPECT_EQ("TEST_1", copy.allocator().name());
    EXPECT_EQ(TRUTH.size(), copy.size());
    EXPECT_EQ(TRUTH, copyDimensions);
  }

  dimensions = std::vector<uint32_t>(l.begin(), l.end());
  EXPECT_EQ("TEST_1", l.allocator().name());
  EXPECT_EQ(TRUTH, dimensions);  
}

TEST(DimensionListTests, NumElements) {
  pt::Allocator<uint32_t> allocator("TEST_1");
  DimensionList< pt::Allocator<uint32_t> > l{ { 2, 7, 4, 5 }, allocator };

  EXPECT_EQ(280, l.numElements());
}

TEST(DimensionListTests, RandomAccess) {
  pt::Allocator<uint32_t> allocator("TEST_1");
  DimensionList< pt::Allocator<uint32_t> > l{ { 2, 7, 4, 5 }, allocator };

  ASSERT_EQ(4, l.size());
  EXPECT_EQ(2, l[0]);
  EXPECT_EQ(7, l[1]);
  EXPECT_EQ(4, l[2]);
  EXPECT_EQ(5, l[3]);
}

