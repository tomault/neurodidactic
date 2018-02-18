#ifndef __NEURODIDACTIC__CORE__ARRAYS__DETAIL__MDARRAYCOMMON_HPP__
#define __NEURODIDACTIC__CORE__ARRAYS__DETAIL__MDARRAYCOMMON_HPP__

#include <neurodidactic/core/arrays/DimensionList.hpp>
#include <neurodidactic/core/arrays/AnyMdArray.hpp>
#include <neurodidactic/core/arrays/detail/MklAdapter.hpp>
#include <pistis/exceptions/ExceptionOrigin.hpp>
#include <pistis/exceptions/IllegalValueError.hpp>

namespace neurodidactic {
  namespace core {
    namespace arrays {
      namespace detail {

	template <typename DerivedArray,
		  typename NewArray,
		  typename ArraySlice,
		  typename InnerProductResult,
		  size_t ARRAY_ORDER,
		  typename Field, typename Allocator>
	class MdArrayCommon : public AnyMdArray<DerivedArray> {
	protected:
	  typedef typename std::allocator_traits<Allocator>
	                      ::template rebind_alloc<uint32_t>
	          DimensionListAllocator;
	public:
	  static constexpr const size_t ORDER = ARRAY_ORDER;
	
	  typedef Field FieldType;
	  typedef Allocator AllocatorType;
	  typedef DimensionList<DimensionListAllocator> DimensionListType;
	  typedef ArraySlice SliceType;
	  typedef MdArrayCommon<DerivedArray, NewArray, ArraySlice,
				InnerProductResult, ARRAY_ORDER, Field,
				Allocator>
	          ThisType;
	
	public:
	  const AllocatorType& allocator() const {
	    return this->self().allocator_();
	  }
	  AllocatorType& allocator() { return this->self().allocator_(); }
	  size_t size() const { return this->self().size_(); }
	  size_t leadingDimension() const {
	    return this->self().leadingDimension_();
	  }
	  const DimensionListType& dimensions() const {
	    return this->self().dimensions_();
	  }

	  const Field* data() const { return this->self().data_(); }
	  Field* data() { return this->self().data_(); }
	  const Field* begin() const { return this->self().data_(); }
	  Field* begin() { return this->self().data_(); }
	  const Field* end() const { return this->self().end_(); }
	  Field* end() { return this->self().end_(); }

	  template <typename OtherArray,
		    typename Enabled =
		        typename std::enable_if<
		            IsMdArray<OtherArray>::value, NewArray
			>::type
		   >
	  Enabled add(const OtherArray& other) const {
	    validateDimensions_("Array \"other\"", other.dimensions(),
				PISTIS_EX_HERE);

	    NewArray result(this->dimensions(), this->allocator());
	    return std::move(this->add(other, result));
	  }

	  template <typename OtherArray,
		    typename ResultArray,
		    typename Enabled =
		        typename std::enable_if<
		            IsMdArray<OtherArray>::value &&
		                IsMdArray<ResultArray>::value,
			    int
			>::type
		   >
	  ResultArray& add(const OtherArray& other, ResultArray& result,
			   Enabled = 0) const {
	    validateDimensions_("Array \"other\"", other.dimensions(),
				PISTIS_EX_HERE);
	    validateDimensions_("Array \"result\"", result.dimensions(),
				PISTIS_EX_HERE);
	    detail::MklAdapter<Field, typename OtherArray::FieldType>::add(
	        this->size(), this->data(), other.data(), result.data()
	    );
	    return result;
	  }

	  template <typename OtherArray,
		    typename Enabled =
		        typename std::enable_if<IsMdArray<OtherArray>::value,
					        int
			         >::type
		   >
	  DerivedArray& addInPlace(const OtherArray& other, Enabled = 0) {
	    return this->add(other, this->self());
	  }

	  template <typename OtherArray,
		    typename Enabled =
		        typename std::enable_if<
		            IsMdArray<OtherArray>::value, NewArray
			>::type
		   >
	  Enabled scaleAndAdd(Field c, const OtherArray& other) const {
	    validateDimensions_("Array \"other\"", other.dimensions(),
				PISTIS_EX_HERE);

	    NewArray result(this->self());
	    return std::move(result.scaleAndAddInPlace(c, other));
	  }

	  template <typename OtherArray,
		    typename ResultArray,
		    typename Enabled =
		        typename std::enable_if<
		            IsMdArray<OtherArray>::value &&
		                IsMdArray<ResultArray>::value,
			    ResultArray
			>::type
		   >
	  Enabled& scaleAndAdd(Field c, const OtherArray& other,
			       ResultArray& result) const {
	    validateDimensions_("Array \"other\"", other.dimensions(),
				PISTIS_EX_HERE);
	    validateDimensions_("Array \"result\"", result.dimensions(),
				PISTIS_EX_HERE);
	    result = this->self();
	    return result.scaleAndAddInPlace(c, other);
	  }

	  template <typename OtherArray,
		    typename Enabled =
		        typename std::enable_if<
		            IsMdArray<OtherArray>::value, int
			>::type
		   >
	  DerivedArray& scaleAndAddInPlace(Field c, const OtherArray& other,
					   Enabled = 0) {
	    validateDimensions_("Array \"other\"", other.dimensions(),
				PISTIS_EX_HERE);
	    detail::MklAdapter<Field, typename OtherArray::FieldType>::scaleAndAdd(
	        this->size(), c, other.data(), this->data()
	    );
	    return this->self();
	  }

	  template <typename OtherArray,
		    typename Enabled =
		        typename std::enable_if<
		            IsMdArray<OtherArray>::value, NewArray
			>::type
		   >
	  Enabled scaleAndAdd(Field c1, Field c2,
			      const OtherArray& other) const {
	    validateDimensions_("Array \"other\"", other.dimensions(),
				PISTIS_EX_HERE);

	    NewArray result(this->self());
	    return std::move(result.scaleAndAddInPlace(c1, c2, other));
	  }

	  template <typename OtherArray,
		    typename ResultArray,
		    typename Enabled =
		        typename std::enable_if<
		            IsMdArray<OtherArray>::value &&
		                IsMdArray<ResultArray>::value,
			    int
			>::type
		   >
	  ResultArray& scaleAndAdd(Field c1, Field c2, const OtherArray& other,
				   ResultArray& result, Enabled = 0) const {
	    validateDimensions_("Array \"other\"", other.dimensions(),
				PISTIS_EX_HERE);
	    validateDimensions_("Array \"result\"", result.dimensions(),
				PISTIS_EX_HERE);
	    result = this->self();
	    return result.scaleAndAddInPlace(c1, c2, other);
	  }

	  template <typename OtherArray,
		    typename Enabled =
		        typename std::enable_if<IsMdArray<OtherArray>::value,
					        int
			         >::type
		   >
	  DerivedArray& scaleAndAddInPlace(Field c1, Field c2,
					   const OtherArray& other,
					   Enabled = 0) {
	    validateDimensions_("Array \"other\"", other.dimensions(),
				PISTIS_EX_HERE);
	    detail::MklAdapter<Field, typename OtherArray::FieldType>::scaleAndAdd(
	        this->size(), c2, other.data(), c1, this->data()
	    );
	    return this->self();
	  }

	  template <typename OtherArray,
		    typename Enabled =
		        typename std::enable_if<
		            IsMdArray<OtherArray>::value, NewArray
			>::type
		   >
	  Enabled subtract(const OtherArray& other) const {
	    validateDimensions_("array \"other\"", other.dimensions(),
				PISTIS_EX_HERE);

	    NewArray result(dimensions(), allocator());
	    return std::move(this->subtract(other, result));
	  }

	  template <typename OtherArray,
		    typename ResultArray,
		    typename Enabled =
		        typename std::enable_if<
		            IsMdArray<OtherArray>::value &&
		                IsMdArray<ResultArray>::value,
		            int
			>::type
		   >
	  ResultArray& subtract(const OtherArray& other, ResultArray& result,
				Enabled = 0) const {
	    validateDimensions_("array \"other\"", other.dimensions(),
				PISTIS_EX_HERE);
	    validateDimensions_("array \"result\"", result.dimensions(),
				PISTIS_EX_HERE);
	    detail::MklAdapter<Field, typename OtherArray::FieldType>::subtract(
		this->size(), this->data(), other.data(), result.data()
	    );
	    return result;
	  }

	  template <typename OtherArray,
		    typename Enabled =
		        typename std::enable_if<IsMdArray<OtherArray>::value,
						int>::type
		    >
	  DerivedArray& subtractInPlace(const OtherArray& other, Enabled = 0) {
	    return this->subtract(other, this->self());
	  }

	  template <typename OtherArray,
		    typename Enabled =
		        typename std::enable_if<
		            IsMdArray<OtherArray>::value, NewArray
			>::type
		   >
	  Enabled multiply(const OtherArray& other) const {
	    validateDimensions_("array \"other\"", other.dimensions(),
				PISTIS_EX_HERE);

	    NewArray result(dimensions(), allocator());
	    return std::move(this->multiply(other, result));
	  }

	  template <typename OtherArray,
		    typename ResultArray,
		    typename Enabled =
		        typename std::enable_if<
		            IsMdArray<OtherArray>::value &&
		                IsMdArray<ResultArray>::value,
		            int
			>::type
		   >
	  ResultArray& multiply(const OtherArray& other, ResultArray& result,
				Enabled = 0) const {
	    validateDimensions_("array \"other\"", other.dimensions(),
				PISTIS_EX_HERE);
	    validateDimensions_("array \"result\"", result.dimensions(),
				PISTIS_EX_HERE);
	    detail::MklAdapter<Field, typename OtherArray::FieldType>::multiply(
		this->size(), this->data(), other.data(), result.data()
	    );
	    return result;
	  }

	  template <typename OtherArray,
		    typename Enabled =
		        typename std::enable_if<IsMdArray<OtherArray>::value,
						int>::type
		    >
	  DerivedArray& multiplyInPlace(const OtherArray& other, Enabled = 0) {
	    return this->multiply(other, this->self());
	  }

	  NewArray multiply(Field c) const {
	    NewArray result(this->self());
	    return std::move(result.multiplyInPlace(c));
	  }

	  template <typename ResultArray,
		    typename Enabled =
		        typename std::enable_if<IsMdArray<ResultArray>::value,
						int>::type
		   >
	  ResultArray& multiply(Field c, ResultArray& result) const {
	    validateDimensions_("Array \"result\"", result.dimensions(),
				PISTIS_EX_HERE);
	    result = this->self();
	    return result.multiplyInPlace(c);
	  }

	  DerivedArray& multiplyInPlace(Field c) {
	    detail::MklAdapter<Field, Field>::scale(this->size(), c,
						    this->data());
	    return this->self();
	  }
	  
	  template <typename OtherArray,
		    typename Enabled =
		        typename std::enable_if<
		            IsMdArray<OtherArray>::value, DerivedArray
			>::type
		   >
	  Enabled divide(const OtherArray& other) const {
	    validateDimensions_("array \"other\"", other.dimensions(),
				PISTIS_EX_HERE);

	    NewArray result(dimensions(), allocator());
	    return std::move(this->divide(other, result));
	  }

	  template <typename OtherArray,
		    typename ResultArray,
		    typename Enabled =
		        typename std::enable_if<
		            IsMdArray<OtherArray>::value &&
		                IsMdArray<ResultArray>::value,
		            int
			>::type
		   >
	  ResultArray& divide(const OtherArray& other, ResultArray& result,
			      Enabled = 0) const {
	    validateDimensions_("array \"other\"", other.dimensions(),
				PISTIS_EX_HERE);
	    validateDimensions_("array \"result\"", result.dimensions(),
				PISTIS_EX_HERE);
	    detail::MklAdapter<Field, typename OtherArray::FieldType>::divide(
		this->size(), this->data(), other.data(), result.data()
	    );
	    return result;
	  }

	  template <typename OtherArray,
		    typename Enabled =
		        typename std::enable_if<
		            IsMdArray<OtherArray>::value, int
		        >::type
		    >
	  DerivedArray& divideInPlace(const OtherArray& other, Enabled = 0) {
	    return this->divide(other, this->self());
	  }

	  
	  template <typename Vector,
		    typename Enabled =
		        typename std::enable_if<
		            IsMdArray<Vector>::value && (Vector::ORDER == 1),
	                    InnerProductResult	    
	                >::type
		   >
	  Enabled innerProduct(const Vector& v) const {
	    this->validateInnerProductArgDimensions_("Vector \"v\"",
						     v.dimensions(),
						     PISTIS_EX_HERE);
	    DimensionListType resultDimensions(
	        this->dimensions().butLast(1)
	    );
	    InnerProductResult result(resultDimensions, this->allocator());
	    return std::move(this->self().innerProduct(v, result));
	  }

	  template <typename Vector,
		    typename Enabled =
		        typename std::enable_if<
		            IsMdArray<Vector>::value && (Vector::ORDER == 1),
	                    InnerProductResult
	                >::type
		   >
	  Enabled transposeInnerProduct(const Vector& v) const {
	    this->validateTransposeInnerProductArgDimensions_("Vector \"v\"",
							      v.dimensions(),
							      PISTIS_EX_HERE);
	    const DimensionListType resultDimensions =
	        this->dimensions().remove(this->dimensions().size() - 2);
	    InnerProductResult result(resultDimensions, this->allocator());
	    return std::move(this->self().transposeInnerProduct(v, result));
	  }
	  
	  template <typename Matrix,
		    typename Enabled =
		        typename std::enable_if<
		            IsMdArray<Matrix>::value && (Matrix::ORDER == 2),
	                    NewArray
	                >::type
		   >
	  Enabled matrixProduct(const Matrix& m) const {
	    this->validateMatrixProductArgDimensions_("Matrix \"m\"",
						      m.dimensions(),
						      PISTIS_EX_HERE);
	    DimensionListType resultDimensions(
	        this->dimensions().replaceLast(m.dimensions()[1])
	    );
	    NewArray result(resultDimensions, this->allocator());
	    return std::move(this->self().matrixProduct(m, result));
	  }

	  template <typename Function>
	  NewArray map(const Function& f) const {
	    NewArray result(this->dimensions(), this->allocator());
	    return std::move(this->map(f, result));
	  }

	  template <typename Function, typename ResultArray,
		    typename Enabled =
		        typename std::enable_if<
		            IsMdArray<ResultArray>::value &&
		                (ResultArray::ORDER == ARRAY_ORDER),
	                    ResultArray
	                >::type
		   >
	  Enabled& map(const Function& f, ResultArray& result) const {
	    if (result.dimensions() != this->dimensions()) {
	      std::ostringstream msg;
	      msg << "Array \"result\" has incorrect dimensions "
		  << result.dimensions() << " -- it should have dimensions "
		  << this->dimensions();
	      throw pistis::exceptions::IllegalValueError(msg.str(),
							  PISTIS_EX_HERE);
	    }

	    auto p = this->data();
	    auto q = result.data();
#pragma omp parallel for
	    {
	      for (size_t i = 0; i < this->size(); ++i) {
		q[i] = f(p[i]);
	      }
	    }

	    return result;
	  }

	  template <typename Function>
	  DerivedArray& mapInPlace(Function f) {
	    return this->map(f, this->self());
	  }
	  
	//   const SliceType operator[](size_t n) const {
	//     return self()->slice_(n);
	//   }

	//   SliceType operator[](size_t n) {
	//     return self()->slice_(n);
	//   }

	protected:
	  void validateDimensions_(
	      const std::string& name,
	      const DimensionListType& d,
	      const pistis::exceptions::ExceptionOrigin& origin
	  ) const {
	    if (d != this->dimensions()) {
	      std::ostringstream msg;
	      msg << name << " has incorrect dimensions " << d
		  << ".  It should have dimensions " << this->dimensions();
	      throw pistis::exceptions::IllegalValueError(msg.str(), origin);
	    }
	  }

	  void validateInnerProductArgDimensions_(
	      const std::string& name,
	      const DimensionListType& dimensions,
	      const pistis::exceptions::ExceptionOrigin& origin
	  ) const {
	    const size_t lastDimension = this->dimensions().back();
	    if ((dimensions.size() != 1) || (dimensions[0] != lastDimension)) {
	      std::ostringstream msg;
	      msg << name << " has dimensions " << dimensions
		  << ", but it should have dimension [ " << lastDimension
		  << " ]";
	      throw pistis::exceptions::IllegalValueError(msg.str(), origin);
	    }
	  }

	  void validateInnerProductResultDimensions_(
	      const std::string& name,
	      const DimensionListType& dimensions,
	      const pistis::exceptions::ExceptionOrigin& origin
	  ) const {
	    const DimensionListType targetDimensions =
	        this->dimensions().butLast(1);
	    
	    if (dimensions != targetDimensions) {
	      std::ostringstream msg;
	      msg << name << " has dimensions " << dimensions
		  << ", but it should have dimensions " << targetDimensions;
	      throw pistis::exceptions::IllegalValueError(msg.str(), origin);
	    }
	  }

	  void validateMatrixProductArgDimensions_(
	      const std::string& name,
	      const DimensionListType& dimensions,
	      const pistis::exceptions::ExceptionOrigin& origin
	  ) const {
	    if (dimensions.size() != 2) {
	      std::ostringstream msg;
	      msg << name << " is not a matrix.  It is an array with order "
		  << dimensions.size();
	      throw pistis::exceptions::IllegalValueError(msg.str(), origin);
	    }
	    if (dimensions[0] != this->self().dimensions().back()) {
	      std::ostringstream msg;
	      msg << name << " has incorrect dimensions " << dimensions
		  << " -- the first dimension must match the last dimension"
		  << " of this array (which is "
		  << this->self().dimensions().back() << ")";
	      throw pistis::exceptions::IllegalValueError(msg.str(), origin);
	    }
	  }

	  void validateMatrixProductResultDimensions_(
	      const std::string& name,
	      const DimensionListType& argDimensions,
	      const DimensionListType& resultDimensions,
	      const pistis::exceptions::ExceptionOrigin& origin
	  ) const {
	    const DimensionListType targetDimensions =
	        this->self().dimensions().replaceLast(argDimensions[1]);

	    if (resultDimensions != targetDimensions) {
	      std::ostringstream msg;
	      msg << name << " has incorrect dimensions " << targetDimensions
		  << " -- it should have dimensions " << resultDimensions;
	      throw pistis::exceptions::IllegalValueError(msg.str(), origin);
	    }
	  }

	  void validateOuterProductDimensions_(
	      const std::string& name,
	      const DimensionListType& argDimensions,
	      const DimensionListType& resultDimensions,
	      const pistis::exceptions::ExceptionOrigin& origin
	  ) const {
	    const DimensionListType targetDimensions =
	        argDimensions.insert(argDimensions.size() - 1,
				     this->self().dimensions().back());
	    if (targetDimensions != resultDimensions) {
	      std::ostringstream msg;
	      msg << name << " has incorrect dimensions " << resultDimensions
		  << " -- it should have dimensions " << targetDimensions;
	      throw pistis::exceptions::IllegalValueError(msg.str(), origin);
	    }
	  }

	  void validateTransposeInnerProductArgDimensions_(
	      const std::string& name,
	      const DimensionListType& argDimensions,
	      const pistis::exceptions::ExceptionOrigin& origin
	  ) const {
	    if (this->self().dimensions().back(1) != argDimensions.front()) {
	      std::ostringstream msg;
	      msg << name << " has incorrect dimensions " << argDimensions
		  << " -- it should have dimensions [ "
		  << this->self().dimensions().back(1) << " ]";
	      throw pistis::exceptions::IllegalValueError(msg.str(), origin);
	    }
	  }

	  void validateTransposeInnerProductResultDimensions_(
	      const std::string& name,
	      const DimensionListType& argDimensions,
	      const DimensionListType& resultDimensions,
	      const pistis::exceptions::ExceptionOrigin& origin
	  ) const {
	    const size_t n = this->self().dimensions().size();
	    const DimensionListType targetDimensions =
	        this->self().dimensions().remove(n - 2);
	    
	    if (targetDimensions != resultDimensions) {
	      std::ostringstream msg;
	      msg << name << " has incorrect dimensions " << resultDimensions
		  << " -- it should have dimensions " << targetDimensions;
	      throw pistis::exceptions::IllegalValueError(msg.str(), origin);
	    }
	  }
	  
	};

      }
    }
  }
}
#endif

