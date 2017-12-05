#include <neurodidactic/core/arrays/DimensionList.hpp>
#include <pistis/testing/Allocator.hpp>
#include <gtest/gtest.h>

using namespace neurodidactic::core::arrays;
namespace pt = pistis::testing;

TEST(DimensionListTests, CreateFromData) {
  const uint32_t DATA[] = { 5, 16, 11, 4, 2 };
  const uint32_t NUM_DATA = sizeof(DATA)/sizeof(uint32_t);
  const pt::Allocator<uint32_t> allocator("TEST_1");
  DimensionList< pt::Allocator<uint32_t> > l(NUM_DATA, DATA, allocator);

  EXPECT_EQ("TEST_1", l.allocator().name());

  std::vector<uint32_t> truth(DATA, DATA + NUM_DATA);
  std::vector<uint32_t> dimensions(l.begin(), l.end());
  EXPECT_EQ(truth, dimensions);
}

TEST(DimensionListTests, CreateFromInitializerList) {
  const pt::Allocator<uint32_t> allocator("TEST_1");
  DimensionList< pt::Allocator<uint32_t> > l{{ 5, 3, 2, 7 }, allocator};

  EXPECT_EQ("TEST_1", l.allocator().name());

  std::vector<uint32_t> truth{ 5, 3, 2, 7 };
  std::vector<uint32_t> dimensions(l.begin(), l.end());
  EXPECT_EQ(truth, dimensions);
}

TEST(DimensionListTests, NumElements) {
  pt::Allocator<uint32_t> allocator("TEST_1");
  DimensionList< pt::Allocator<uint32_t> > l{ { 2, 7, 4, 5 }, allocator };

  EXPECT_EQ(280, l.numElements());
}
