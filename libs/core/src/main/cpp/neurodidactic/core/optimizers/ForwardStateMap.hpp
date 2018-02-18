#ifndef __NEURODIDACTIC__CORE__OPTIMIZATION__FORWARDSTATEMAP_HPP__
#define __NEURODIDACTIC__CORE__OPTIMIZATION__FORWARDSTATEMAP_HPP__

#include <neurodidactic/core/arrays/AnyMdArray.hpp>
#include <neurodidactic/core/arrays/AnyMdArrayRef.hpp>
#include <algorithm>
#include <iterator>
#include <unordered_map>

namespace neurodidactic {
  namespace core {
    namespace optimization {

      template <typename Field, typename Allocator>
      class ForwardStateMap {
      public:
	typedef AnyMdArrayRef<Field, Allocator> ArrayRefType;
	
      public:
	ForwardStateMap(): inputs_(), activations_() { }
	ForwardStateMap(const ForwardStateMap&) = default;
	ForwardStateMap(ForwardStateMap&&) = default;

	size_t numInputs() const { return inputs_.size(); }
	size_t numActivations() const { return activations_.size(); }
	std::vector<size_t> inputIds() const {
	  return extractIds_(inputs_);
	}
	
	std::vector<size_t> activationIds() const {
	  return extractIds_(activations_);
	}
	
	const ArrayRefType& inputs(size_t id) const {
	  return retrieve_(inputMap_, id);
	}

	const ArrayRefType& activations(size_t id) const {
	  return retrieve_(activationMap_, id);
	}
	
	template <typename Array>
	void setInputs(size_t id, const arrays::AnyMdArray<Array>& inputs) {
	  inputMap_.insert(std::make_pair(id, inputs.self().ref()));	  
	}

	template <typename Array>
	void setActivations(size_t id,
			    const arrays::AnyMdArray<Array>& activations) {
	  activationMap_.insert(std::make_pair(id, activations.self().ref()));
	}

	void reset() {
	  inputMap_.clear();
	  activationMap_.clear();
	}

	ForwardStateMap& operator=(const ForwardStateMap&) = default;
	ForwardStateMap& operator=(ForwardStateMap&&) = default;

      private:
	typedef std::unordered_map<size_t, ArrayRefType> IdToArrayMap;

      private:
	IdToArrayMap inputMap_;
	IdToArrayMap activationMap_;

	static ArrayRefType retrieve_(const IdToArrayMap& data, size_t id) {
	  auto i = data.find(id);
	  if (i == data.end()) {
	    std::ostringstream msg;
	    msg << "Layer " << id;
	    throw pistis::exceptions::NoSuchItem(msg.str(), PISTIS_EX_HERE);
	  }
	  return i->second;
	}

	static std::vector<size_t> extractIds_(const IdToArrayMap& data) {
	  std::vector<size_t> ids;

	  ids.reserve(data.size());
	  std::transform(data.begin(), data.end(), std::back_inserter(ids),
			 [](const IdToArrayMap::value_type& x) {
			     return x.first;
			 });
	}
	
      };
    }
  }
}
#endif
