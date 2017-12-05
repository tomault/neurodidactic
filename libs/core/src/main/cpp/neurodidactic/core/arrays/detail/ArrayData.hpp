#ifndef __NEURODIDACTIC__CORE__ARRAYS__DETAIL__ARRAYDATA_HPP__
#define __NEURODIDACTIC__CORE__ARRAYS__DETAIL__ARRAYDATA_HPP__

#include <neurodidactic/core/arrays/DimensionList.hpp>
#include <atomic>
#include <memory>
#include <utility>
#include <stdint.h>

namespace neurodidactic {
  namespace core {
    namespace arrays {
      namespace detail {

	template <typename Field, typename Allocator>
	class ArrayData : Allocator {
	public:
	  typedef Field FieldType;
	  typedef Allocator AllocatorType;
	  typedef typename std::allocator_traits<Allocator>
	              ::template rebind_alloc<uint32_t>
	          DimensionListAllocator;
	  typedef DimensionList<DimensionListAllocator> DimensionListType;
	  
	public:
	  ArrayData(DimensionListType&& dimensions, const Allocator& allocator):
	      Allocator(allocator), dimensions_(std::move(dimensions)),
	      size_(dimensions_.numElements()),
	      leadingDimension_(size_ / dimensions_[0]),
  	      data_(this->allocate(size_)),
	      refCnt_(0) {
	    // Intentionally left blank
	  }
	  ArrayData(const ArrayData<Field, Allocator>&) = delete;
	  ~ArrayData() noexcept {
	    this->deallocate(data_, dimensions_.numElements());
	  }

	  const Allocator& allocator() const noexcept {
	    return static_cast<const Allocator&>(*this);
	  }
	  Allocator& allocator() noexcept {
	    return static_cast<Allocator&>(*this);
	  }
	  const DimensionListType& dimensions() const noexcept {
	    return dimensions_;
	  }
	  uint64_t size() const noexcept { return size_; }
	  uint64_t leadingDimension() const noexcept {
	    return leadingDimension_;
	  }
	  const Field* data() const noexcept { return data_; }
	  Field* data() noexcept { return data_; }
	  const Field* end() const noexcept { return data_ + size(); }
	  Field* end() const noexcept { return data_ + size(); }
	  uint32_t refCnt() const noexcept {
	    return refCnt_.load(std::memory_order_consume);
	  }

	  ArrayData& addRef() noexcept { ++refCnt_; return *this; }
	  uint32_t removeRef() noexcept { return --refCnt_; }
	  
	  Field operator[](uint64_t n) const noexcept { return data_[n]; }
	  Field& operator[](uint64_t n) noexcept { return data_[n]; }

	private:
	  DimensionListType dimensions_;
	  uint64_t size_;
	  uint64_t leadingDimension_;
	  Field* data_;
	  std::atomic<uint32_t> refCnt_;
	};							     
	
      }
    }
  }
}

#endif
