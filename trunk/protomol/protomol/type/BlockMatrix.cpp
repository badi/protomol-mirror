#include <protomol/type/BlockMatrix.h>

#if defined (HAVE_LAPACK)
#include <protomol/integrator/hessian/LapackProtomol.h>
#else
#if defined (HAVE_SIMTK_LAPACK)
#include "SimTKlapack.h"
#endif
#endif

using namespace std;
using namespace ProtoMol::Report;
using namespace ProtoMol;

namespace ProtoMol
{

  // Constructors/distructors~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  BlockMatrix::BlockMatrix() :
      RowStart ( 0 ), ColumnStart ( 0 ), Rows ( 0 ), Columns ( 0 ), arraySize( 0 )
  {
  }

  BlockMatrix::BlockMatrix( unsigned int rowStart, unsigned int columnStart, unsigned int rows, unsigned int columns )
  {
    RowStart    = rowStart;
    ColumnStart = columnStart;

    Rows    = rows;
    Columns = columns;

    arraySize = Rows * Columns;
    MyArray.resize( arraySize );
  }

  BlockMatrix::~BlockMatrix() {};

  // Methods~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  void BlockMatrix::initialize( unsigned int rowStart, unsigned int columnStart, unsigned int rows, unsigned int columns )
  {
    RowStart    = rowStart;
    ColumnStart = columnStart;

    Rows    = rows;
    Columns = columns;

    arraySize = Rows * Columns;
    MyArray.resize( arraySize );
  }

  void BlockMatrix::clear()
  {
    std::fill( MyArray.begin(), MyArray.end(), 0.0 );
  };

  // Pointer to data
  double * BlockMatrix::arrayPointer()
  {
    return &MyArray[0];
  }

  // Move block
  void BlockMatrix::blockMove( unsigned int newRowStart, unsigned int newColumnStart )
  {
    RowStart    = newRowStart;
    ColumnStart = newColumnStart;
  }

  // Re-size by columns
  void BlockMatrix::columnResize( unsigned int newColumns )
  {
    Columns   = newColumns;
    arraySize = Rows * Columns;
    MyArray.resize( arraySize );
  }

  // Add 'this' with 'bm', return result
  const BlockMatrix BlockMatrix::operator+( const BlockMatrix &bm ) const
  {
    //find bounds
    unsigned int rl = max( RowStart, bm.RowStart );
    unsigned int rh = min( RowStart + Rows, bm.RowStart + bm.Rows );
    unsigned int cl = max( ColumnStart, bm.ColumnStart );
    unsigned int ch = min( ColumnStart + Columns, bm.ColumnStart + bm.Columns );

    BlockMatrix om( rl, cl, rh - rl, ch - cl );

    for ( unsigned int i = rl; i < rh; ++i ) {
      const int iTempStart  = i - om.RowStart;
      const int iOtherStart = i - bm.RowStart;
      const int iThisStart  = i -    RowStart;

      for ( unsigned int j = cl; j < ch; ++j ) {
        om.MyArray[iTempStart + ( j - om.ColumnStart ) * om.Rows] =
          MyArray[iThisStart  + ( j - ColumnStart ) * Rows] +
          bm.MyArray[iOtherStart + ( j - bm.ColumnStart ) * bm.Rows];
      }
    }

    return om;
  }

  // Add 'this' with 'bm', return result
  BlockMatrix &BlockMatrix::operator+=( const BlockMatrix &bm )
  {
    //find bounds
    unsigned int rl = max( RowStart, bm.RowStart );
    unsigned int rh = min( RowStart + Rows, bm.RowStart + bm.Rows );
    unsigned int cl = max( ColumnStart, bm.ColumnStart );
    unsigned int ch = min( ColumnStart + Columns, bm.ColumnStart + bm.Columns );

    for ( unsigned int i = rl; i < rh; ++i ) {
      const int iOtherStart = i - bm.RowStart;
      const int iThisStart  = i -    RowStart;

      for ( unsigned int j = cl; j < ch; ++j ) {
        MyArray[iThisStart + ( j - ColumnStart ) * Rows] +=
          bm.MyArray[iOtherStart + ( j - bm.ColumnStart ) * bm.Rows];
      }
    }

    return *this;
  }

  // Add 'this' with 'bm', put result in 'om'
  void BlockMatrix::add( const BlockMatrix &bm, BlockMatrix &om ) const
  {
    //find bounds
    unsigned int rl = max( max( RowStart, bm.RowStart ), om.RowStart );
    unsigned int rh = min( min( RowStart + Rows, bm.RowStart + bm.Rows ), om.RowStart + om.Rows );
    unsigned int cl = max( max( ColumnStart, bm.ColumnStart ), om.ColumnStart );
    unsigned int ch = min( min( ColumnStart + Columns, bm.ColumnStart + bm.Columns ), om.ColumnStart + om.Columns );

    for ( unsigned int i = rl; i < rh; ++i ) {
      const int iTempStart  = i - om.RowStart;
      const int iOtherStart = i - bm.RowStart;
      const int iThisStart  = i -    RowStart;

      for ( unsigned int j = cl; j < ch; ++j ) {
        om.MyArray[iTempStart + ( j - om.ColumnStart ) * om.Rows] =
          MyArray[iThisStart  + ( j - ColumnStart ) * Rows] +
          bm.MyArray[iOtherStart + ( j - bm.ColumnStart ) * bm.Rows];
      }
    }
  }

  // Multiply 'this' with 'bm', return result: TEST A, C
  const BlockMatrix BlockMatrix::operator*( const BlockMatrix &bm ) const
  {
    unsigned int kl = max( ColumnStart, bm.RowStart );
    unsigned int kh = min( ColumnStart + Columns, bm.RowStart + bm.Rows );

    BlockMatrix om( RowStart, bm.ColumnStart, Rows, bm.Columns );

#if defined(HAVE_LAPACK) || defined(HAVE_SIMTK_LAPACK)
    char *transA = ( char * )"N"; char *transB = ( char * )"N";
    int m = Rows; int n = bm.Columns; int k = kh - kl;
    int lda = Rows; int ldb = bm.Rows; int ldc = om.Rows;
    double alpha = 1.0; double beta = 0.0;
#endif

#if defined(HAVE_LAPACK)
    dgemm_ ( transA, transB, &m, &n, &k, &alpha, ( double * )&MyArray[( kl-ColumnStart )*Rows], &lda, ( double * )&bm.MyArray[kl - bm.RowStart],
             &ldb, &beta, &om.MyArray[( RowStart - om.RowStart ) + ( bm.ColumnStart - om.ColumnStart ) * om.Rows], &ldc );
#elif defined(HAVE_SIMTK_LAPACK)
    int len_trans = 1;
    dgemm_ ( *transA, *transB, m, n, k, alpha, ( double * )&MyArray[( kl-ColumnStart )*Rows], lda, ( double * )&bm.MyArray[kl - bm.RowStart],
             ldb, beta, &om.MyArray[( RowStart - om.RowStart ) + ( bm.ColumnStart - om.ColumnStart ) * om.Rows], ldc, len_trans, len_trans );
#else
    const unsigned int rowLength = RowStart + Rows;
    const unsigned int otherColumnLength = bm.ColumnStart + bm.Columns;

    for ( unsigned int i = RowStart; i < rowLength; ++i ) {
      const int iThisRowStart  = i - RowStart;
      const int iOtherRowStart = i - om.RowStart;

      for ( unsigned int j = bm.ColumnStart; j < otherColumnLength; ++j ) {
        Real tempf = 0.0;

        for ( unsigned int k = kl; k < kh; ++k ) {
          tempf += MyArray[ iThisRowStart + ( k - ColumnStart ) * Rows] *
                   bm.MyArray[( k - bm.RowStart ) + ( j - bm.ColumnStart ) * bm.Rows];
        }

        om.MyArray[ iOtherRowStart + ( j - om.ColumnStart ) * om.Rows] = tempf; //.at( changed to [] for efficiency
      }
    }
#endif

    return om;
  }

  // Multiply 'this' with 'bm', put result in 'om': TEST A, B, C
  void BlockMatrix::product( const BlockMatrix &bm, BlockMatrix &om ) const
  {
    unsigned int kl = max( ColumnStart, bm.RowStart );
    unsigned int kh = min( ColumnStart + Columns, bm.RowStart + bm.Rows );

    if ( om.RowStart > RowStart || om.ColumnStart > bm.ColumnStart ||
         om.RowStart + om.Rows < RowStart + Rows || om.ColumnStart + om.Columns < bm.ColumnStart + bm.Columns )
      Report::report << Report::error << "[BlockMatrix::product] Target Block Matrix too small." << Report::endr;

#if defined(HAVE_LAPACK) || defined(HAVE_SIMTK_LAPACK)
    char *transA = ( char * )"N"; char *transB = ( char * )"N";
    int m = Rows; int n = bm.Columns; int k = kh - kl;
    int lda = Rows; int ldb = bm.Rows; int ldc = om.Rows;
    double alpha = 1.0; double beta = 0.0;
#endif

#if defined(HAVE_LAPACK)
    dgemm_ ( transA, transB, &m, &n, &k, &alpha, ( double * )&MyArray[( kl-ColumnStart )*Rows], &lda, ( double * )&bm.MyArray[kl - bm.RowStart],
             &ldb, &beta, &om.MyArray[( RowStart - om.RowStart ) + ( bm.ColumnStart - om.ColumnStart ) * om.Rows], &ldc );
#elif defined(HAVE_SIMTK_LAPACK)
    int len_trans = 1;
    dgemm_ ( *transA, *transB, m, n, k, alpha, ( double * )&MyArray[( kl-ColumnStart )*Rows], lda, ( double * )&bm.MyArray[kl - bm.RowStart],
             ldb, beta, &om.MyArray[( RowStart - om.RowStart ) + ( bm.ColumnStart - om.ColumnStart ) * om.Rows], ldc, len_trans, len_trans );
#else
    const unsigned int rowLength = RowStart + Rows;
    const unsigned int otherColumnLength = bm.ColumnStart + bm.Columns;

    for ( unsigned int i = RowStart; i < rowLength; ++i ) {
      const int iThisRowStart  = i - RowStart;
      const int iOtherRowStart = i - om.RowStart;

      for ( unsigned int j = bm.ColumnStart; j < otherColumnLength; ++j ) {
        Real tempf = 0.0;

        for ( unsigned int k = kl; k < kh; ++k ) {
          tempf += MyArray[ iThisRowStart + ( k - ColumnStart ) * Rows] *
                   bm.MyArray[( k - bm.RowStart ) + ( j - bm.ColumnStart ) * bm.Rows];
        }

        om.MyArray[ iOtherRowStart + ( j - om.ColumnStart ) * om.Rows] = tempf; //.at( changed to [] for efficiency
      }
    }
#endif
  }

  // Multiply 'this' with 'bm', put result in double array 'om': TEST A, B, C
  void BlockMatrix::productToArray( const BlockMatrix &bm,
                                    double *om_MyArray, unsigned int om_RowStart, unsigned int om_ColumnStart,
                                    unsigned int om_Rows, unsigned int om_Columns ) const
  {
    unsigned int kl = max( ColumnStart, bm.RowStart );
    unsigned int kh = min( ColumnStart + Columns, bm.RowStart + bm.Rows );

    if ( om_RowStart > RowStart || om_ColumnStart > bm.ColumnStart ||
         om_RowStart + om_Rows < RowStart + Rows || om_ColumnStart + om_Columns < bm.ColumnStart + bm.Columns )
      Report::report << Report::error << "[BlockMatrix::product] Target Block Matrix too small." << Report::endr;

#if defined(HAVE_LAPACK) || defined(HAVE_SIMTK_LAPACK)
    char *transA = ( char * )"N"; char *transB = ( char * )"N";
    int m = Rows; int n = bm.Columns; int k = kh - kl;
    int lda = Rows; int ldb = bm.Rows; int ldc = om_Rows;
    double alpha = 1.0; double beta = 0.0;
#endif

#if defined(HAVE_LAPACK)
    dgemm_ ( transA, transB, &m, &n, &k, &alpha, ( double * )&MyArray[( kl-ColumnStart )*Rows], &lda, ( double * )&bm.MyArray[kl - bm.RowStart],
             &ldb, &beta, &om_MyArray[( RowStart - om_RowStart ) + ( bm.ColumnStart - om_ColumnStart ) * om_Rows], &ldc );
#elif defined(HAVE_SIMTK_LAPACK)
    int len_trans = 1;
    dgemm_ ( *transA, *transB, m, n, k, alpha, ( double * )&MyArray[( kl-ColumnStart )*Rows], lda, ( double * )&bm.MyArray[kl - bm.RowStart],
             ldb, beta, &om_MyArray[( RowStart - om_RowStart ) + ( bm.ColumnStart - om_ColumnStart ) * om_Rows], ldc, len_trans, len_trans );
#else
    const unsigned int rowLength = RowStart + Rows;
    const unsigned int otherColumnLength = bm.ColumnStart + bm.Columns;

    for ( unsigned int i = RowStart; i < rowLength; ++i ) {
      const int iThisRowStart  = i - RowStart;
      const int iOtherRowStart = i - om_RowStart;

      for ( unsigned int j = bm.ColumnStart; j < otherColumnLength; ++j ) {
        Real tempf = 0.0;

        for ( unsigned int k = kl; k < kh; ++k ) {
          tempf += MyArray[ iThisRowStart + ( k - ColumnStart ) * Rows] *
                   bm.MyArray[( k - bm.RowStart ) + ( j - bm.ColumnStart ) * bm.Rows];
        }

        om_MyArray[ iOtherRowStart + ( j - om_ColumnStart ) * om_Rows] = tempf; //.at( changed to [] for efficiency
      }
    }
#endif
  }

  // Multiply 'this' with 'bm', sum result in 'om': TEST A, B, C
  void BlockMatrix::sumProduct( const BlockMatrix &bm, BlockMatrix &om ) const
  {
    unsigned int kl = max( ColumnStart, bm.RowStart );
    unsigned int kh = min( ColumnStart + Columns, bm.RowStart + bm.Rows );

    if ( om.RowStart > RowStart || om.ColumnStart > bm.ColumnStart ||
         om.RowStart + om.Rows < RowStart + Rows || om.ColumnStart + om.Columns < bm.ColumnStart + bm.Columns )
      Report::report << Report::error << "[BlockMatrix::product] Target Block Matrix too small." << Report::endr;

#if defined(HAVE_LAPACK) || defined(HAVE_SIMTK_LAPACK)
    char *transA = ( char * )"N"; char *transB = ( char * )"N";
    int m = Rows; int n = bm.Columns; int k = kh - kl;
    int lda = Rows; int ldb = bm.Rows; int ldc = om.Rows;
    double alpha = 1.0; double beta = 1.0;
#endif

#if defined(HAVE_LAPACK)
    dgemm_ ( transA, transB, &m, &n, &k, &alpha, ( double * )&MyArray[( kl-ColumnStart )*Rows], &lda, ( double * )&bm.MyArray[kl - bm.RowStart],
             &ldb, &beta, &om.MyArray[( RowStart - om.RowStart ) + ( bm.ColumnStart - om.ColumnStart ) * om.Rows], &ldc );
#elif defined(HAVE_SIMTK_LAPACK)
    int len_trans = 1;
    dgemm_ ( *transA, *transB, m, n, k, alpha, ( double * )&MyArray[( kl-ColumnStart )*Rows], lda, ( double * )&bm.MyArray[kl - bm.RowStart],
             ldb, beta, &om.MyArray[( RowStart - om.RowStart ) + ( bm.ColumnStart - om.ColumnStart ) * om.Rows], ldc, len_trans, len_trans );
#else
    const unsigned int rowLength = RowStart + Rows;
    const unsigned int otherColumnLength = bm.ColumnStart + bm.Columns;

    for ( unsigned int i = RowStart; i < rowLength; ++i ) {
      const int iThisRowStart  = i - RowStart;
      const int iOtherRowStart = i - om.RowStart;

      for ( unsigned int j = bm.ColumnStart; j < otherColumnLength; ++j ) {
        Real tempf = 0.0;

        for ( unsigned int k = kl; k < kh; ++k ) {
          tempf += MyArray[ iThisRowStart + ( k - ColumnStart ) * Rows] *
                   bm.MyArray[( k - bm.RowStart ) + ( j - bm.ColumnStart ) * bm.Rows];
        }

        om.MyArray[ iOtherRowStart + ( j - om.ColumnStart ) * om.Rows] += tempf; //.at( changed to [] for efficiency
      }
    }
#endif
  }

  // Multiply transpose of 'this' with 'bm', put result in 'om': TEST A, B, C
  void BlockMatrix::transposeProduct( const BlockMatrix &bm, BlockMatrix &om ) const
  {
    unsigned int kl = max( RowStart, bm.RowStart );
    unsigned int kh = min( RowStart + Rows, bm.RowStart + bm.Rows );

    if ( om.RowStart > ColumnStart || om.ColumnStart > bm.ColumnStart ||
         om.RowStart + om.Rows < ColumnStart + Columns || om.ColumnStart + om.Columns < bm.ColumnStart + bm.Columns )
      Report::report << Report::error << "[BlockMatrix::transposeProduct] Target Block Matrix too small." << Report::endr;

#if defined(HAVE_LAPACK) || defined(HAVE_SIMTK_LAPACK)
    char *transA = ( char * )"T"; char *transB = ( char * )"N";
    int m = Columns; int n = bm.Columns; int k = kh - kl;//
    int lda = Rows; int ldb = bm.Rows; int ldc = om.Rows;//
    double alpha = 1.0; double beta = 0.0;
#endif

#if defined(HAVE_LAPACK)
    dgemm_ ( transA, transB, &m, &n, &k, &alpha, ( double * )&MyArray[( kl-RowStart )], &lda, ( double * )&bm.MyArray[kl - bm.RowStart],
             &ldb, &beta, &om.MyArray[( ColumnStart - om.RowStart ) + ( bm.ColumnStart - om.ColumnStart ) * om.Rows], &ldc );
#elif defined(HAVE_SIMTK_LAPACK)
    int len_trans = 1;
    dgemm_ ( *transA, *transB, m, n, k, alpha, &MyArray[kl-RowStart], lda, ( double * )&bm.MyArray[kl - bm.RowStart],
             ldb, beta, &om.MyArray[( ColumnStart - om.RowStart ) + ( bm.ColumnStart - om.ColumnStart ) * om.Rows], ldc, len_trans, len_trans );
#else
    const unsigned int columnLength = ColumnStart + Columns;
    const unsigned int otherColumnLength = bm.ColumnStart + bm.Columns;

    for ( unsigned int i = ColumnStart; i < columnLength; ++i ) {
      const int iColumnStart = i - ColumnStart;
      const int iRowStart    = i - om.RowStart;

      for ( unsigned int j = bm.ColumnStart; j < otherColumnLength; ++j ) {
        Real tempf = 0.0;
        for ( unsigned int k = kl; k < kh; ++k ) {
          tempf += MyArray[ iColumnStart * Rows + ( k - RowStart ) ] *
                   bm.MyArray[( k - bm.RowStart ) + ( j - bm.ColumnStart ) * bm.Rows];
        }
        om.MyArray.at( iRowStart + ( j - om.ColumnStart ) * om.Rows ) = tempf; //.at( changed to [] for efficiency
      }
    }
#endif
  }

  // Multiply transpose of 'this' with 'bm', return result: TEST C
  const BlockMatrix BlockMatrix::operator/( const BlockMatrix &bm ) const
  {
    unsigned int kl = max( RowStart, bm.RowStart );
    unsigned int kh = min( RowStart + Rows, bm.RowStart + bm.Rows );

    BlockMatrix om( ColumnStart, bm.ColumnStart, Columns, bm.Columns );

#if defined(HAVE_LAPACK) || defined(HAVE_SIMTK_LAPACK)
    char *transA = ( char * )"T"; char *transB = ( char * )"N";
    int m = Columns; int n = bm.Columns; int k = kh - kl;
    int lda = Rows; int ldb = bm.Rows; int ldc = om.Rows;
    double alpha = 1.0; double beta = 0.0;
#endif

#if defined(HAVE_LAPACK)
    dgemm_ ( transA, transB, &m, &n, &k, &alpha, ( double * )&MyArray[( kl-RowStart )], &lda, ( double * )&bm.MyArray[kl - bm.RowStart],
             &ldb, &beta, &om.MyArray[( RowStart - om.RowStart ) + ( bm.ColumnStart - om.ColumnStart ) * om.Rows], &ldc );
#elif defined(HAVE_SIMTK_LAPACK)
    int len_trans = 1;
    dgemm_ ( *transA, *transB, m, n, k, alpha, &MyArray[kl-RowStart], lda, ( double * )&bm.MyArray[kl - bm.RowStart],
             ldb, beta, &om.MyArray[( RowStart - om.RowStart ) + ( bm.ColumnStart - om.ColumnStart ) * om.Rows], ldc, len_trans, len_trans );
#else
    const unsigned int columnLength = ColumnStart + Columns;
    const unsigned int otherColumnLength = bm.ColumnStart + bm.Columns;

    for ( unsigned int i = ColumnStart; i < columnLength; ++i ) {
      const int iColumnStart = i - ColumnStart;
      const int iRowStart    = i - om.RowStart;

      for ( unsigned int j = bm.ColumnStart; j < otherColumnLength; ++j ) {
        Real tempf = 0.0;

        for ( unsigned int k = kl; k < kh; ++k ) {
          tempf += MyArray[iColumnStart * Rows + ( k - RowStart ) ] *
                   bm.MyArray[( k - bm.RowStart ) + ( j - bm.ColumnStart ) * bm.Rows];
        }

        om.MyArray[iRowStart + ( j - om.ColumnStart ) * om.Rows] = tempf;
      }
    }
#endif

    return om;
  }

  // Get sub-matrix: TEST A
  const BlockMatrix BlockMatrix::subMatrix( unsigned int atRow, unsigned int atColumn,
      unsigned int getRows, unsigned int getColumns ) const
  {
    //create matrix
    BlockMatrix om( atRow, atColumn, getRows, getColumns );
    om.clear();

    const unsigned int atRowSize = atRow + getRows;
    const unsigned int atColSize = atColumn + getColumns;

    if ( atRow >= RowStart && atRowSize <= RowStart + Rows &&
         atColumn >= ColumnStart && atColSize <= ColumnStart + Columns ) {

      for ( unsigned int i = atRow; i < atRowSize; ++i ) {
        const int iTempStart  = i - om.RowStart;
        const int iThisStart  = i -    RowStart;

        for ( unsigned int j = atColumn; j < atColSize; ++j ) {
          om.MyArray.at( iTempStart + ( j - om.ColumnStart ) * om.Rows ) =
            MyArray[iThisStart + ( j - ColumnStart ) * Rows];
        }
      }
    }
    return om;
  }

  // Operators~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  // Double Index access
  double & BlockMatrix::operator()( int rowIndex, int colIndex )
  {
    if ( rowIndex < RowStart || rowIndex > RowStart + Rows || colIndex < ColumnStart || colIndex > ColumnStart + Columns ) {
      Report::report << Report::error << "[BlockMatrix::operator(,)] Index out of range." << Report::endr;
    }

    return MyArray[( rowIndex - RowStart ) + ( colIndex - ColumnStart ) * Rows];
  }

  // Index access
  double & BlockMatrix::operator[]( int index )
  {
    if ( index < 0 || index > arraySize ) {
      Report::report << Report::error << "[BlockMatrix::operator[]] Index out of range." << Report::endr;
    }

    return MyArray[index];
  }

  // Equals
  BlockMatrix & BlockMatrix::operator=( const BlockMatrix & bm )
  {
    arraySize = bm.arraySize;

    MyArray.resize( arraySize );

    std::copy ( bm.MyArray.begin(), bm.MyArray.end(), MyArray.begin() );

    RowStart    = bm.RowStart;
    ColumnStart = bm.ColumnStart;

    Rows    = bm.Rows;
    Columns = bm.Columns;

    return *this;
  }
}