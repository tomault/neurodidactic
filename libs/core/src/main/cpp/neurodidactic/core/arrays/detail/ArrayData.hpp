#ifndef __NEURODIDACTIC__CORE__ARRAYS__DETAIL__ARRAYDATA_HPP__
#define __NEURODIDACTIC__CORE__ARRAYS__DETAIL__ARRAYDATA_HPP__

#include <neurodidactic/core/arrays/DimensionList.hpp>
#include <atomic>
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
	  
	public:
	  ArrayData(DimensionList&& dimensions, const Allocator& allocator):
	      Allocator(allocator), dimensions_(std::move(dimensions)),
  	      data_(allocator.allocate(dimensions_.numElements())),
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
	  const DimensionList& dimensions() const noexcept {
	    return dimensions_;
	  }
	  uint64_t size() const noexcept {
	    return dimensions_.numElements();
	  }
	  const Field* data() const noexcept { return data_; }
	  Field* data() noexcept { return data_; }
	  uint32_t refCnt() const noexcept {
	    return refCnt_.load(std::memory_order_consume);
	  }

	  ArrayData* addRef() noexcept { ++refCnt_; return *this; }
	  uint32_t removeRef() noexcept { return --refCnt_; }
	  
	  Field operator[](uint64_t n) const noexcept { return data_[n]; }
	  Field& operator[](uint64_t n) const noexcept { return data_[n]; }

	private:
	  DimensionList dimensions_;
	  Field* data_;
	  std::atomic_uint32_t refCnt_;
	};							     

	template <typename Field, typename Allocator>
	class ArrayDataPtr :
	    typename std::allocator_traits<Allocator>
	                ::template rebind_alloc<ArrayData> {
	public:
	  typedef std::allocator_traits<Allocator>
	             ::template rebind_alloc<ArrayData> ArrayDataAllocator;
	  typedef std::allocator_traits<Allocator>
	             ::template rebind_alloc<Field> ElementAllocator;
	  typedef ArrayData<Field, ElementAllocator> ArrayDataType;
	  
	public:	  
	  explicit ArrayDataPtr(const Allocator& allocator) :
	      ArrayDataAllocator(allocator), p_(nullptr) {
	    // Intentionally left blank
	  }
	  
	  ArrayDataPtr(ArrayDataType* p, const Allocator& allocator) :
	      ArrayDataAllocator(allocator), p_(addRef_(p)) {
	    // Intentionally left blank
	  }

	  
	  ArrayDataPtr(const ArrayDataPtr<Field, Allocator>& p) :
	      ArrayDataAllocator(p), p_(addRef_(p)) {
	    // Intentionally left blank
	  }
	  
	  ArrayDataPtr(ArrayDataPtr<Field, Allocator>&& p) :
	      ArrayDataAllocator(std::move(p)), p_(p.release()) {
	    // Intentionally left blank
	  }
	  ~ArrayDataPtr() { removeRef_(*this, p_); }


	  const ArrayDataAllocator& allocator() const {
	    return static_cast<const ArrayDataAllocator&>(*this);
	  }

	  ArrayDataAllocator& allocator() {
	    return static_cast<ArrayDataAllocator&>(*this);
	  }
	  
	  ArrayDataType* get() const noexcept { return p_; }
	  ArrayDataType* release() noexcept {
	    ArrayDataType* tmp = p_;
	    p_ = nullptr;
	    return tmp;
	  }

	  ArrayDataPtr<Field, Allocator>& operator=(
	      const ArrayDataPtr<Field, Allocator>& other
	  ) noexcept {
	    if (p_ != other.p_) {
	      removeRef_(allocator(), p_);
	      allocator() = other.allocator();
	      p_ = addRef_(other.p_);
	    }
	    return *this;
	  }

	  ArrayDataPtr<Field, Allocator>& operator=(
	      ArrayDataPtr<Field, Allocator>&& other
	  ) noexcept {
	    if (p_ != other.p_) {
	      removeRef(allocator(), p_);
	      allocator() = other.allocator();
	      p_ = other.release();
	    }
	    return *this;
	  }
	  
	  operator bool() const noexcept { return (bool)p_; }
	  ArrayDataType& operator*() const noexcept { return *p_; }
	  ArrayDataType* operator->() const noexcept { return p_; }

	  bool operator==(
	      const ArrayDataPtr<Field, Allocator>& other
	  ) const noexcept {
	    return p_ == other.p_;
	  }

	  bool operator!=(
	      const ArrayDataPtr<Field, Allocator>& other
	  ) const noexcept {
	    return p_ != other.p_;
	  }

	  static ArrayDataPtr<Field, Allocator> newData(
	      DimensionList&& dimensions, const Allocator& allocator
	  ) {
	    ArrayDataAllocator tdAllocator(allocator);
	    return ArrayDataPtr<Field, Allocator>(
		new(tdAllocator.allocate(sizeof(ArrayDataType))),
		    ArrayDataType(std::move(dimensions),
				   ElementAllocator(allocator))
	    );
	  }
	  
	private:
	  ArrayDataType* p_;

	  static ArrayDataType* addRef_(
	      const ArrayDataPtr<Field, Allocator>& p
	  ) noexcept {
	    return addRef_(p.get());
	  }
	  
	  static ArrayDataType* addRef_(ArrayDataType* p) noexcept {
	    return p ? p->addRef() : p;
	  }

	  static void removeRef_(ArrayDataAllocator& allocator,
				 ArrayDataPtr<Field, Allocator>& p) noexcept {
	    removeRef_(allocator, p.get());
	  }
	  
	  static void removeRef_(ArrayDataAllocator& allocator,
				 ArrayDataType* p) noexcept {
	    if (p && !p->removeRef()) {
	      p->~ArrayData();
	      allocator.deallocate(p, sizeof(ArrayDataType));
	    }
	  }
	};
	
      }
    }
  }
}

#endif
