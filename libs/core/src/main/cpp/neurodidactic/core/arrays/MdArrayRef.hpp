#ifndef __NEURODIDACTIC__CORE__ARRAYS__MDARRAYREF_HPP__
#define __NEURODIDACTIC__CORE__ARRAYS__MDARRAYREF_HPP__

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

      template <typename Field, typename Allocator>
      class AnyMdArrayRef;

      template <size_t ARRAY_ORDER, typename Field,
		typename Allocator = MklAllocator<Field, 64> >
      class MdArrayRef :
	  public detail::MdArrayBase<
	      MdArrayRef<ARRAY_ORDER, Field, Allocator>,
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
	typedef MdArray<ARRAY_ORDER, Field, Allocator> ArrayType;
	
	typedef typename detail::MdArraySliceType<
	            ARRAY_ORDER, Field, Allocator
	        >::type SliceType;

	typedef MdArrayRef<ARRAY_ORDER, Field, Allocator> RefType;

	typedef typename detail::MdArrayInnerProductType<
	            ARRAY_ORDER, Field, Allocator
	        >::type InnerProductType;

      protected:
	typedef detail::MdArrayBase<MdArrayRef<ARRAY_ORDER, Field, Allocator>,
				    MdArray<ARRAY_ORDER, Field, Allocator>,
				    SliceType, InnerProductType,
				    ARRAY_ORDER, Field, Allocator>
	        BaseType;
	typedef detail::ArrayDataPtr<Field, Allocator> DataPtr;

      public:
	typedef MdArray<ARRAY_ORDER, Field, Allocator> ThisType;
	typedef typename BaseType::AllocatorType AllocatorType;
	typedef typename BaseType::DimensionListType DimensionListType;

      public:
	MdArrayRef(const DataPtr& base) : p_(base) { }
	MdArrayRef(const MdArrayRef&) = default;
	MdArrayRef(MdArrayRef&&) = default;

	RefType ref() const { return *this; }
	bool refersTo(const ArrayType& array) const {
	  return p_ == array.p_;
	}
	
	MdArrayRef& operator=(const MdArrayRef&) = default;
	MdArrayRef& operator=(MdArrayRef&&) = default;

	MdArrayRef& operator=(
	    const MdArray<ARRAY_ORDER, Field, Allocator>& other
	) {
	  if (p_ == other.p_) {
	    // Do nothing
	  } else if (other.dimensions() == this->dimensions()) {
	    std::copy(other.data(), other.end(), this->data());
	  } else {
	    signalAssignmentFromArrayOfIncorrectDimension_(other);
	  }
	  return *this;
	}
	
	template <typename OtherArray,
		  typename Enabler =
		      typename std::enable_if<
		          IsMdArray<OtherArray>::value &&
		              OtherArray::ORDER == ARRAY_ORDER,
		          MdArrayRef
		      >::type
		 >
	MdArrayRef& operator=(const OtherArray& other) {
	  if (other.dimensions() == this->dimensions()) {
	    std::copy(other.data(), other.end(), this->data());
	  } else {
	    signalAssignmentFromArrayOfIncorrectDimension_(other);
	  }
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

	template <typename OtherField, typename OtherAllocator>
	[[noreturn]] void signalAssignmentFromArrayOfIncorrectDimension_(
	    const MdArray<ARRAY_ORDER, OtherField, OtherAllocator>& other
	) {
	  std::ostringstream msg;
	  msg << "Array \"other\" has incorrect dimensions "
	      << other.dimensions() << " -- it should have dimensions "
	      << this->dimensions() << ".  Assignment of an MdArray to "
	      << "an MdArrayRef cannot change the shape of the underlying "
	      << "array.";
	  throw pistis::exceptions::IllegalValueError(msg.str(),
						      PISTIS_EX_HERE);
	}
	
      private:
	DataPtr p_;

	friend class detail::MdArrayCommon<
	    MdArrayRef<ARRAY_ORDER, Field, Allocator>,
	    MdArray<ARRAY_ORDER, Field, Allocator>,
	    SliceType, InnerProductType,
	    ARRAY_ORDER, Field, Allocator
	>;
	friend class detail::MdArrayBase<
	    MdArrayRef<ARRAY_ORDER, Field, Allocator>,
	    MdArray<ARRAY_ORDER, Field, Allocator>,
	    SliceType, InnerProductType,
	    ARRAY_ORDER, Field, Allocator
	>;

	friend class AnyMdArrayRef<Field, Allocator>;
      };
    }
  }
}
#endif
