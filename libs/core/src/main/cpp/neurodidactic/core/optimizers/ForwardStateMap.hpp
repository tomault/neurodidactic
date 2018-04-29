#ifndef __NEURODIDACTIC__CORE__OPTIMIZATION__FORWARDSTATEMAP_HPP__
#define __NEURODIDACTIC__CORE__OPTIMIZATION__FORWARDSTATEMAP_HPP__

#include <neurodidactic/core/arrays/AnyMdArray.hpp>
#include <neurodidactic/core/arrays/AnyMdArrayRef.hpp>
#include <pistis/exceptions/NoSuchItem.hpp>
#include <algorithm>
#include <iterator>
#include <unordered_map>
#include <utility>

namespace neurodidactic {
  namespace core {
    namespace optimizers {

      template <typename Field, typename Allocator>
      class ForwardStateMap {
      public:
	typedef arrays::AnyMdArrayRef<Field, Allocator> ArrayRefType;
	
      public:
	ForwardStateMap(): inputMap_(), activationMap_() { }
	ForwardStateMap(const ForwardStateMap&) = default;
	ForwardStateMap(ForwardStateMap&&) = default;

	size_t numInputs() const { return inputMap_.size(); }
	size_t numActivations() const { return activationMap_.size(); }
	std::vector<size_t> inputIds() const {
	  return extractIds_(inputMap_);
	}
	
	std::vector<size_t> activationIds() const {
	  return extractIds_(activationMap_);
	}
	
	const ArrayRefType& inputs(size_t id) const {
	  return retrieve_(inputMap_, id);
	}

	const ArrayRefType& activations(size_t id) const {
	  return retrieve_(activationMap_, id);
	}
	
	template <typename Array>
	void setInputs(size_t id, const arrays::AnyMdArray<Array>& inputs) {
	  set_(inputMap_, id, inputs.self().ref());
	}

	template <typename Array>
	void setActivations(size_t id,
			    const arrays::AnyMdArray<Array>& activations) {
	  set_(activationMap_, id, activations.self().ref());
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

	static const ArrayRefType& retrieve_(const IdToArrayMap& data,
					     size_t id) {
	  auto i = data.find(id);
	  if (i == data.end()) {
	    std::ostringstream msg;
	    msg << "Layer " << id;
	    throw pistis::exceptions::NoSuchItem(msg.str(), PISTIS_EX_HERE);
	  }
	  return i->second;
	}

	template <typename RefType>
	static void set_(IdToArrayMap& data, size_t id,
			 const RefType& ref) {
	  auto i = data.find(id);
	  if (i != data.end()) {
	    i->second = ArrayRefType(ref);
	  } else {
	    data.insert(std::make_pair(id, ArrayRefType(ref)));
	  }
	}

	static std::vector<size_t> extractIds_(const IdToArrayMap& data) {
	  std::vector<size_t> ids;

	  ids.reserve(data.size());
	  std::transform(data.begin(), data.end(), std::back_inserter(ids),
			 [](const typename IdToArrayMap::value_type& x) {
			     return x.first;
			 });
	  return ids;
	}
	
      };
    }
  }
}
#endif
