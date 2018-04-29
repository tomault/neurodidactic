#ifndef __NEURODIDACTIC__CORE__ARRAYS__ANYMDARRAYREF_HPP__
#define __NEURODIDACTIC__CORE__ARRAYS__ANYMDARRAYREF_HPP__

#include <neurodidactic/core/arrays/MdArrayRef.hpp>
#include <neurodidactic/core/arrays/detail/ArrayDataPtr.hpp>
#include <pistis/exceptions/IllegalValueError.hpp>
#include <sstream>

namespace neurodidactic {
  namespace core {
    namespace arrays {

      template <typename Field, typename Allocator>
      class AnyMdArrayRef {
      protected:
	typedef detail::ArrayDataPtr<Field, Allocator> DataPtr;
	
      public:
	template <size_t ARRAY_ORDER>	
	explicit AnyMdArrayRef(
	    const MdArrayRef<ARRAY_ORDER, Field, Allocator>& r
	): p_(r.p_) {
	}

	AnyMdArrayRef(const AnyMdArrayRef&) = default;
	AnyMdArrayRef(AnyMdArrayRef&&) = default;

	template <size_t ARRAY_ORDER>
	MdArrayRef<ARRAY_ORDER, Field, Allocator> cast() const {
	  if (p_->dimensions().size() != ARRAY_ORDER) {
	    std::ostringstream msg;
	    msg << "Cannot convert an MdArrayRef of order "
		<< p_->dimensions().size() << " to an an MdArrayRef of order "
		<< ARRAY_ORDER;
	    throw pistis::exceptions::IllegalValueError(msg.str(),
							PISTIS_EX_HERE);
	  }
	  return MdArrayRef<ARRAY_ORDER, Field, Allocator>(p_);
	}
	
	AnyMdArrayRef& operator=(const AnyMdArrayRef&) = default;
	AnyMdArrayRef& operator=(AnyMdArrayRef&&) = default;

	bool operator==(const AnyMdArrayRef& other) const {
	  return p_ == other.p_;
	}

	bool operator!=(const AnyMdArrayRef& other) const {
	  return p_ != other.p_;
	}

      private:
	DataPtr p_;
      };
      
    }
  }
}

#endif
