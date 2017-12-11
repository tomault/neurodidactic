#ifndef __NEURODIDACTIC__CORE__ARRAYS__MDARRAYSLICE_HPP__
#define __NEURODIDACTIC__CORE__ARRAYS__MDARRAYSLICE_HPP__

#include <neurodidactic/core/arrays/DimensionList.hpp>
#include <neurodidactic/core/arrays/MklAllocator.hpp>
#include <neurodidactic/core/arrays/detail/ArrayDataPtr.hpp>
#include <neurodidactic/core/arrays/detail/MdArrayBase.hpp>
#include <neurodidactic/core/arrays/detail/MdArrayProperties.hpp>

namespace neurodidactic {
  namespace core {
    namespace arrays {

      template <size_t ARRAY_ORDER, typename Field, typename Allocator>
      class MdArraySlice :
	  public detail::MdArrayBase<
	      MdArraySlice<ARRAY_ORDER, Field, Allocator>,
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
		    ARRAY_ORDER, Field, Allocator>::type
	        SliceType;
	typedef typename detail::MdArrayInnerProductType<
	            ARRAY_ORDER, Field, Allocator
	        >::type InnerProductType;

      protected:
	typedef detail::MdArrayBase<
	            MdArraySlice<ARRAY_ORDER, Field, Allocator>,
	            SliceType, InnerProductType, ARRAY_ORDER,
		    Field, Allocator>
	        BaseType;
	typedef detail::ArrayDataPtr<Field, Allocator> DataPtr;

      public:
	typedef MdArraySlice<ARRAY_ORDER, Field, Allocator> ThisType;
	typedef typename BaseType::AllocatorType AllocatorType;
	typedef typename BaseType::DimensionListType DimensionListType;

      public:
	MdArraySlice(const MdArraySlice&) = default;
	MdArraySlice(MdArraySlice&&) = default;

	MdArray<ARRAY_ORDER, Field, Allocator> toArray() const {
	  return MdArray<ARRAY_ORDER, Field, Allocator>(
	      this->dimensions(), this->begin(), this->allocator()
	  );
	}

	MdArraySlice& operator=(const MdArraySlice&) = default;
	MdArraySlice& operator=(MdArraySlice&&) = default;

	template <typename OtherField, typename OtherAllocator>
	MdArraySlice& operator=(
	    const MdArray<ARRAY_ORDER, OtherField, OtherAllocator>& other
	) {
	  validateDimensions_("Array \"other\"", other.dimensions(),
			      PISTIS_EX_HERE);
	  std::copy_n(other.data(), other.size(), this->data());
	  return *this;
	}

	template <typename OtherField, typename OtherAllocator>
	MdArraySlice& operator=(
	    const MdArraySlice<ARRAY_ORDER, OtherField, OtherAllocator>& other
	) {
	  validateDimensions_("Array \"other\"", other.dimensions(),
			      PISTIS_EX_HERE);
	  std::copy_n(other.begin(), other.size(), this->data());
	  return *this;
	}

      protected:
	const Allocator& allocator_() const { return p_->parent->allocator(); }
	Allocator& allocator_() { return p_->parent->allocator(); }
	size_t size_() const { return p_->size; }
	size_t leadingDimension_() const { return p_->leadingDimension; }
	const DimensionListType& dimensions_() const {
	  return p_->dimensions;
	}

	const Field* data_() const { return p_->data; }
	Field* data_() { return p_->data; }
	const Field* end_() const { return p_->data + p_->size; }
	Field* end_() { return p_->data + p_->size; }

	template <typename Index,
		  typename Enabled =
		      typename std::enable_if<
		          std::is_unsigned<Index>::value && (ARRAY_ORDER > 1),
		          const SliceType
	              >::type
	          >
	Enabled slice_(Index n) const {
	  return SliceType(p_->parent, p_->dimensions.sublist(1),
			   p_->data + n * p_->leadingDimension);
	}

	template <typename Index,
		  typename Enabled =
		      typename std::enable_if<
		          std::is_unsigned<Index>::value && (ARRAY_ORDER > 1),
		          const SliceType
	              >::type
	          >
	Enabled slice_(Index n) {
	  return SliceType(p_->parent, p_->dimensions.sublist(1),
			   p_->data + n * p_->leadingDimension);
	}

      private:
	struct SliceData {
	  DataPtr parent;
	  DimensionListType dimensions;
	  Field* data;
	  size_t size;
	  size_t leadingDimension;

	  SliceData(DataPtr parent_, const DimensionListType& dimensions_,
		    Field* data_):
	      parent(parent_), dimensions(dimensions_), data(data_),
	      size(dimensions_.numElements()),
	      leadingDimension(dimensions_.numElements() / dimensions_[0]) {
	  }
	};

      private:
	std::shared_ptr<SliceData> p_;

	MdArraySlice(const DataPtr& parent,
		     const DimensionListType& dimensions,
		     Field* data):
	    p_(new SliceData(parent, dimensions, data)) {
	}

	friend class MdArray<ARRAY_ORDER + 1, Field, Allocator>;
	friend class MdArraySlice<ARRAY_ORDER + 1, Field, Allocator>;
	friend class detail::MdArrayCommon<
	    MdArraySlice<ARRAY_ORDER, Field, Allocator>,
	    SliceType, InnerProductType,
	    ARRAY_ORDER, Field, Allocator
	>;
	friend class detail::MdArrayBase<
	    MdArraySlice<ARRAY_ORDER, Field, Allocator>,
	    SliceType, InnerProductType,
	    ARRAY_ORDER, Field, Allocator
	>;
      };
      
    }
  }
}
#endif

