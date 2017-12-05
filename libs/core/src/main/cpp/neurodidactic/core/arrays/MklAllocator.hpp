#ifndef __NEURODIDACTIC__CORE__ARRAYS__MKLALLOCATOR_HPP__
#define __NEURODIDACTIC__CORE__ARRAYS__MKLALLOCATOR_HPP__

#include <type_traits>
#include <stdint.h>

namespace neurodidactic {
  namespace core {
    namespace arrays {

      template <typename T, size_t ALIGNMENT = 64>
      class MklAllocator {
      public:
	typedef T value_type;
	typedef T* pointer;
	typedef const T* const_pointer;
	typedef T& reference;
	typedef const T& const_reference;
	typedef size_t size_type;
	typedef ssize_t difference_type;
	typedef std::true_type propagate_on_container_move_assignment;
	typedef std::true_type is_always_equal;

	static constexpr const size_t MEMORY_ALIGNMENT = ALIGNMENT;

	template <typename U>
	struct rebind { typedef MklAllocator<U> other; };

      public:
	MklAllocator() noexcept { }

	template <typename U>
	MklAllocator(const MklAllocator<U>& other) noexcept { }

	const T* reference(T& r) const noexcept { return &r; }
	T* reference(T& r) noexcept { return &r; }

	T* allocate(size_t n, const void* hint = nullptr) {
	  T* p = (T*)mkl_malloc(n * sizeof(T), ALIGNMENT);
	  if (!p) {
	    throw std::bad_alloc();
	  }
	  return p;
	}
	void deallocate(T* p, size_t n) noexcept { mkl_free((void*)p); }
	size_t max_size() const noexcept { return size_t(-1); }

	template <typename U, typename... Args>
	void construct(U* p, Args&&... args) {
	  ::new((void*) p) U(std::forward<Args>(args)...);
	}

	template <typename U>
	void destroy(U* p) {
	  p->~U();
	}
      };

      template <typename T, typename U>
      bool operator==(const MklAllocator<T>& left,
		      const MklAllocator<U>& right) noexcept {
	return true;
      }

      template <typename T, typename U>
      bool operator!=(const MklAllocator<T>& left,
		      const MklAllocator<U>& right) noexcpet {
	return false;
      }
      
    }
  }
}
#endif
