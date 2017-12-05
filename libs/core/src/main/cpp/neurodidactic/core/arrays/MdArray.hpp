#ifndef __NEURODIDACTIC__CORE__ARRAYS__MDARRAY_HPP__
#define __NEURODIDACTIC__CORE__ARRAYS__MDARRAY_HPP__

#include <neurodidactic/core/arrays/DimensionList.hpp>
#include <neurodidactic/core/arrays/MdArraySlice.hpp>
#include <neurodidactic/core/arrays/MklAllocator.hpp>
#include <neurodidactic/core/arrays/detail/ArrayDataPtr.hpp>
#include <algorithm>
#include <memory>
#include <type_traits>

namespace neurodidactic {
  namespace core {
    namespace arrays {

      template <size_t ARRAY_ORDER, typename Field,
		typename Allocator = MklAllocator<Field, 64> >
      class MdArray {
      public:
	static constexpr const size_t ORDER = ARRAY_ORDER;
	
	typedef Field FieldType;
	typedef Allocator AllocatorType;
	typedef DimensionList<> DimensionListType;
	typedef MdArraySlice<ARRAY_ORDER - 1, Field, Allocator> SliceType;
	
      protected:
	typedef detail::ArrayDataPtr<Field, Allocator> DataPtr;
	
      public:
	MdArray(const DimensionListType& dimensions,
		const AllocatorType& allocator = AllocatorType()):
	    p_(DataPtr::newData(dimensions, allocator)) {
	}

	MdArray(const DimensionListType& dimensions, Field value,
		const AllocatorType& allocator = AllocatorType()):
	    p_(DataPtr::newData(dimensions, allocator)) {
	  std::fill(p_->data(), p_->end(), value);
	}
	
	template <typename Iterator,
		  typename Enabler =
		      typename std::enable_if<
		          !std::is_arithmatic<Iterator>::value,
		          int
		      >::type>
	MdArray(const DimensionListType& dimensions, Iterator arrayData,
		const AllocatorType& allocator = AllocatorType(),
		Enabler = 0):
	    p_(DataPtr::newData(dimensions, allocator)) {
	  std::copy_n(arrayData, p_->size(), p_->data());
	}

	MdArray(const DimensionListType& dimensions,
		const std::initializer_list<Field>& arrayData,
		const AllocatorType& allocator = AllocatorType()):
	    p_(DataPtr::newData(dimensions, allocator)) {
	  std::copy(arrayData.begin(), arrayData.end(), p_->data());
	}

	MdArray(const MdArray& other) = default;
	MdArray(MdArray&& other) = default;

	template <typename OtherField, typename OtherAllocator>
	explicit MdArray(
	    const MdArray<ARRAY_ORDER, OtherField, OtherAllocator>& other
	):
	    p_(DataPtr::newData(other.dimensions(), other.allocator())) {
	  std::copy(other.begin(), other.end(), p_->data());
	}

	template <typename SliceField, typename SliceAllocator>
	explicit MdArray(
	    const MdArraySlice<ARRAY_ORDER, SliceField, SliceAllocator>& slice
	):
	    p_(DataPtr::newData(slice.dimensions(), slice.allocator())) {
	  std::copy(slice.begin(), slice.end(), p_->data());
	}

	const AllocatorType& allocator() { return p_->allocator(); }
	size_t size() const { return p_->size(); }
	size_t leadingDimension() const { return p_->leadingDimension(); }
	const DimensionListType& dimensions() const { return p_->dimensions(); }

	const Field* data() const { return p_->data(); }
	Field* data() { return p_->data(); }
	const Field* begin() const { return p_->data(); }
	Field* begin() { return p_->data(); }
	const Field* end() const { return p_->end(); }
	Field* end() { return p_->end(); }

	MdArray<ARRAY_ORDER - 1, Field, Allocator> product(
	    const MdArray<1, Field, Allocator>& vector
	) const {

	}

	MdArray<ARRAY_ORDER - 1, Field, Allocator>& product(
	    const MdArray<1, Field, Allocator>& vector,
	    MdArray<ARRAY_ORDER - 1, Field, Allocator>& result
	) const {

	}

	MdArray<ARRAY_ORDER, Field, Allocator> product(
	    const MdArray<2, Field, Allocator>& matrix
	) const {

	}

	MdArray<ARRAY_ORDER, Field, Allocator>& product(
	    const MdArray<2, Field, Allocator>& matrix,
	    MdArray<ARRAY_ORDER, Field, Allocator>& result
	) const {

	}
	
	MdArray& operator=(const MdArray&) = default;
	MdArray& operator=(MdArray&&) = default;

	template <typename OtherField, typename OtherAllocator>
	MdArray& operator=(const MdArray<OtherField, OtherAllocator>& other) {
	  p_ = DataPtr::newData(other.dimensions(), allocator());
	  std::copy(other.begin(), other.end(), p_->data());
	  return *this;
	}

	template <typename SliceField, typename SliceAllocator>
	MdArray& operator=(
	    const MdArraySlice<ARRAY_ORDER, SliceField, SliceAllocator>& slice
	) {
	  p_ = DataPtr::newData(slice.dimensions(), allocator());
	  std::copy(other.begin(), other.end(), p_->data());
	  return *this;
	}

	const SliceType operator[](size_t n) const {
	  return SliceType(p_, p_->dimensions().subList(1),
			   p_->data() + n * p_->leadingDimension());
	}

	SliceType operator[](size_t n) {
	  return SliceType(p_, p_->dimensions().subList(1),
			   p_->data() + n * p_->leadingDimension());
	}
	
      private:
	DataPtr p_;
	

      };
      
    }
  }
}
#endif

