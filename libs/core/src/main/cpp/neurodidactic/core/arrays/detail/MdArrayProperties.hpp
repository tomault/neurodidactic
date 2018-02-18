#ifndef __NEURODIDACTIC__CORE__ARRAYS__DETAIL__MDARRAYPROPERTIES_HPP__
#define __NEURODIDACTIC__CORE__ARRAYS__DETAIL__MDARRAYPROPERTIES_HPP__

namespace neurodidactic {
  namespace core {
    namespace arrays {
      template <size_t ARRAY_ORDER, typename Field, typename Allocator>
      class MdArray;

      template <size_t ARRAY_ORDER, typename Field, typename Allocator>
      class MdArraySlice;
      
      namespace detail {

	template <size_t ARRAY_ORDER, typename Field, typename Allocator>
	struct MdArrayInnerProductType {
	  static_assert(ARRAY_ORDER > 0, "Zero-order arrays are not allowed");
	  typedef MdArray<ARRAY_ORDER - 1, Field, Allocator> type;
	};

	template <typename Field, typename Allocator>
	struct MdArrayInnerProductType<1, Field, Allocator> {
	  typedef Field type;
	};

	template <size_t ARRAY_ORDER, typename Field, typename Allocator>
	struct MdArraySliceType {
	  static_assert(ARRAY_ORDER > 0, "Zero-order arrays are not allowed");
	  typedef MdArraySlice<ARRAY_ORDER - 1, Field, Allocator> type;
	};

	template <typename Field, typename Allocator>
	struct MdArraySliceType<1, Field, Allocator> {
	  typedef Field type;
	};

      }
    }
  }
}
#endif

