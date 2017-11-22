#ifndef __NEURODIDACTIC__CORE__ARRAYS__DIMENSIONLIST_HPP__
#define __NEURODIDACTIC__CORE__ARRAYS__DIMENSIONLIST_HPP__

#include <allocator>
#include <memory>
#include <stdint.h>

namespace neurodidactic {
  namespace core {
    namespace arrays {

      template <typename Allocator = std::allocator<uint32_t> >
      class DimensionList : Allocator {
      public:
	typedef Allocator AllocatorType;

      public:
	template <typename Iterator>
	DimensionList(uint32_t numDimensions, Iterator dimensions,
		      const Allocator& allocator):
	    Allocator(allocator), numDimensions_(numDimensions),
	    dimensions_(allocator.allocate(numDimensions)) {
	  std::copy_n(dimensions, numDimensions, dimensions_.get());
	}

	DimensionList(const std::initializer_list<uint32_t>& dimensions,
		      const Allocator& allocator):
	    Allocator(allocator), numDimensions_(dimensions.size()),
	    dimensions_(allocator.allocate(numDimensions)) {
	  std::copy(dimensions.begin(), dimensions.end(), dimensions_.get());
	}
	
	DimensionList(const DimensionList<Allocator>& other):
	    Allocator(other.allocator()), numDimensions_(other.size()),
	    dimensions_(other.dimensions_) {
	  // Intentionally left blank
	}

	const Allocator& allocator() const {
	  return static_cast<const Allocator&>(*this);
	}
	Allocator& allocator() { return static_cast<Allocator&>(*this); }
	
	uint32_t size() const { return numDimensions_; }
	const uint32_t* begin() const { return dimensions_.get(); }
	const uint32_t* end() const { return dimensions_.get() + size(); }
	uint64_t numElements() const {
	  if (!numElements) {
	    return 0;
	  } else {
	    uint64_t n = dimensions_[0];
	    const uint64_t* const endOfDimensions = end();
	    for (const uint64_t* p = begin() + 1; p != endOfDimensions; ++p) {
	      n *= *p;
	    }
	    return n;
	  }
	}
	
	DimensionList<Allocator>& operator=(
	    const DimensionList<Allocator>& other
	) {
	  if (this != &other) {
	    allocator() = other.allocator();
	    numDimensions_ = other.numDimensions_;
	    dimensions_ = other.dimensions_;
	  }
	  return *this;
	}
	
	uint32_t operator[](uint32_t n) const { return dimensions_[n]; }
	
      private:
	uint32_t numDimensions_;
	std::shared_ptr<uint32_t[]> dimensions_;
      };

    }
  }
}

#endif

