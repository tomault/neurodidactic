#ifndef __NEURODIDACTIC__CORE__LAYERS__FULLYCONNECTED_HPP__
#define __NEURODIDACTIC__CORE__LAYERS__FULLYCONNECTED_HPP__

#include <neurodidactic/core/arrays/MdArray.hpp>

namespace neurodidactic {
  namespace core {
    namespace layers {

      template <typename Field,
		typenmae Nonlinearity,
		typename Allocator = MklAllocator<Field, 64> >
      class FullyConnectedLayer {
      public:
	typedef arrays::MdArray<1, Field, Allocator> InputType;
	typedef arrays::MdArray<1, Field, Allocator> OutputType;
	typedef arrays::MdArray<2, Field, Allocator> WeightMatrixType;
	typedef arrays::MdArray<1, Field, Allocator> BiasVectorType;
	
      public:
	FullyConnectedLayer(uint32_t id,
			    size_t numInputs, size_t numOutputs,
			    const Nonlinearity& nonlinearity = Nonlinearity(),
			    const Allocator& allocator = Allocator()):
	    id_(id), weights_({ numOutputs, numInputs }, allocator),
	    bias_({ numOutputs }, allocator), f_(nonlinearity) {
	}
	
	FullyConnectedLayer(uint32_t id,
			    const WeightMatrixType& weights,
			    const BiasVectorType& bias,
			    const Nonlinearity& nonlinearity = Nonlinearity(),
			    const Allocator& allocator = Allocator()):
	    id_(id), weights_(weights.dimensions(), weights.data(), allocator),
	    bias_(bias.dimensions(), bias.data(), allocator), f_(nonlinearity) {
	  // TODO: Check dimensions of weights and bias
	}

	FullyConnectedLayer(uint32_t id,
			    WeightMatrixType&& weights,
			    BiasVectorType&& bias,
			    const Nonlinearity& nonlinearity = Nonlinearity()):
	    id_(id), weights_(std::move(weights)), bias(std::move(bias_)),
	    f_(nonlinearity) {
	  // TODO: Check dimensions of weights and bias
	}
	
	FullyConnectedLayer(const FullyConnectedLayer&) = default;
	FullyConnectedLayer(FullyConnectedLayer&&) = default;
	
	uint32_t id() const { return id_; }
	size_t numInputs() const { return weights_.dimensions()[1]; }
	size_t numOutputs() const { return weights_.dimensions()[0]; }
	const Nonlinearity& nonlinearity() const { return f_; }
	const WeightMatrixType& weights() const { return weights_; }
	WeightMatrixType& weights() { return weights_; }
	const BiasVectorType& bias() const { return bias_; }
	BiasVectorType& bias() { return bias_; }

	OutputType forward(const InputType& input) const {
	  return f_(weights_.innerProduct(input).addInPlace(bias_));
	}

	template <typename ForwardState>
	OutputType forward(const InputType& input,
			   ForwardState& forwardState) const {
	  OutputType activations(
	      weights_.innerProduct(input).addInPlace(bias_)
	  );
	  forwardState.setInputs(id(), input);
	  forwardState.setActivations(id(), activations);
	  return f_(activations);
	}

	template <typename ForwardState>
	InputType lossGradient(const OutputType& lossGradient,
			       const ForwardState& forwardState) const {
	  return weights._transposeInnerProduct(
	      f_.gradient(forwardState.activations(id()).cast<1>())
	        .multiplyInPlace(lossGradient)
	  );
	}

	template <typename ForwardState>
	WeightMatrixType weightGradient(
	    const OutputType& lossGradient,
	    const ForwardState& forwardState
	) const {
	  return f_.gradient(forwardState.activations(id()).cast<1>())
	           .multiplyInPlace(lossGradient)
	           .outerProduct(forwardState.inputs(id()).cast<1>());
	}

	template <typename ForwardState>
	BiasVectorType biasGradient(const OutputType& lossGradient,
				    const ForwardState& forwardState) {
	  return f_.gradient(forwardState.activations(id()).cast<1>())
	           .multiplyInPlace(lossGradient);
	}

	template <typename ForwardState, typename Optimizer>
	InputType backward(const OutputType& lossGradient,
			   const ForwardState& forwardState,
			   Optimizer& optimizer) {
	  static const uint32_t WEIGHTS = 0;
	  static const uint32_t BIAS = 1;
	  
	  auto weightedLoss =
	      f_.gradient(forwardState.activations(id()).cast<1>())
	        .multiplyInPlace(lossGradient);
	  optimizer.update(id(), WEIGHTS, weights_,
	      weightedLoss.outerProduct(forwardState.inputs(id()).cast<1>())
	  );
	  optimizer.update(id(), BIAS, bias_, weightedLoss);
	  return weights_.transposeInnerProduct(weightedLoss);
	}

	FullyConnectedLayer& operator=(const FullyConnectedLayer&) = default;
	FullyConnectedLayer& operator=(FullyConnectedLayer&&) = default;
	
      private:
	uint32_t id_;
	WeightMatrixType weights_;
	BiasVectorType bias_;
	Nonlinearity f_;
      };
      
    }
  }
}

#endif
