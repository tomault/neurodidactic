#ifndef __NEURODIDACTIC__CORE__ARRAYS__DETAIL__MKLADAPTER_HPP__
#define __NEURODIDACTIC__CORE__ARRAYS__DETAIL__MKLADAPTER_HPP__

#include <stddef.h>
#include <mkl.h>

namespace neurodidactic {
  namespace core {
    namespace arrays {
      namespace detail {

	template <typename T, typename U = T>
	struct MklAdapter {
	  static_assert(sizeof(T) == 0,
			"Do not know how to combine arrays of these types");
	};

	template<>
	struct MklAdapter<float, float> {
	  static void add(size_t n, const float* x, const float* y,
			  float* result) {
	    vsAdd(n, x, y, result);
	  }

	  static void subtract(size_t n, const float* x, const float* y,
			       float* result) {
	    vsSub(n, x, y, result);
	  }

	  static void multiply(size_t n, const float* x, const float* y,
			       float* result) {
	    vsMul(n, x, y, result);
	  }

	  static void divide(size_t n, const float* x, const float* y,
			     float* result) {
	    vsDiv(n, x, y, result);
	  }

	  static void scale(size_t n, float c, float* x) {
	    cblas_sscal(n, c, x, 1);
	  }

	  static void scale(size_t n, float c, const float* x, float* y) {
	    cblas_saxpby(n, c, x, 1, 0.0, y, 1);
	  }

	  static void scaleAndAdd(size_t n, float c, const float* x,
				  float* y) {
	    cblas_saxpy(n, c, x, 1, y, 1);
	  }

	  static void scaleAndAdd(size_t n, float c1, const float* x,
				  float c2, float* y) {
	    cblas_saxpby(n, c1, x, 1, c2, y, 1);
	  }

	  static void exp(size_t n, const float* x, float* y) {
	    vsExp(n, x, y);
	  }

	  static float innerProduct(size_t n, const float* x, const float* y) {
	    return cblas_sdot(n, x, 1, y, 1);
	  }

	  static void outerProduct(size_t m, size_t n, const float* x,
				   const float* v, float* y) {
	    float* p = y;
	    for (size_t i = 0; i < m; ++i) {
	      cblas_saxpby(n, x[i], v, 1, 0.0f, p, 1);
	      p += n;
	    }
	  }
	  
	  static void multiplyMatrixByVector(size_t m, size_t n,
					     const float* x,
					     const float* v, float *y) {
	    cblas_sgemv(CblasRowMajor, CblasNoTrans, m, n, 1.0f, x, n,
			v, 1, 0.0f, y, 1);
	  }

	  static void multiplyMatrixTransposeByVector(size_t m, size_t n,
						      const float* x,
						      const float* v,
						      float* y) {
	    cblas_sgemv(CblasRowMajor, CblasTrans, m, n, 1.0f, x, n,
			v, 1, 0.0f, y, 1);
	  }

	  static void multiplyMatrixByMatrix(size_t m, size_t n, size_t k,
					     const float* x, const float* u,
					     float* y) {
	    cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, m, n, k,
			1.0, x, k, u, n, 0.0, y, n);
	  }

	};

	
      }
    }
  }
}
#endif
