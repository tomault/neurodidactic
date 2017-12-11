#ifndef __NEURODIDACTIC__CORE__ARRAYS__DIMENSIONLIST_HPP__
#define __NEURODIDACTIC__CORE__ARRAYS__DIMENSIONLIST_HPP__

#include <pistis/util/ImmutableList.hpp>
#include <ostream>
#include <type_traits>
#include <stdint.h>

namespace neurodidactic {
  namespace core {
    namespace arrays {

      template <typename Allocator = std::allocator<uint32_t> >
      class DimensionList :
	  public pistis::util::ImmutableList<uint32_t, Allocator> {
      private:
	typedef pistis::util::ImmutableList<uint32_t, Allocator>
	        ParentType;
	
      public:
	template <typename Iterator,
		  typename Enabler =
		      typename std::enable_if<
		          !std::is_integral<Iterator>::value, int
		      >::type
		  >
	DimensionList(uint32_t numDimensions, Iterator dimensions,
		      const Allocator& allocator = Allocator(),
		      Enabler = 0):
	    ParentType(numDimensions, dimensions, allocator) {
	}

	template <typename Iterator,
		  typename Enabler =
		      typename std::enable_if<
		          !std::is_integral<Iterator>::value, int
		      >::type
		  >
	DimensionList(Iterator startOfDimensions,
		      Iterator endOfDimensions,
		      const Allocator& allocator = Allocator(),
		      Enabler = 0):
	    ParentType(startOfDimensions, endOfDimensions, allocator) {
	}
	
	DimensionList(const std::initializer_list<uint32_t>& dimensions,
		      const Allocator& allocator = Allocator()):
	    ParentType(dimensions, allocator) {
	}
	
	DimensionList(const DimensionList<Allocator>& other) = default;

	DimensionList(const ParentType& other):
	    ParentType(other) {
	}

	DimensionList replaceLast(uint32_t v) const {
	  return this->replace(this->size() - 1, v);
	}
	
	uint64_t numElements() const {
	  static const auto PRODUCT = [](uint32_t product, uint32_t value) {
	    return product * value;
	  };
	  
	  return this->empty() ? 0 : this->reduce(PRODUCT, 1);
	}
      };

      template <typename Allocator>
      inline std::ostream& operator<<(
	  std::ostream& output, const DimensionList<Allocator>& list
      ) {
	output << "[";
	for (auto i = list.begin(); i != list.end(); ++i) {
	  if (i != list.begin()) {
	    output << ", " << *i;
	  } else {
	    output << *i;
	  }
	}
	return output << " ]";
      }
      
    }
  }
}

#endif

