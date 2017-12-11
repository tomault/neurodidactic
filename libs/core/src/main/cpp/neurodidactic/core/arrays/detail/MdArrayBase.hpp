#ifndef __NEURODIDACTIC__CORE__ARRAYS__DETAIL__MDARRAYBASE_HPP__
#define __NEURODIDACTIC__CORE__ARRAYS__DETAIL__MDARRAYBASE_HPP__

#include <neurodidactic/core/arrays/DimensionList.hpp>
#include <neurodidactic/core/arrays/detail/MdArrayCommon.hpp>
#include <pistis/exceptions/IllegalValueError.hpp>
#include <type_traits>

namespace neurodidactic {
  namespace core {
    namespace arrays {
      namespace detail {

	template <typename DerivedArray,
		  typename ArraySlice,
		  typename InnerProductResult,
		  size_t ARRAY_ORDER,
		  typename Field, typename Allocator>
	class MdArrayBase :
	    public MdArrayCommon<DerivedArray, ArraySlice, InnerProductResult,
				 ARRAY_ORDER, Field, Allocator>
	{
	protected:
	  typedef MdArrayCommon<DerivedArray, ArraySlice, InnerProductResult,
				ARRAY_ORDER, Field, Allocator>
	          CommonArrayBaseType_;
	  
	public:
	  using CommonArrayBaseType_::innerProduct;
	  using CommonArrayBaseType_::matrixProduct;
								  
	  template <typename Vector,
		    typename ResultArray,
		    typename Enabled =
		      typename std::enable_if<
		          IsMdArray<Vector>::value &&
		              IsMdArray<ResultArray>::value &&
		              (Vector::ORDER == 1),
	                  InnerProductResult	    
	              >::type
		   >
	  Enabled& innerProduct(const Vector& v, ResultArray& result) const {
	    this->validateInnerProductArgDimensions_("Vector \"v\"",
						     v.dimensions(),
						     PISTIS_EX_HERE);
	    this->validateInnerProductResultDimensions_("Array \"result\"",
							result.dimensions(),
							PISTIS_EX_HERE);
	    auto& myDimensions = this->self().dimensions();
	    size_t rows = myDimensions[myDimensions.size() - 2];
	    size_t columns = myDimensions[myDimensions.size() - 1];
	    size_t matrixSize = rows * columns;
	    const Field* p = this->self().data();
	    const Field* end = this->self().end();
	    Field* q = result.data();

	    for (; p < end; p += matrixSize, q += rows) {
	      detail::MklAdapter<Field, typename Vector::FieldType>::multiplyMatrixByVector(rows, columns, p, v.data(), q);
	    }
	    return result;
	  }

	  template <typename Matrix,
		    typename ResultArray,
		    typename Enabled =
		      typename std::enable_if<
		          IsMdArray<Matrix>::value &&
		              IsMdArray<ResultArray>::value &&
		              (Matrix::ORDER == 2) &&
	                      (ResultArray::ORDER == ARRAY_ORDER),
	                  DerivedArray	    
	              >::type
		   >
	  Enabled& matrixProduct(const Matrix& m, ResultArray& result) const {
	    typedef detail::MklAdapter<Field, typename Matrix::FieldType>
	            MklAdapter;
	    this->validateMatrixProductArgDimensions_("Matrix \"m\"",
						      m.dimensions(),
						      PISTIS_EX_HERE);
	    this->validateMatrixProductResultDimensions_("Array \"result\"",
							 m.dimensions(),
							 result.dimensions(),
							 PISTIS_EX_HERE);
	    auto& myDimensions = this->self().dimensions();
	    auto& argDimensions = m.dimensions();
	    size_t myRows = myDimensions[myDimensions.size() - 2];
	    size_t myColumns = myDimensions[myDimensions.size() - 1];
	    size_t argColumns = argDimensions.back();
	    size_t myMatrixSize = myRows * myColumns;
	    size_t resultMatrixSize = myRows * argColumns;
	    const Field* p = this->self().data();
	    const Field* end = this->self().end();
	    Field* q = result.data();

	    for (; p < end; p += myMatrixSize, q += resultMatrixSize) {
	      MklAdapter::multiplyMatrixByMatrix(
	          myRows, argColumns, myColumns, p, m.data(), q
	      );
	    }
	    return result;
	  }

	  const ArraySlice operator[](size_t n) const {
	    return this->self().slice_(n);
	  }
	  
	  ArraySlice operator[](size_t n) {
	    return this->self().slice_(n);
	  }
	};

	template <typename DerivedArray, typename ArraySlice,
		  typename InnerProductResult, typename Field,
		  typename Allocator>
	class MdArrayBase<DerivedArray, ArraySlice, InnerProductResult,
			  2, Field, Allocator> :
	    public MdArrayCommon<DerivedArray, ArraySlice, InnerProductResult,
				 2, Field, Allocator>
	{
	protected:
	  typedef MdArrayCommon<DerivedArray, ArraySlice, InnerProductResult,
				2, Field, Allocator>
	          CommonArrayBaseType_;
	  
	public:
	  using CommonArrayBaseType_::innerProduct;
	  using CommonArrayBaseType_::matrixProduct;
	  
	  template <typename Vector,
		    typename ResultArray,
		    typename Enabled =
		      typename std::enable_if<
		          IsMdArray<Vector>::value &&
		              IsMdArray<ResultArray>::value &&
		              (Vector::ORDER == 1),
	                  InnerProductResult	    
	              >::type
		   >
	  Enabled& innerProduct(const Vector& v, ResultArray& result) const {
	    typedef detail::MklAdapter<Field, typename Vector::FieldType>
	            MklAdapter;
	    this->validateInnerProductArgDimensions_("Vector \"v\"",
						     v.dimensions(),
						     PISTIS_EX_HERE);
	    this->validateInnerProductResultDimensions_("Array \"result\"",
							result.dimensions(),
							PISTIS_EX_HERE);
	    auto& myDimensions = this->self().dimensions();
	    size_t rows = myDimensions[myDimensions.size() - 2];
	    size_t columns = myDimensions[myDimensions.size() - 1];

	    MklAdapter::multiplyMatrixByVector(
		rows, columns, this->self().data(), v.data(), result.data()
	    );
	    return result;
	  }
	  
	  template <typename Matrix,
		    typename ResultArray,
		    typename Enabled =
		      typename std::enable_if<
		          IsMdArray<Matrix>::value &&
		              IsMdArray<ResultArray>::value &&
		              (Matrix::ORDER == 2) &&
	                      (ResultArray::ORDER == 2),
	                  DerivedArray	    
	              >::type
		   >
	  Enabled& matrixProduct(const Matrix& m, ResultArray& result) const {
	    typedef detail::MklAdapter<Field, typename Matrix::FieldType>
	            MklAdapter;
	    this->validateMatrixProductArgDimensions_("Matrix \"m\"",
						      m.dimensions(),
						      PISTIS_EX_HERE);
	    this->validateMatrixProductResultDimensions_("Array \"result\"",
							 m.dimensions(),
							 result.dimensions(),
							 PISTIS_EX_HERE);
	    auto& myDimensions = this->self().dimensions();
	    auto& argDimensions = m.dimensions();

	    MklAdapter::multiplyMatrixByMatrix(
	         myDimensions[0], argDimensions[1], myDimensions[1],
		 this->self().data(), m.data(), result.data()
	    );
	    return result;
	  }

	  const ArraySlice operator[](size_t n) const {
	    return this->self().slice_(n);
	  }
	  
	  ArraySlice operator[](size_t n) {
	    return this->self().slice_(n);
	  }

	};


	template <typename DerivedArray, typename ArraySlice,
		  typename Field, typename Allocator>
	class MdArrayBase<DerivedArray, ArraySlice, Field, 1, Field,
			  Allocator> :
	    public MdArrayCommon<DerivedArray, ArraySlice, Field,
				 1, Field, Allocator>
	{
	protected:
	  typedef MdArrayCommon<DerivedArray, ArraySlice, Field,
				1, Field, Allocator>
	          CommonArrayBaseType_;
	  
	public:
	  using CommonArrayBaseType_::matrixProduct;

	  template <typename Vector,
		    typename Enabled =
		        typename std::enable_if<
		            IsMdArray<Vector>::value && (Vector::ORDER == 1),
	                    Field
	                 >::type
		   >
	  Field innerProduct(const Vector& v) const {
	    typedef detail::MklAdapter<Field, typename Vector::FieldType>
	            MklAdapter;
	    this->validateDimensions_("Vector \"v\"", v.dimensions(),
				      PISTIS_EX_HERE);
	    return MklAdapter::innerProduct(this->self().size(),
					     this->self().data(),
					     v.data());
	  }
	          
	  template <typename Vector,
		    typename ResultArray,
		    typename Enabled =
		        typename std::enable_if<
		            IsMdArray<Vector>::value &&
		                IsMdArray<ResultArray>::value &&
		                (Vector::ORDER == 1),
	                    Field	    
	                >::type
		   >
	  Enabled& innerProduct(const Vector& v, Field& result) const {
	    typedef detail::MklAdapter<Field, typename Vector::FieldType>
	            MklAdapter;
	    this->validateDimensions_("Vector \"v\"", v.dimensions(),
				      PISTIS_EX_HERE);
	    result = MklAdapter::innerProduct(this->self().size(),
					      this->self().data(),
					      v.data());
	    return result;
	  }

	  template <typename Matrix,
		    typename ResultVector,
		    typename Enabled =
		        typename std::enable_if<
		            IsMdArray<Matrix>::value &&
		                IsMdArray<ResultVector>::value &&
		                (Matrix::ORDER == 2) &&
	                        (ResultVector::ORDER == 1),
	                    ResultVector
	                >::type
		   >
	  Enabled& matrixProduct(const Matrix& m,
				 ResultVector& result) const {
	    typedef detail::MklAdapter<Field, typename Matrix::FieldType>
	            MklAdapter;
	    this->validateMatrixProductArgDimensions_("Matrix \"m\"",
						      m.dimensions(),
						      PISTIS_EX_HERE);
	    this->validateMatrixProductResultDimensions_("Vector \"result\"",
							 m.dimensions(),
							 result.dimensions(),
							 PISTIS_EX_HERE);

	    auto& myDimensions = this->dimensions();
	    auto& argDimensions = m.dimensions();

	    MklAdapter::multiplyMatrixTransposeByVector(
		argDimensions[0], argDimensions[1], m.data(),
		this->self().data(), result.data()
	    );
	    
	    return result;
	  }

	  Field operator[](size_t n) const { return this->data()[n]; }
	  Field& operator[](size_t n) { return this->data()[n]; }
	};
	
      }
    }
  }
}
#endif

