#include <neurodidactic/core/layers/FullyConnectedLayer.hpp>

#include <neurodidactic/core/layers/Nonlinearities.hpp>
#include <neurodidactic/core/layers/FullyConnectedLayer.hpp>

#include <neurodidactic/core/arrays/MdArrays.hpp>
#include <neurodidactic/core/layers/Nonlinearities.hpp>
#include <neurodidactic/core/optimiziers/ForwardStateMap.hpp>

#include <pistis/testing/Allocators.hpp>
#include <neurodidactic/testing/MdArrayVerification.hpp>
#include <gtest/gtest.h>

using neurodidactic::testing::verifyMdArray;
using namespace neurodidactic::core::arrays;
using namespace neurodidactic::core::layers;
using namespace neurodidactic::core::optimiziers;

namespace nl = neurodidactic::core::layers::nonlinearities;

namespace {
  typedef pistis::testing::Allocator<float> NamedFloatAllocator;

  typedef MdArray<1, float> FloatVector;
  typedef MdArray<2, float> FloatMatrix;

  typedef MdArray<1, float, NamedFloatAllocator> FloatVectorWithNamedAllocator;
  typedef MdArray<2, float, NamedFloatAllocator> FloatMatrixWithNamedAllocator;
  
  typedef FullyConnectedLayer<float, nl::Identity> FullyConnectedIdLayer;
  typedef FullyConnectedLayer<float, nl::ReLU> FullyConnectedReLULayer;

  class NamedNonlinearity {
  public:
    NamedNonlinearity(const std::string& name): name_(name) { }

    const std::string& name() const { return name_; }

    template <typename Array>
    const Array& operator()(const Array& a) const { return a; }

    template <typename Array>
    typename Array::ArrayType gradient(const Array& a) const {
      typedef typename Array::FieldType Field;
      return typename Array::ArrayType(a.dimensions(), Field(1), a.allocator());
    }

  private:
    std::string name_;
  };
}

TEST(FullyConnectedLayerTests, CreateUninitialized) {
  const uint32_t LAYER_ID = 10;
  const size_t NUM_INPUTS = 3;
  const size_t NUM_OUTPUTS = 2;
  const std::string ALLOCATOR_NAME("TEST_ALLOCATOR");
  const std::string NONLINEARITY_NAME("TEST_NONLINEARITY");
  NamedFloatAllocator allocator(ALLOCATOR_NAME);
  NamedNonlinearity nonlinearity(NONLINEARITY_NAME);
  FullyConnectedLayer<float, NamedNonlinearity> layer(
      LAYER_ID, NUM_INPUTS, NUM_OUTPUTS, nonlinearity, allocator
  );

  EXPECT_EQ(LAYER_ID, layer.id());
  EXPECT_EQ(NUM_INPUTS, layer.numInputs());
  EXPECT_EQ(NUM_OUTPUTS, layer.numOutputs());
  EXPECT_EQ(NONLINEARITY_NAME, layer.nonlinearity().name());

  EXPECT_EQ(ALLOCATOR_NAME, layer.weights().allocator().name());
  EXPECT_EQ(2, layer.weights().dimensions().size());
  EXPECT_EQ(NUM_OUTPUTS, layer.weights().dimensions()[0]);
  EXPECT_EQ(NUM_INPUTS, layer.weights().dimensions()[1]);

  EXPECT_EQ(ALLOCATOR_NAME, layer.bias().allocator().name());
  EXPECT_EQ(1, layer.bias().dimensions().size());
  EXPECT_EQ(NUM_OUTPUTS, layer.bias().dimensions()[0]);
}

TEST(FullyConnectedLayerTests, CreateInitialized) {
  const uint32_t LAYER_ID = 10;
  const size_t NUM_INPUTS = 3;
  const size_t NUM_OUTPUTS = 2;
  const std::string WEIGHTS_ALLOCATOR_NAME("TEST_WEIGHTS_ALLOCATOR");
  const std::string BIAS_ALLOCATOR_NAME("TEST_BIAS_ALLOCATOR");
  const std::string NONLINEARITY_NAME("TEST_NONLINEARITY");
  const FloatMatrixWithNamedAllocator::DimensionListType
      WEIGHTS_DIMENSIONS{ 2, 3 };
  const FloatVectorWithNamedAllocator::DimensionListType BIAS_DIMENSIONS{ 2 };
  const std::vector<float> LAYER_WEIGHTS{ };
  const std::vecotr<float> LAYER_BIAS{ };
  NamedFloatAllocator weightsAllocator(WEIGHTS_ALLOCATOR_NAME);
  NamedFloatAllocator biasAllocator(BIAS_ALLOCATOR_NAME);
  NamedFloatAllocator layerAllocator(LAYER_ALLOCATOR_NAME);
  NamedNonlinearity nonlinearity(NONLINEARITY_NAME);
  FloatMatrixWithNamedAllocator weights(
      WEIGHTS_DIMENSIONS, LAYER_WEIGHTS.begin(), weightsAllocator
  );
  FloatVectorWithNamedAllocator bias(BIAS_DIMENSIONS, LAYER_BIAS.begin(),
				     biasAllocator);
  FullyConnectedLayer<float, NamedNonlinearity, NamedFloatAllocator>
      layer(LAYER_ID, weights, bias, nonlinearity, layerAllocator);

  EXPECT_EQ(LAYER_ID, layer.id());
  EXPECT_EQ(WEIGHTS_DIMENSIONS[1], layer.numInputs());
  EXPECT_EQ(WEIGHTS_DIMENSIONS[0], layer.numOutputs());
  EXPECT_EQ(NONLINEARITY_NAME, layer.nonlinearity.name());

  EXPECT_EQ(LAYER_ALLOCATOR_NAME, layer.weights().allocator().name());
  EXPECT_TRUE(verifyMdArray(WEIGHTS_DIMENSIONS, LAYER_WEIGHTS.begin(),
			    layer.weights()));
  
  EXPECT_EQ(LAYER_ALLOCATOR_NAME, layer.bias().allocator().name());
  EXPECT_TRUE(verifyMdArray(BIAS_DIMENSIONS, LAYER_BIAS.begin(),
			    layer.bias()));
}

// TEST(FullyConnectedLayerTests, CreateByMovingParameters) {

// }

// TEST(FullyConnectedLayerTests, ForwardComputation) {

// }

// TEST(FullyConnectedLayerTests, LossGradientComputation) {

// }

// TEST(FullyConnectedLayerTests, WeightGradientComputation) {

// }

// TEST(FullyConnectedLayerTests, BiasGradientComputation) {

// }

// TEST(FullyConnectedLayerTests, Backpropagation) {

// }

