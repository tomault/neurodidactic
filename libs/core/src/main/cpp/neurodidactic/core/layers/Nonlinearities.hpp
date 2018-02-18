#ifndef __NEURODIDACTIC__CORE__LAYERS__NONLINEARITIES_HPP__
#define __NEURODIDACTIC__CORE__LAYERS__NONLINEARITIES_HPP__

#include <neurodidactic/core/arrays/AnyMdArray.hpp>
#include <neurodidactic/core/arrays/detail/MklAdapter.hpp>
#include <algorithm>
#include <cmath>
#include <type_traits>

namespace neurodidactic {
  namespace core {
    namespace layers {

      namespace nonlinearities {

	struct Identity {
	  template <typename Array>
	  const Array& operator()(const Array& a) const { return a; }

	  template <typename Array>
	  typename Array::ArrayType gradient(const Array& a) const {
	    typedef typename Array::FieldType Field;
	    return typename Array::ArrayType(a.dimensions(), Field(1),
					     a.allocator());
	  }
	};

	struct ReLU {
	  template <typename Array,
		    typename Enabled =
		        typename std::enable_if<
		           arrays::IsMdArray<Array>::value,
		           typename Array::ArrayType
		        >::type
		   >
	  Enabled operator()(const Array& a) const {
	    typedef typename Array::FieldType Field;
	    return a.map([](Field x) { return x > Field(0) ? x : Field(0); });
	  }

	  template <typename Array,
		    typename Enabled =
		        typename std::enable_if<
		            arrays::IsMdArray<Array>::value,
		            typename Array::ArrayType
		        >::type
		   >
	  Array gradient(const Array& a) const {
	    typedef typename Array::FieldType Field;
	    return a.map([](Field x) {
		return x > Field(0) ? Field(1) : Field(0);
	    });
	  }
	};

	struct Sigmoid {
	  template <typename Array,
		    typename Enabled =
		        typename std::enable_if<
		            arrays::IsMdArray<Array>::value,
		            typename Array::ArrayType
		        >::type
		   >
	  Enabled operator()(const Array& a) const {
	    typedef typename Array::FieldType Field;
	    typedef arrays::detail::MklAdapter<Field, Field> MklAdapter;
	    
	    typename Array::ArrayType result = a.multiply(Field(-1));
	    MklAdapter::exp(result.size(), result.data(), result.data());
	    result.mapInPlace(
		[](Field x) { return Field(1) / (Field(1) + x); }
	    );
	    return std::move(result);
	  }

	  template <typename Array,
		    typename Enabled =
		        typename std::enable_if<
		            arrays::IsMdArray<Array>::value,
		            typename Array::ArrayType
		        >::type
		   >
	  Array gradient(const Array& a) const {
	    typedef typename Array::FieldType Field;
	    typedef arrays::detail::MklAdapter<Field, Field> MklAdapter;

	    typename Array::ArrayType result = a.multiply(Field(-1));
	    MklAdapter::exp(result.size(), result.data(), result.data());
	    result.mapInPlace([](Field x) {
		Field z = Field(1) / (Field(1) + x);
		return z * (1 - z);
	    });
	    return std::move(result);
	  }
	};

	struct TanH {
	  template <typename Array,
		    typename Enabled =
		        typename std::enable_if<
		            arrays::IsMdArray<Array>::value,
		            typename Array::ArrayType
		        >::type
		   >
	  Enabled operator()(const Array& a) const {
	    typedef typename Array::FieldType Field;
	    typedef arrays::detail::MklAdapter<Field, Field> MklAdapter;
	    
	    typename Array::ArrayType result = a.multiply(Field(-2));
	    MklAdapter::exp(result.size(), result.data(), result.data());
	    result.mapInPlace([](Field x) {
		return (Field(1) - x)/(Field(1) + x);
	    });
	    return std::move(result);
	  }

	  template <typename Array,
		    typename Enabled =
		        typename std::enable_if<
		            arrays::IsMdArray<Array>::value,
		            typename Array::ArrayType
		        >::type
		   >
	  Array gradient(const Array& a) const {
	    typedef typename Array::FieldType Field;
	    typedef arrays::detail::MklAdapter<Field, Field> MklAdapter;

	    typename Array::ArrayType result = a.multiply(Field(-2));
	    MklAdapter::exp(result.size(), result.data(), result.data());
	    result.mapInPlace([](Field x) {
		Field z = (Field(1) - x) / (Field(1) + x);
		return Field(1) - z * z;
	    });
	    return std::move(result);
	  }
	};
	
      }
      
    }
  }
}
#endif
