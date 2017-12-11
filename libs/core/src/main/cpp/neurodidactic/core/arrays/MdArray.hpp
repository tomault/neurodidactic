#ifndef __NEURODIDACTIC__CORE__ARRAYS__MDARRAY_HPP__
#define __NEURODIDACTIC__CORE__ARRAYS__MDARRAY_HPP__

#include <neurodidactic/core/arrays/DimensionList.hpp>
#include <neurodidactic/core/arrays/MklAllocator.hpp>
#include <neurodidactic/core/arrays/MdArraySlice.hpp>
#include <neurodidactic/core/arrays/detail/ArrayDataPtr.hpp>
#include <neurodidactic/core/arrays/detail/MdArrayBase.hpp>
#include <neurodidactic/core/arrays/detail/MdArrayProperties.hpp>
#include <algorithm>
#include <memory>
#include <type_traits>

namespace neurodidactic {
  namespace core {
    namespace arrays {
      template <size_t ARRAY_ORDER, typename Field, typename Allocator>
      class MdArray;
      
      template <size_t ARRAY_ORDER, typename Field,
		typename Allocator = MklAllocator<Field, 64> >
      class MdArray :
	  public detail::MdArrayBase<
	      MdArray<ARRAY_ORDER, Field, Allocator>,
	      typename detail::MdArraySliceType<
	          ARRAY_ORDER, Field, Allocator
	      >::type,
	      typename detail::MdArrayInnerProductType<
		  ARRAY_ORDER, Field, Allocator
	      >::type,
	      ARRAY_ORDER, Field, Allocator
	  >
      {
      public:
	typedef typename detail::MdArraySliceType<
	           ARRAY_ORDER, Field, Allocator
	        >::type SliceType;
	
	typedef typename detail::MdArrayInnerProductType<
		    ARRAY_ORDER, Field, Allocator
	        >::type
	        InnerProductType;
	
      protected:
	typedef detail::MdArrayBase<MdArray<ARRAY_ORDER, Field, Allocator>,
				    SliceType, InnerProductType,
				    ARRAY_ORDER, Field, Allocator>
	        BaseType;
	typedef detail::ArrayDataPtr<Field, Allocator> DataPtr;

      public:
	typedef MdArray<ARRAY_ORDER, Field, Allocator> ThisType;
	typedef typename BaseType::AllocatorType AllocatorType;
	typedef typename BaseType::DimensionListType DimensionListType;	
	
      public:
	MdArray(const DimensionListType& dimensions,
		const AllocatorType& allocator = AllocatorType()):
	    p_(DataPtr::newData(DimensionListType(dimensions), allocator)) {
	}

	MdArray(const DimensionListType& dimensions, Field value,
		const AllocatorType& allocator = AllocatorType()):
	    p_(DataPtr::newData(DimensionListType(dimensions), allocator)) {
	  std::fill(p_->data(), p_->end(), value);
	}
	
	template <typename Iterator,
		  typename Enabler =
		      typename std::enable_if<
		          !std::is_arithmetic<Iterator>::value,
		          int
		      >::type>
	MdArray(const DimensionListType& dimensions, Iterator arrayData,
		const AllocatorType& allocator = AllocatorType(),
		Enabler = 0):
	    p_(DataPtr::newData(DimensionListType(dimensions), allocator)) {
	  std::copy_n(arrayData, p_->size(), p_->data());
	}

	MdArray(const DimensionListType& dimensions,
		const std::initializer_list<Field>& arrayData,
		const AllocatorType& allocator = AllocatorType()):
	    p_(DataPtr::newData(DimensionListType(dimensions), allocator)) {
	  std::copy(arrayData.begin(), arrayData.end(), p_->data());
	}

	MdArray(const MdArray& other):
	    p_(DataPtr::newData(DimensionListType(other.dimensions()),
				other.allocator())) {
	  std::copy(other.begin(), other.end(), p_->data());
	}

	MdArray(MdArray&& other) = default;

	template <typename OtherArray,
		  typename Enabler =
		      typename std::enable_if<
		          IsMdArray<OtherArray>::value &&
		              (OtherArray::ORDER == ARRAY_ORDER),
			  int
		      >::type
		 >
	explicit MdArray(const OtherArray& other, Enabler = 0):
	    p_(DataPtr::newData(DimensionListType(other.dimensions()),
				other.allocator())) {
	  std::copy(other.begin(), other.end(), p_->data());
	}

	MdArray& operator=(const MdArray& other) {
	  if (p_ != other.p_) {
	    p_ = DataPtr::newData(DimensionListType(other.dimensions()),
				  other.allocator());
	    std::copy(other.begin(), other.end(), p_->data());
	  }
	  return *this;
	}
	
	MdArray& operator=(MdArray&&) = default;
	
	template <typename OtherArray,
		  typename Enabler =
		      typename std::enable_if<
		          IsMdArray<OtherArray>::value &&
		              (OtherArray::ORDER == ARRAY_ORDER),
			  MdArray
		      >::type
		 >
	Enabler& operator=(const OtherArray& other) {
	  p_ = DataPtr::newData(DimensionListType(other.dimensions()),
				this->allocator());
	  std::copy(other.begin(), other.end(), p_->data());
	  return *this;
	}

      protected:
	const Allocator& allocator_() const { return p_->allocator(); }
	Allocator& allocator_() { return p_->allocator(); }
	size_t size_() const { return p_->size(); }
	size_t leadingDimension_() const { return p_->leadingDimension(); }
	const DimensionListType& dimensions_() const {
	  return p_->dimensions();
	}

	const Field* data_() const { return p_->data(); }
	Field* data_() { return p_->data(); }
	const Field* end_() const { return p_->end(); }
	Field* end_() { return p_->end(); }

	template <typename Index,
		  typename Enabled =
		      typename std::enable_if<
		          std::is_unsigned<Index>::value && (ARRAY_ORDER > 1),
		          const SliceType
	              >::type
	          >
	Enabled slice_(Index n) const {
	  Field* data = const_cast<Field*>(p_->data());
	  return SliceType(p_, p_->dimensions().sublist(1),
			   data + n * p_->leadingDimension());
	}

	template <typename Index,
		  typename Enabled =
		      typename std::enable_if<
		          std::is_unsigned<Index>::value && (ARRAY_ORDER > 1),
		          SliceType
	              >::type
	          >
	Enabled slice_(Index n) {
	  return SliceType(p_, DimensionListType(p_->dimensions().sublist(1)),
			   p_->data() + n * p_->leadingDimension());
	}

      private:
	DataPtr p_;

	friend class detail::MdArrayCommon<
	    MdArray<ARRAY_ORDER, Field, Allocator>,
	    SliceType, InnerProductType,
	    ARRAY_ORDER, Field, Allocator
	>;
	friend class detail::MdArrayBase<
	    MdArray<ARRAY_ORDER, Field, Allocator>,
	    SliceType, InnerProductType,
	    ARRAY_ORDER, Field, Allocator
	>;
      };
      
    }
  }
}
#endif

