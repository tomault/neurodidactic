#ifndef __NEURODIDACTIC__CORE__ARRAYS__DIMENSIONLIST_HPP__
#define __NEURODIDACTIC__CORE__ARRAYS__DIMENSIONLIST_HPP__

#include <pistis/util/ImmutableList.hpp>
#include <type_traits>
#include <stdint.h>

namespace neurodidactic {
  namespace core {
    namespace arrays {

      template <typename Allocator = std::allocator<uint32_t> >
      class DimensionList : public ImmutabeList<uint32_t, Allocator> {
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
	    ImmutableList<uint32_t, Allocator>(numDimensions, dimensions,
					       allocator) {
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
	    ImmutableList<uint32_t, Allocator>(startOfDimensions,
					       endOfDimensions
					       allocator) {
	}
	
	DimensionList(const std::initializer_list<uint32_t>& dimensions,
		      const Allocator& allocator):
	    ImmutableList<uint32_t, Allocator>(dimensions, allocator) {
	}
	
	DimensionList(const DimensionList<Allocator>& other) = default;

	uint64_t numElements() const {
	  static const auto PRODUCT = [](uint32_t product, uint32_t value) {
	    return product * value;
	  };
	  
	  return empty() ? 0 : reduce(PRODUCT, 1);
	}
	
      private:
      };

    }
  }
}

#endif

