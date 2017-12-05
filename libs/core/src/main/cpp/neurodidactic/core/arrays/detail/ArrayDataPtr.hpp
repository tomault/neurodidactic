#ifndef __NEURODIDACTIC__CORE__ARRAYS__DETAIL__ARRAYDATAPTR_HPP__
#define __NEURODIDACTIC__CORE__ARRAYS__DETAIL__ARRAYDATAPTR_HPP__

#include <neurodidactic/core/arrays/detail/ArrayData.hpp>
#include <memory>
#include <utility>

namespace neurodidactic {
  namespace core {
    namespace arrays {
      namespace detail {

	template <typename Field, typename Allocator>
	class ArrayDataPtr :
	    std::allocator_traits<Allocator>
	        ::template rebind_alloc<
	            ArrayData<
		        Field,
		        typename std::allocator_traits<Allocator>
		            ::template rebind_alloc<Field>
		    >
	        >
        {
	public:
	  typedef typename std::allocator_traits<Allocator>
	                       ::template rebind_alloc<Field>
	          ElementAllocator;
	  typedef ArrayData<Field, ElementAllocator> ArrayDataType;
	  typedef typename std::allocator_traits<Allocator>
	                       ::template rebind_alloc<ArrayDataType>
	          ArrayDataAllocator;
	  
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
	      removeRef_(allocator(), p_);
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
	      typename ArrayDataType::DimensionListType&& dimensions,
	      const Allocator& allocator
	  ) {
	    ArrayDataAllocator tdAllocator(allocator);
	    ArrayDataType* p =
	        new(tdAllocator.allocate(sizeof(ArrayDataType)))
	        ArrayDataType(std::move(dimensions),
			      ElementAllocator(allocator));
	    return ArrayDataPtr<Field, Allocator>(p,
						  ElementAllocator(allocator));
	  }
	  
	private:
	  ArrayDataType* p_;

	  static ArrayDataType* addRef_(
	      const ArrayDataPtr<Field, Allocator>& p
	  ) noexcept {
	    return addRef_(p.get());
	  }
	  
	  static ArrayDataType* addRef_(ArrayDataType* p) noexcept {
	    if (p) {
	      p->addRef();
	    }
	    return p;
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
