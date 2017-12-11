#ifndef __NEURODIDACTIC__CORE__ARRAYS__ANYMDARRAY_HPP__
#define __NEURODIDACTIC__CORE__ARRAYS__ANYMDARRAY_HPP__

#include <type_traits>

namespace neurodidactic {
  namespace core {
    namespace arrays {

      template <typename DerivedArray>
      class AnyMdArray {
      public:
	const DerivedArray& self() const {
	  return static_cast<const DerivedArray&>(*this);
	}
	DerivedArray& self() { return static_cast<DerivedArray&>(*this); }
      };

      template <typename T>
      struct IsMdArray : public std::is_base_of< AnyMdArray<T>, T > { };

      template <typename T>
      struct IsMdArray< AnyMdArray<T> > : public std::true_type { };
      
    }
  }
}
#endif
