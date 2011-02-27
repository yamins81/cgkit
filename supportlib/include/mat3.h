/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is the Python Computer Graphics Kit.
 *
 * The Initial Developer of the Original Code is Matthias Baas.
 * Portions created by the Initial Developer are Copyright (C) 2004
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */

/*
  ---------------------------------------------------------------------
  Änderungen:
  ---------------------------------------------------------------------
  06.08.99: Template-Klammern in der Deklaration eingefügt (innerhalb 
            der mat3-Klasse):

  friend vec3<T> operator*<>(const vec3<T>& v, const mat3<T>& M);
                          ^^ 
                 Diese Klammern eingefügt

  egcs hatte sich über die fehlenden Klammern beschwert (C++ Builder
  hatte damit keine Probleme).
  ---------------------------------------------------------------------

 */

///////////////////////////////////////////////////////////////////
#ifndef MAT3_H
#define MAT3_H

/**
   \file mat3.h

   Contains the declaration of mat3, a 3x3 matrix with 
   components of type T. 
 */

#include <iostream>
#include <algorithm>
#include "compile_switches.h"
//#include "xmath.h"
#include "vec3.h"

namespace support3d {

/** 
   3x3 matrix with components of type T.

   mat3 is a 3x3 template matrix with components of type T. Some
   matrix methods assume that T is a floating point type (float,
   double or long double). If the matrix is used with components of
   type int then those methods won't work properly (e.g. matrix
   inversion).

   Math operations:

   \verbatim

   Negation        matrix  = -matrix
   Addition        matrix  = matrix+matrix
                   matrix += matrix
   Subtraction     matrix  = matrix-matrix
                   matrix -= matrix
   Multiplication  matrix  = matrix*matrix
                   matrix *= matrix
                   matrix  = matrix*scalar
                   matrix  = scalar*matrix
                   matrix *= scalar
                   vector  = matrix*vector
                   vector  = vector*matrix
   Division        matrix  = matrix/scalar         (can throw an exception!)
                   matrix /= scalar                (can throw an exception!)
   Transpose       matrix  = matrix.transpose()
                   matrix.transpose(destination)
   Invert          matrix  = matrix.invert()       (can throw an exception!)
                   matrix.invert(destination)      (can throw an exception!)
   Determinant     scalar  = matrix.det()

   Comparison      matrix == matrix
                   matrix != matrix
   \endverbatim

   @author Matthias Baas 
*/
template<class T> 
class mat3
{
  public:
  // Constructors
  mat3();
  explicit mat3(T v);
  mat3(T a, T b, T c,  T d, T e, T f,  T g, T h, T i);
  mat3(const mat3<T>& A);

  T& at(short i, short j);
  const T& at(short i, short j) const;

  // set_ and get_ methods
  mat3<T>& setIdentity();
  mat3<T>& setNull();
  mat3<T>& setRow(short i, const vec3<T>& r);
  mat3<T>& setRow(short i, const T a, const T b, const T c);
  mat3<T>& setColumn(short i, const vec3<T>& c);
  mat3<T>& setColumn(short i, const T a, const T b, const T c);
  mat3<T>& setDiag(const vec3<T>& d);
  mat3<T>& setDiag(T a, T b, T c);
  vec3<T>  getRow(short i) const;
  void     getRow(short i, vec3<T>& dest) const;
  void     getRow(short i, T& a, T& b, T& c) const;
  vec3<T>  getColumn(short i) const;
  void     getColumn(short i, vec3<T>& dest) const;
  void     getColumn(short i, T& a, T& b, T& c) const;
  vec3<T>  getDiag() const;
  void     getDiag(vec3<T>& dest) const;
  void     getDiag(T& a, T& b, T& c) const;

  mat3<T>& setRotation(T angle, const vec3<T>& axis);
  mat3<T>& setScaling(const vec3<T>& s);

  mat3<T>& setRotationZXY(T x, T y, T z);
  mat3<T>& setRotationYXZ(T x, T y, T z);
  mat3<T>& setRotationXYZ(T x, T y, T z);
  mat3<T>& setRotationXZY(T x, T y, T z);
  mat3<T>& setRotationYZX(T x, T y, T z);
  mat3<T>& setRotationZYX(T x, T y, T z);
  void getRotationZXY(T& x, T& y, T& z) const;
  void getRotationYXZ(T& x, T& y, T& z) const;
  void getRotationXYZ(T& x, T& y, T& z) const;
  void getRotationXZY(T& x, T& y, T& z) const;
  void getRotationYZX(T& x, T& y, T& z) const;
  void getRotationZYX(T& x, T& y, T& z) const;

  mat3<T>& fromToRotation(const vec3<T>& from, const vec3<T>& to);

  // Operators
  mat3<T>& operator+=(const mat3<T>& A);       // matrix += matrix
  mat3<T>& operator-=(const mat3<T>& A);       // matrix -= matrix
  mat3<T>& operator*=(const mat3<T>& A);       // matrix *= matrix
  mat3<T>& operator*=(const T s);              // matrix *= scalar
  mat3<T>& operator/=(const T s);              // matrix /= scalar
  mat3<T>& operator%=(const T r);
  mat3<T>& operator%=(const mat3<T>& b);

  mat3<T> operator+(const mat3<T>& A) const;   // matrix = matrix + matrix
  mat3<T> operator-(const mat3<T>& A) const;   // matrix = matrix - matrix
  mat3<T> operator-() const;                   // matrix = -matrix

  mat3<T> operator*(const mat3<T>& A) const;   // matrix = matrix * matrix
  vec3<T> operator*(const vec3<T>& v) const;   // vector = matrix * vector
  mat3<T> operator*(const T s) const;          // matrix = matrix * scalar

  mat3<T> operator/(const T s) const;          // matrix = matrix / scalar
  mat3<T> operator%(const T b) const;          // mat = mat % scalar (each component)
  mat3<T> operator%(const mat3<T>& b) const;   // mat = mat % mat

  bool operator==(const mat3<T>& A) const;     // matrix == matrix
  bool operator!=(const mat3<T>& A) const;     // matrix == matrix

  void toList(T* dest, bool rowmajor=false) const;

  // determinant
  T determinant() const;

  // Inversion (*this is never changed, unless dest = *this)
  mat3<T>  inverse() const;
  mat3<T>& inverse(mat3<T>& dest) const;

  // Transposition
  mat3<T>  transpose() const;
  mat3<T>& transpose(mat3<T>& dest) const;

  mat3<T>& scale(const vec3<T>& s);
  mat3<T>& rotate(T angle, const vec3<T>& axis);

  mat3<T>& ortho(mat3<T>& dest) const;
  mat3<T> ortho() const;

  void decompose(mat3<T>& rot, vec3<T>& scale) const;

  #ifdef MSVC6_TEMPLATES
    // Declaration for MSVC6
    friend vec3<T> operator*(const vec3<T>& v, const mat3<T>& M);
  #else
    // Declaration for gcc 3.3
    template<class T2> 
    friend vec3<T2> operator*(const vec3<T2>& v, const mat3<T2>& M);
  #endif
  //friend vec3<T> operator*<>(const vec3<T>& v, const mat3<T>& M);

  ///////////////////// Private stuff ///////////////////////
  private:
  // Matrix rows
  vec3<T> r1;
  vec3<T> r2;
  vec3<T> r3;

  void _getRotation(int i, bool neg, bool alt, bool rev, T& r1, T& r2, T& r3) const;
  void _eulerIndices(int i, bool neg, bool alt, int &j, int &k, int&h) const;
  void _eulerGivens(T a, T b, T& c, T& s, T& v) const;

  // Constructor with three vectors: Init the rows with those vectors.
  // This constructor is private because its semantics depends
  // on the actual implementation (that is, the three vectors are
  // considered to be rows). It's just used by the class itself.
  mat3(const vec3<T>& a, const vec3<T>& b, const vec3<T>& c)
    : r1(a), r2(b), r3(c) {}
};

// Related functions
template<class T> mat3<T> operator*(const T s, const mat3<T>& M);
template<class T> vec3<T> operator*(const vec3<T>& v, const mat3<T>& M);
template<class T> std::ostream& operator<<(std::ostream &s, const mat3<T>& M);


/////////////////////// Method definitions ///////////////////////////


/*----------------------operator* (scalar on the left)------------------*//**
  matrix=scalar*matrix.

  @return  Product of a scalar and a matrix.
*//*-----------------------------------------------------------------------*/
template<class T> 
inline mat3<T> operator*(const T s, const mat3<T>& M)
{
  return M*s;
}

/*----------------------operator* (vector on the left)------------------*//**
  vector=vector*matrix.

  @return  Product of a vector and a matrix.
*//*-----------------------------------------------------------------------*/
template<class T> 
inline vec3<T> operator*(const vec3<T>& v, const mat3<T>& M)
{
  return vec3<T>( 
                 v.x*M.r1.x + v.y*M.r2.x + v.z*M.r3.x,                
                 v.x*M.r1.y + v.y*M.r2.y + v.z*M.r3.y,                
                 v.x*M.r1.z + v.y*M.r2.z + v.z*M.r3.z
                );
}

/// Output operator for matrices.
template<class T> 
std::ostream& operator<<(std::ostream &s, const mat3<T>& M)
{
  s<<M.getRow(0)<<std::endl;
  s<<M.getRow(1)<<std::endl;
  s<<M.getRow(2)<<std::endl;
  return s;
}

///////////////////////////////////////////////////////////////////////

/**
   Constructor with no initial values: Then set everything to 0.

  \verbatim

       [ 0  0  0 ]
   M = [ 0  0  0 ]
       [ 0  0  0 ] 
  \endverbatim

 */
template<class T>
mat3<T>::mat3() 
  : r1(0,0,0), r2(0,0,0), r3(0,0,0) 
{
}

/**
  Constructor with one value: Fill the diagonal.

  \verbatim

       [ v  0  0 ]
   M = [ 0  v  0 ]
       [ 0  0  v ] 
  \endverbatim
*/
template<class T>
mat3<T>::mat3(T v) 
  : r1(v,0,0), r2(0,v,0), r3(0,0,v) 
{
}

/**
  Constructor with three vectors: Set the columns.

  \verbatim

       [ c1.x  c2.x  c3.x ]
   M = [ c1.y  c2.y  c3.y ]
       [ c1.z  c2.z  c3.z ] 
  \endverbatim
*/
/*template<class T>
mat3<T>::mat3(const vec3<T>& c1, const vec3<T>& c2, const vec3<T>& c3)
: r1(c1.x, c2.x, c3.x),
  r2(c1.y, c2.y, c3.y),
  r3(c1.z, c2.z, c3.z)
{
}*/

/** 
  Constructor with 9 values: 
  Each value is one component (each triple is one row).

  \verbatim

       [ a  b  c ]
   M = [ d  e  f ]
       [ g  h  i ] 
  \endverbatim
*/
template<class T>
mat3<T>::mat3(T a, T b, T c,  T d, T e, T f,  T g, T h, T i)
 : r1(a,b,c), r2(d,e,f), r3(g,h,i) 
{
}

/** 
  Constructor with 1 matrix: 
  Initialize with matrix A.

  \verbatim

   M = A
  \endverbatim
*/
template<class T>
mat3<T>::mat3(const mat3<T>& A)
  : r1(A.r1), r2(A.r2), r3(A.r3)
{
}

/**
  Return element [i,j].

  The indices range from 0 to 2. Other values will cause an EIndexError
  exception to be thrown.

  @param i Row
  @param j Column
  @return Reference to element [i,j]
  @exception EIndexError
 */
template<class T>
inline T& mat3<T>::at(short i, short j)
{
  switch(i)
  {
   case 0: return r1[j];
   case 1: return r2[j];
   case 2: return r3[j];
   default: throw EIndexError();
  }  
}

/**
  Return element [i,j].

  The indices range from 0 to 2. Other values will cause an EIndexError
  exception to be thrown.

  @param i Row
  @param j Column
  @return Reference to element [i,j]
  @exception EIndexError
 */
template<class T>
inline const T& mat3<T>::at(short i, short j) const
{
  switch(i)
  {
   case 0: return r1[j];
   case 1: return r2[j];
   case 2: return r3[j];
   default: throw EIndexError();
  }  
}


/*----------------------------setIdentity------------------------------*//**
  Set identity matrix. 

  \verbatim

       [ 1  0  0 ]
   M = [ 0  1  0 ]
       [ 0  0  1 ] 
  \endverbatim

  @return Reference to this.
*//*------------------------------------------------------------------------*/
template<class T> 
inline mat3<T>& mat3<T>::setIdentity()
{
  r1.set(1,0,0);
  r2.set(0,1,0);
  r3.set(0,0,1);
  return *this;
}

/*-------------------------------setNull---------------------------------*/
/**
  Set all components to 0. 

  \verbatim

       [ 0  0  0 ]
   M = [ 0  0  0 ]
       [ 0  0  0 ] 
  \endverbatim

  @return Reference to this.
*/
/*-------------------------------------------------------------------------*/
template<class T> 
inline mat3<T>& mat3<T>::setNull()
{
  r1.set(0,0,0);
  r2.set(0,0,0);
  r3.set(0,0,0);
  return *this;
}

/*-------------------------------setRow----------------------------------*/
/**
  Set row i to r. 

  \verbatim
   Example for i=1:

       [ rx ry rz]
   M = [ *  *  * ]
       [ *  *  * ] 
  \endverbatim

  @param     i  Row index (0, 1, 2)
  @param     r  Row.
  @return    Reference to this.
  @exception EIndexError Index is out of range.
  @see       getRow
*/
/*-------------------------------------------------------------------------*/
template<class T> 
mat3<T>& mat3<T>::setRow(short i, const vec3<T>& r)
{
  switch(i)
  {
   case 0: r1=r; break;
   case 1: r2=r; break;
   case 2: r3=r; break;
   default: throw EIndexError();
  }
  return *this;
}

/*-------------------------------setRow----------------------------------*/
/**
  Set row i to [a, b, c]. 

  \verbatim
   Example for i=1:

       [ a  b  c ]
   M = [ *  *  * ]
       [ *  *  * ] 
  \endverbatim

  @param     i  Row index (0, 1, 2)
  @param     a  First row element.
  @param     b  Second row element.
  @param     c  Third row element.
  @return    Reference to this.
  @exception EIndexError Index is out of range.
  @see       getRow
*/
/*-------------------------------------------------------------------------*/
template<class T> 
mat3<T>& mat3<T>::setRow(short i, const T a, const T b, const T c)
{
  switch(i)
  {
   case 0: r1.set(a,b,c); break;
   case 1: r2.set(a,b,c); break;
   case 2: r3.set(a,b,c); break;
   default: throw EIndexError();
  }
  return *this;
}

/*-------------------------------setColumn------------------------------*//**
  Set column i to c. 

  \verbatim
   Example for i=1:

       [ cx *  * ]
   M = [ cy *  * ]
       [ cz *  * ] 
  \endverbatim

  @param     i  Column index (0, 1, 2)
  @param     c  Column.
  @return    Reference to this.
  @exception EIndexError Index is out of range.
  @see       getColumn
*//*------------------------------------------------------------------------*/
template<class T> 
mat3<T>& mat3<T>::setColumn(short i, const vec3<T>& c)
{
  switch(i)
  {
   case 0: r1.x=c.x; r2.x=c.y; r3.x=c.z; break;
   case 1: r1.y=c.x; r2.y=c.y; r3.y=c.z; break;
   case 2: r1.z=c.x; r2.z=c.y; r3.z=c.z; break;
   default: throw EIndexError();
  }
  return *this;
}

/*-------------------------------setColumn------------------------------*//**
  Set column i to [a, b, c]. 

  \verbatim
   Example for i=1:

       [ a  *  * ]
   M = [ b  *  * ]
       [ c  *  * ] 
  \endverbatim

  @param     i  Column index (0, 1, 2)
  @param     a  First column element.
  @param     b  Second column element.
  @param     c  Third column element.
  @return    Reference to this.
  @exception EIndexError Index is out of range.
  @see       getColumn
*//*-----------------------------------------------------------------------*/
template<class T> 
mat3<T>& mat3<T>::setColumn(short i, const T a, const T b, const T c)
{
  switch(i)
  {
   case 0: r1.x=a; r2.x=b; r3.x=c; break;
   case 1: r1.y=a; r2.y=b; r3.y=c; break;
   case 2: r1.z=a; r2.z=b; r3.z=c; break;
   default: throw EIndexError();
  }
  return *this;
}

/*-------------------------------setDiag----------------------------------*/
/**
  Set diagonal to d. 

  \verbatim

       [ dx *  * ]
   M = [ *  dy * ]
       [ *  *  dz] 
  \endverbatim

  @param     d  New diagonal.
  @return    Reference to this.
  @see       getDiag
*/
/*-------------------------------------------------------------------------*/
template<class T> 
inline mat3<T>& mat3<T>::setDiag(const vec3<T>& d)
{
  r1.x=d.x;
  r2.y=d.y;
  r3.z=d.z;
  return *this;
}

/*-------------------------------setDiag--------------------------------*//**
  Set diagonal to [a, b, c].

  \verbatim

       [ a  *  * ]
   M = [ *  b  * ]
       [ *  *  c ] 
  \endverbatim

  @param     a  First diagonal element.
  @param     b  Second diagonal element.
  @param     c  Third diagonal element.
  @return    Reference to this.
  @see       getDiag
*//*-----------------------------------------------------------------------*/
template<class T>
mat3<T>& mat3<T>::setDiag(T a, T b, T c)
{
  r1.x=a;
  r2.y=b;
  r3.z=c;
  return *this;
}

/*-------------------------------getRow--------------------------------*//**
  Return row with index i. 

  @param     i  Row index (i=0,1,2).
  @return    Row i.
  @exception EIndexError Index is out of range.
  @see       setRow
*//*------------------------------------------------------------------------*/
template<class T> 
vec3<T> mat3<T>::getRow(short i) const
{
  switch(i)
  {
   case 0: return r1;
   case 1: return r2;
   case 2: return r3;
   default: throw EIndexError();
  }
}

/*-------------------------------getRow--------------------------------*//**
  Return row with index i. 

  @param     i  Row index (i=0,1,2).
  @param     dest  Receives result.
  @exception EIndexError Index is out of range.
  @see       setRow
*//*------------------------------------------------------------------------*/
template<class T> 
void mat3<T>::getRow(const short i, vec3<T>& dest) const
{
  switch(i)
  {
   case 0: dest = r1; break;
   case 1: dest = r2; break;
   case 2: dest = r3; break;
   default: throw EIndexError();
  }
}

/*-------------------------------getRow--------------------------------*//**
  Return row with index i. 

  @param     i  Row index (i=0,1,2)
  @param     a  Element [i,0] (output).
  @param     b  Element [i,1] (output).
  @param     c  Element [i,2] (output).
  @exception EIndexError Index is out of range.
  @see       setRow
*//*------------------------------------------------------------------------*/
template<class T>
void mat3<T>::getRow(short i, T& a, T& b, T& c) const
{
  switch(i)
  {
   case 0: r1.get(a,b,c); break;
   case 1: r2.get(a,b,c); break;
   case 2: r3.get(a,b,c); break;
   default: throw EIndexError();
  }
}

/*-------------------------------getColumn------------------------------*//**
  Returns column with index i. 

  @param     i  Column index (i=0,1,2).
  @return    Column i.
  @exception EIndexError Index is out of range.
  @see       setColumn
*//*------------------------------------------------------------------------*/
template<class T> 
vec3<T> mat3<T>::getColumn(short i) const
{
  switch(i)
  {
   case 0: return vec3<T>(r1.x, r2.x, r3.x);
   case 1: return vec3<T>(r1.y, r2.y, r3.y);
   case 2: return vec3<T>(r1.z, r2.z, r3.z);
   default: throw EIndexError();
  }
}

/*-------------------------------getColumn-----------------------------*//**
  Return column with index i. 

  @param     i  Column index (i=0,1,2).
  @param     dest  Receives result.
  @exception EIndexError Index is out of range.
  @see       setColumn
*//*------------------------------------------------------------------------*/
template<class T> 
void mat3<T>::getColumn(short i, vec3<T>& dest) const
{
  switch(i)
  {
   case 0: dest.set(r1.x, r2.x, r3.x); break;
   case 1: dest.set(r1.y, r2.y, r3.y); break;
   case 2: dest.set(r1.z, r2.z, r3.z); break;
   default: throw EIndexError();
  }
}

/*-------------------------------getColumn------------------------------*//**
  Return column with index i. 

  @param     i  Column index (i=0,1,2).
  @param     a  Element [0,i] (output).
  @param     b  Element [1,i] (output).
  @param     c  Element [2,i] (output).
  @exception EIndexError Index is out of range.
  @see       setColumn
*//*------------------------------------------------------------------------*/
template<class T>
void mat3<T>::getColumn(short i, T& a, T& b, T& c) const
{
  switch(i)
  {
   case 0: a=r1.x; b=r2.x; c=r3.x; break;
   case 1: a=r1.y; b=r2.y; c=r3.y; break;
   case 2: a=r1.z; b=r2.z; c=r3.z; break;
   default: throw EIndexError();
  }
}

/*-------------------------------getDiag--------------------------------*//**
  Return diagonal. 

  @return    Diagonal.
  @see       setDiag
*//*------------------------------------------------------------------------*/
template<class T> 
inline vec3<T> mat3<T>::getDiag() const
{
  return vec3<T>(r1.x, r2.y, r3.z);
}

/*-------------------------------getDiag--------------------------------*//**
  Return diagonal. 

  @param     dest  Receives result.
  @return    Diagonal.
  @see       setDiag
*//*------------------------------------------------------------------------*/
template<class T> 
inline void mat3<T>::getDiag(vec3<T>& dest) const
{
  dest.set(r1.x, r2.y, r3.z);
}

/*-------------------------------getDiag--------------------------------*//**
  Return diagonal. 

  @param     a  Element [0,0] (output).
  @param     b  Element [1,1] (output).
  @param     c  Element [2,2] (output).
  @return    Diagonal.
  @see       setDiag
*//*------------------------------------------------------------------------*/
template<class T> 
inline void mat3<T>::getDiag(T& a, T& b, T& c) const
{
  a = r1.x;
  b = r2.y;
  c = r3.z;
}

/*---------------------------setRotation-------------------------------*//**
  Set rotation matrix. 

  Sets a matrix which can be used to rotate a point around an axis
  that goes through the origin and has the direction indicated by the
  vector \a axis. The \a angle is given in radians. The rotation is
  actually performed by multiplying the matrix with a vector (the
  vector must be on the right!). 
  Example: Point \c x is rotated with \c R*x where \c R is the rotation
  matrix.
  You can determine the direction of the positive rotation with the same
  hand that you use for the coordinate system. For example, if you use
  a left-handed coordinate system point the thumb of your left hand
  in the direction of the axis, then your other fingers show the
  direction of the rotation.

  Example code:
  \code
  mat3<float> R;
  vec3<float> axis(0,0,1);
  vec3<float> point(1,0,0);

  R.setRotation(M_PI/4.0, axis);    // 45 degrees around z axis
  point=R*point;                    // Rotate point
                                    // The result is <0.707107,0.707107,0>
  \endcode

  @param     angle  Angle in radians.
  @param     axis   Rotate around this vector.
  @return    Reference to this.
  @exception EZeroDivisionError  Is thrown when length of axis is 0.
*//*-----------------------------------------------------------------------*/
template<class T> 
mat3<T>& mat3<T>::setRotation(T angle, const vec3<T>& axis)
{
  T sqr_a = axis.x*axis.x;
  T sqr_b = axis.y*axis.y;
  T sqr_c = axis.z*axis.z;
  T len2  = sqr_a+sqr_b+sqr_c;
  if (len2<=vec3<T>::epsilon) throw EZeroDivisionError("mat3.setRotation(): divide by zero");

  T k2    = cos(angle);
  T k1    = (1-k2)/len2;
  T k3    = sin(angle)/sqrt(len2);
  T k1ab  = k1*axis.x*axis.y;
  T k1ac  = k1*axis.x*axis.z;
  T k1bc  = k1*axis.y*axis.z;
  T k3a   = k3*axis.x;
  T k3b   = k3*axis.y;
  T k3c   = k3*axis.z;

  r1.x    = k1*sqr_a+k2;
  r1.y    = k1ab-k3c;
  r1.z    = k1ac+k3b;
  r2.x    = k1ab+k3c;
  r2.y    = k1*sqr_b+k2;
  r2.z    = k1bc-k3a;
  r3.x    = k1ac-k3b;
  r3.y    = k1bc+k3a;
  r3.z    = k1*sqr_c+k2;
 
  return *this;
}

/*-------------------------------setScaling-----------------------------*//**
  Set a scaling transformation.

  This method is equivalent to setNull().setDiag(s).

  \param s Scaling vector.
  \return  Reference to this.
*//*-----------------------------------------------------------------------*/
template<class T> 
mat3<T>& mat3<T>::setScaling(const vec3<T>& s)
{
  r1.set(s.x,   0,   0);
  r2.set(  0, s.y,   0);
  r3.set(  0,   0, s.z);  
  return *this;
}


/*-------------------------------operator+=-----------------------------*//**
  matrix+=matrix.

  @return  Reference to this.
*//*-----------------------------------------------------------------------*/
template<class T> 
inline mat3<T>& mat3<T>::operator+=(const mat3<T>& A)
{ 
  r1 += A.r1; 
  r2 += A.r2; 
  r3 += A.r3; 
  return *this; 
}

/*-------------------------------operator-=-----------------------------*//**
  matrix-=matrix.

  @return  Reference to this.
*//*-----------------------------------------------------------------------*/
template<class T> 
inline mat3<T>& mat3<T>::operator-=(const mat3<T>& A)
{ 
  r1 -= A.r1; 
  r2 -= A.r2; 
  r3 -= A.r3;
  return *this; 
}

/*----------------------------operator*= (scalar)-----------------------*//**
  matrix*=scalar.

  @return  Reference to this.
*//*-----------------------------------------------------------------------*/
template<class T> 
inline mat3<T>& mat3<T>::operator*=(const T s)
{ 
  r1 *= s; 
  r2 *= s; 
  r3 *= s; 
  return *this; 
}

/*----------------------------operator/= (scalar)-----------------------*//**
  matrix/=scalar.

  @return     Reference to this.
  @exception  EZeroDivisionError  Is thrown when s=0.
*//*-----------------------------------------------------------------------*/
template<class T> 
inline mat3<T>& mat3<T>::operator/=(const T s)
{ 
  if (xabs(s) <= vec3<T>::epsilon) throw EZeroDivisionError("mat3: divide by zero");

  r1.x /= s;
  r1.y /= s;
  r1.z /= s;
  r2.x /= s;
  r2.y /= s;
  r2.z /= s;
  r3.x /= s;
  r3.y /= s;
  r3.z /= s;
  return *this; 
}

/*-----------------------------operator%=-------------------------------*//**
  matrix %= scalar.

  @return    Reference to this.
  @exception EZeroDivisionError  Is thrown when r=0.
*//*------------------------------------------------------------------------*/
template<class T>
inline mat3<T>& mat3<T>::operator%=(const T r)
{
  r1 %= r;
  r2 %= r;
  r3 %= r;
  return *this;
}

/*-----------------------------operator%=-------------------------------*//**
  matrix %= matrix

  @return    Reference to this.
  @exception EZeroDivisionError  Is thrown when a component of b=0.
*//*------------------------------------------------------------------------*/
template<class T>
inline mat3<T>& mat3<T>::operator%=(const mat3<T>& b)
{
  vec3<T> s1, s2, s3;
  s1 = r1 % b.r1;
  s2 = r2 % b.r2;
  s3 = r3 % b.r3;
  r1 = s1;
  r2 = s2;
  r3 = s3;
  return *this;
}


/*--------------------------------operator+----------------------------*//**
  matrix=matrix+matrix.

  @return  Sum of two matrices.
*//*-----------------------------------------------------------------------*/
template<class T> 
inline mat3<T> mat3<T>::operator+(const mat3<T>& A) const
{
  return mat3<T>(r1+A.r1, r2+A.r2, r3+A.r3);
}

/*--------------------------------operator- ----------------------------*//**
  matrix=matrix-matrix.

  @return  Difference of two matrices.
*//*-----------------------------------------------------------------------*/
template<class T> 
inline mat3<T> mat3<T>::operator-(const mat3<T>& A) const
{
  return mat3<T>(r1-A.r1, r2-A.r2, r3-A.r3);
}

/*----------------------------operator- (unary)-------------------------*//**
  matrix=-matrix.

  @return  Negated matrix.
*//*-----------------------------------------------------------------------*/
template<class T> 
inline mat3<T> mat3<T>::operator-() const
{
  return mat3<T>(-r1, -r2, -r3);
}


/*--------------------------------operator*----------------------------*//**
  matrix=matrix*matrix.

  @return  Product of two matrices.
*//*-----------------------------------------------------------------------*/
template<class T> 
inline mat3<T> mat3<T>::operator*(const mat3<T>& A) const
{
  return mat3<T>( 
                  r1.x*A.r1.x + r1.y*A.r2.x + r1.z*A.r3.x,
                  r1.x*A.r1.y + r1.y*A.r2.y + r1.z*A.r3.y,
                  r1.x*A.r1.z + r1.y*A.r2.z + r1.z*A.r3.z,

                  r2.x*A.r1.x + r2.y*A.r2.x + r2.z*A.r3.x,
                  r2.x*A.r1.y + r2.y*A.r2.y + r2.z*A.r3.y,
                  r2.x*A.r1.z + r2.y*A.r2.z + r2.z*A.r3.z,

                  r3.x*A.r1.x + r3.y*A.r2.x + r3.z*A.r3.x,
                  r3.x*A.r1.y + r3.y*A.r2.y + r3.z*A.r3.y,
                  r3.x*A.r1.z + r3.y*A.r2.z + r3.z*A.r3.z
                );
}

/*--------------------------------operator*=----------------------------*//**
  matrix*=matrix.

  A may also be this.

  @return  Reference to this.
*//*-----------------------------------------------------------------------*/
template<class T> 
inline mat3<T>& mat3<T>::operator*=(const mat3<T>& A)
{
  double _m11 = r1.x*A.r1.x + r1.y*A.r2.x + r1.z*A.r3.x;
  double _m12 = r1.x*A.r1.y + r1.y*A.r2.y + r1.z*A.r3.y;
  double _m13 = r1.x*A.r1.z + r1.y*A.r2.z + r1.z*A.r3.z;
  double _m21 = r2.x*A.r1.x + r2.y*A.r2.x + r2.z*A.r3.x;
  double _m22 = r2.x*A.r1.y + r2.y*A.r2.y + r2.z*A.r3.y;
  double _m23 = r2.x*A.r1.z + r2.y*A.r2.z + r2.z*A.r3.z;
  double _m31 = r3.x*A.r1.x + r3.y*A.r2.x + r3.z*A.r3.x;
  double _m32 = r3.x*A.r1.y + r3.y*A.r2.y + r3.z*A.r3.y;
  double _m33 = r3.x*A.r1.z + r3.y*A.r2.z + r3.z*A.r3.z;

  r1.set(_m11, _m12, _m13);
  r2.set(_m21, _m22, _m23);
  r3.set(_m31, _m32, _m33);

  return *this;
}

/*--------------------------------operator*-----------------------------*//**
  vector=matrix*vector.

  @return  Product of a matrix and a vector.
*//*-----------------------------------------------------------------------*/
template<class T> 
inline vec3<T> mat3<T>::operator*(const vec3<T>& v) const
{
  return vec3<T>( 
                 r1.x*v.x + r1.y*v.y + r1.z*v.z,
                 r2.x*v.x + r2.y*v.y + r2.z*v.z,
                 r3.x*v.x + r3.y*v.y + r3.z*v.z
                );
}

/*--------------------------------operator*-----------------------------*//**
  matrix=matrix*scalar.

  @return  Product of a matrix and a scalar.
*//*-----------------------------------------------------------------------*/
template<class T> 
inline mat3<T> mat3<T>::operator*(const T s) const
{ 
  return mat3<T>(s*r1,s*r2,s*r3); 
}

/*--------------------------------operator/-----------------------------*//**
  matrix=matrix/scalar.

  @return     Matrix diveded by scalar (component wise).
  @exception  EZeroDivisionError  Is returned when s=0.
*//*-----------------------------------------------------------------------*/
template<class T> 
inline mat3<T> mat3<T>::operator/(const T s) const
{ 
  if (xabs(s) <= vec3<T>::epsilon) throw EZeroDivisionError("mat3: divide by zero");

  return mat3<T>( r1.x/s, r1.y/s, r1.z/s,
                  r2.x/s, r2.y/s, r2.z/s,
                  r3.x/s, r3.y/s, r3.z/s);
}

/*-----------------------------operator% -------------------------------*//**
  mat = mat % scalar.

  @return    Matrix modulo scalar (component wise).
  @exception EZeroDivisionError  Is returned when r=0.
*//*------------------------------------------------------------------------*/
template<class T>
inline mat3<T> mat3<T>::operator%(const T r) const
{
  return mat3<T>(r1%r, r2%r, r3%r);
}

/*-----------------------------operator% -------------------------------*//**
  mat = mat % mat

  @return    Matrix modulo matrix (component wise).
  @exception EZeroDivisionError  Is returned when component of b=0.
*//*------------------------------------------------------------------------*/
template<class T>
inline mat3<T> mat3<T>::operator%(const mat3<T>& b) const
{
  return mat3<T>(r1%b.r1, r2%b.r2, r3%b.r3);
}


/*------------------------------operator==------------------------------*//**
  matrix==matrix

  @return     True, if matrices are equal.
*//*-----------------------------------------------------------------------*/
template<class T>
inline bool mat3<T>::operator==(const mat3<T>& A) const
{
  return (r1==A.r1) && (r2==A.r2) && (r3==A.r3);
}

/*------------------------------operator!=------------------------------*//**
  matrix!=matrix

  @return     True, if matrices are not equal.
*//*-----------------------------------------------------------------------*/
template<class T>
inline bool mat3<T>::operator!=(const mat3<T>& A) const
{
  return !(*this==A);
}

/*----------------------------------------------------------------------*//**
  Copy the matrix values into an array.

  \pre The size of dest is at least 9
  \param dest The target array (this must be big enough to hold 9 values)
  \param rowmajor If true the values are laid out in row major order
*//*-----------------------------------------------------------------------*/
template<class T> 
void mat3<T>::toList(T* dest, bool rowmajor) const
{
  if (rowmajor)
  {
    dest[0] = r1.x;
    dest[1] = r1.y;
    dest[2] = r1.z;
    dest[3] = r2.x;
    dest[4] = r2.y;
    dest[5] = r2.z;
    dest[6] = r3.x;
    dest[7] = r3.y;
    dest[8] = r3.z;
  }
  else
  {
    dest[0] = r1.x;
    dest[1] = r2.x;
    dest[2] = r3.x;
    dest[3] = r1.y;
    dest[4] = r2.y;
    dest[5] = r3.y;
    dest[6] = r1.z;
    dest[7] = r2.z;
    dest[8] = r3.z;
  }
}


/*-----------------------------determinant-------------------------------*//**
  Calculate the determinant.

  @return  Determinant.
*//*-----------------------------------------------------------------------*/
template<class T> 
inline T mat3<T>::determinant() const
{
  return r1.x*r2.y*r3.z +
         r1.y*r2.z*r3.x +
         r1.z*r2.x*r3.y -
         r3.x*r2.y*r1.z -
         r3.y*r2.z*r1.x -
         r3.z*r2.x*r1.y;
}


/*--------------------------------inverse--------------------------------*//**
  Invert matrix. 

  Throws an ESingularMatrix exception if the matrix can't be inverted
  (that is, the determinant is zero). The inverted matrix is stored in
  dest, the matrix *this remains unchanged. It is allowed to pass
  *this as destination matrix.

  @return     dest.
  @exception EZeroDivisionError The matrix isn't invertible (det=0).  
*//*-----------------------------------------------------------------------*/
template<class T> 
inline mat3<T>& mat3<T>::inverse(mat3<T>& dest) const
{
  T d   = determinant();
  if (xabs(d)<=vec3<T>::epsilon) throw EZeroDivisionError("mat3.inverse(): divide by zero");
  d=1/d;
  T d11 = d*(r2.y*r3.z - r2.z*r3.y);
  T d12 = d*(r2.z*r3.x - r2.x*r3.z);   // * (-1)
  T d13 = d*(r2.x*r3.y - r2.y*r3.x);
  T d21 = d*(r1.z*r3.y - r1.y*r3.z);   // * (-1)
  T d22 = d*(r1.x*r3.z - r1.z*r3.x);
  T d23 = d*(r1.y*r3.x - r1.x*r3.y);   // * (-1)
  T d31 = d*(r1.y*r2.z - r1.z*r2.y);
  T d32 = d*(r1.z*r2.x - r1.x*r2.z);   // * (-1)
  T d33 = d*(r1.x*r2.y - r1.y*r2.x);

  // Transpose...
  dest.r1.x = d11;
  dest.r1.y = d21;
  dest.r1.z = d31;
  dest.r2.x = d12;
  dest.r2.y = d22;
  dest.r2.z = d32;
  dest.r3.x = d13;
  dest.r3.y = d23;
  dest.r3.z = d33;

  return dest;
}

/*--------------------------------inverse--------------------------------*//**
  Invert matrix. 

  See invert.

  @return     Inverted matrix.
  @exception  ESingularMatrix  The matrix isn't invertible (det=0).
  @see        invert
*//*-----------------------------------------------------------------------*/
template<class T> 
inline mat3<T> mat3<T>::inverse() const
{
  mat3<T> res;
  return inverse(res);
}


/*--------------------------------transpose-----------------------------*//**
  Transpose matrix. 

  The transposed matrix is stored in dest.
  matrix *this remains unchanged. It is allowed to pass *this
  as destination matrix.

  @return     dest.
*//*-----------------------------------------------------------------------*/
template<class T> 
inline mat3<T>& mat3<T>::transpose(mat3<T>& dest) const
{
  if (&dest==this)
  {
    T dummy    = r1.y;
    dest.r1.y  = r2.x;
    dest.r2.x  = dummy;
    dummy      = r1.z;
    dest.r1.z  = r3.x;
    dest.r3.x  = dummy;
    dummy      = r2.z;
    dest.r2.z  = r3.y;
    dest.r3.y  = dummy;
  }
  else
  {
    dest.r1.x = r1.x;
    dest.r1.y = r2.x;
    dest.r1.z = r3.x;
    dest.r2.x = r1.y;
    dest.r2.y = r2.y;
    dest.r2.z = r3.y;
    dest.r3.x = r1.z;
    dest.r3.y = r2.z;
    dest.r3.z = r3.z;
  }

  return dest;
}

/*--------------------------------transpose-----------------------------*//**
  Transpose matrix. 

  See transpose.

  @return     Transposed matrix.
  @see        transpose
*//*-----------------------------------------------------------------------*/
template<class T> 
inline mat3<T> mat3<T>::transpose() const
{
  return mat3<T>(r1.x,r2.x,r3.x, r1.y,r2.y,r3.y, r1.z,r2.z,r3.z);
}

/*----------------------------------scale-------------------------------*//**
  Concatenate a scaling transformation.

  This method is equivalent to this*S where S is the scaling matrix.

  \return Reference to this.
*//*-----------------------------------------------------------------------*/
template<class T> 
inline mat3<T>& mat3<T>::scale(const vec3<T>& s)
{
  T sx=s.x, sy=s.y, sz=s.z;

  r1.x *= sx;
  r1.y *= sy;
  r1.z *= sz;  
  r2.x *= sx;
  r2.y *= sy;
  r2.z *= sz;  
  r3.x *= sx;
  r3.y *= sy;
  r3.z *= sz;
  return *this;
}

/*----------------------------------rotate------------------------------*//**
  Concatenate a rotation transformation.

  \param  axis   Rotate around this vector
  \param  angle  Angle in radians
  \return Reference to this.
*//*-----------------------------------------------------------------------*/
template<class T> 
mat3<T>& mat3<T>::rotate(T angle, const vec3<T>& axis)
{
  mat3<T> R;
  R.setRotation(angle, axis);
  (*this)*=R;
  return *this;
}

/*----------------------------------ortho------------------------------*//**
  Make base vectors orthogonal.

  \param  dest Destination matrix that receives the result (may be this).
  \return Reference to dest
*//*-----------------------------------------------------------------------*/
template<class T> 
mat3<T>& mat3<T>::ortho(mat3<T>& dest) const
{
  vec3<T> x(r1.x, r2.x, r3.x);
  vec3<T> y(r1.y, r2.y, r3.y);
  vec3<T> z(r1.z, r2.z, r3.z);
  T xl = x.length();
  if (xl<=vec3<T>::epsilon)
    throw EZeroDivisionError("mat3.ortho(): divide by zero");

  xl *= xl;
  y -= ((x*y)/xl)*x;
  z -= ((x*z)/xl)*x;

  T yl = y.length();
  if (yl<=vec3<T>::epsilon)
    throw EZeroDivisionError("mat3.ortho(): divide by zero");

  yl *= yl;
  z -= ((y*z)/yl)*y;

  dest.r1.set(x.x, y.x, z.x);
  dest.r2.set(x.y, y.y, z.y);
  dest.r3.set(x.z, y.z, z.z);
  return dest;
}

/*----------------------------------ortho------------------------------*//**
  Make base vectors orthogonal.

  this remains unchanged.

  \return Matrix with orthogonal vector.
*//*-----------------------------------------------------------------------*/
template<class T> 
mat3<T> mat3<T>::ortho() const
{
  mat3<T> res;
  return ortho(res);
}

/*---------------------------------decompose-----------------------------*//**
  Decompose the matrix intro a rotation and scaling part.

  If there was a scaling part that was zero then 
  a EZeroDivisionError exception is thrown.

  \param rot  Receives the rotation part
  \param scale Receives the scaling part
*//*-----------------------------------------------------------------------*/
template<class T> 
void mat3<T>::decompose(mat3<T>& rot, vec3<T>& scale) const
{
  vec3<T> a,b,c;
  T al, bl, cl;

  ortho(rot);
  rot.getColumn(0, a);
  rot.getColumn(1, b);
  rot.getColumn(2, c);
  // al,bl,cl will be the scaling factors

  al = a.length();
  bl = b.length();
  cl = c.length();
  if ( (al<=vec3<T>::epsilon) || (bl<=vec3<T>::epsilon) || (cl<=vec3<T>::epsilon))
    throw EZeroDivisionError("mat3.decompose(): divide by zero");

  scale.set(al, bl, cl);

  // Normalize a,b,c
  a /= al;
  b /= bl;
  c /= cl;

  rot.setColumn(0, a);
  rot.setColumn(1, b);
  rot.setColumn(2, c);
  if (rot.determinant()<0)
  {
    rot.setColumn(0, -a);
    scale.x = -scale.x;
  }
}

/**
  Set Euler angles (order: yxz).

  \param x  Angle around x (radians)
  \param y  Angle around y (radians)
  \param z  Angle around z (radians)
 */
template<class T> 
mat3<T>& mat3<T>::setRotationYXZ(T x, T y, T z)
{
  T A = cos(x);
  T B = sin(x);
  T C = cos(y);
  T D = sin(y);
  T E = cos(z);
  T F = sin(z);
  T CE = C*E;
  T CF = C*F;
  T DE = D*E;
  T DF = D*F;

  r1.set(CE+DF*B, DE*B-CF, A*D);
  r2.set(A*F, A*E, -B);
  r3.set(CF*B-DE, DF+CE*B, A*C);
  return *this;
}

/**
  Set Euler angles (order: zxy).

  \param x  Angle around x (radians)
  \param y  Angle around y (radians)
  \param z  Angle around z (radians)
 */
template<class T> 
mat3<T>& mat3<T>::setRotationZXY(T x, T y, T z)
{
  T A = cos(x);
  T B = sin(x);
  T C = cos(y);
  T D = sin(y);
  T E = cos(z);
  T F = sin(z);
  T CE = C*E;
  T CF = C*F;
  T DE = D*E;
  T DF = D*F;

  r1.set(CE-DF*B, -A*F, DE+CF*B);
  r2.set(CF+DE*B, A*E, DF-CE*B);
  r3.set(-A*D, B, A*C);
  return *this;
}

/**
  Set Euler angles (order: zyx).

  \param x  Angle around x (radians)
  \param y  Angle around y (radians)
  \param z  Angle around z (radians)
 */
template<class T> 
mat3<T>& mat3<T>::setRotationZYX(T x, T y, T z)
{
  T A = cos(x);
  T B = sin(x);
  T C = cos(y);
  T D = sin(y);
  T E = cos(z);
  T F = sin(z);

  T AE = A*E;
  T AF = A*F;
  T BE = B*E;
  T BF = B*F;
  r1.set(C*E, BE*D-AF, AE*D+BF);
  r2.set(C*F, BF*D+AE, AF*D-BE);
  r3.set(-D, B*C, A*C);
  return *this;
}

/**
  Set Euler angles (order: yzx).

  \param x  Angle around x (radians)
  \param y  Angle around y (radians)
  \param z  Angle around z (radians)
 */
template<class T> 
mat3<T>& mat3<T>::setRotationYZX(T x, T y, T z)
{
  T A = cos(x);
  T B = sin(x);
  T C = cos(y);
  T D = sin(y);
  T E = cos(z);
  T F = sin(z);
  T AC = A*C;
  T AD = A*D;
  T BC = B*C;
  T BD = B*D;

  r1.set(C*E, BD-AC*F, BC*F+AD); 
  r2.set(F, A*E, -B*E);
  r3.set(-D*E, AD*F+BC, AC-BD*F);
  return *this;
}

/**
  Set Euler angles (order: xzy).

  \param x  Angle around x (radians)
  \param y  Angle around y (radians)
  \param z  Angle around z (radians)
 */
template<class T> 
mat3<T>& mat3<T>::setRotationXZY(T x, T y, T z)
{
  T A = cos(x);
  T B = sin(x);
  T C = cos(y);
  T D = sin(y);
  T E = cos(z);
  T F = sin(z);
  T AC = A*C;
  T AD = A*D;
  T BC = B*C;
  T BD = B*D;

  r1.set(C*E, -F, D*E);
  r2.set(AC*F+BD, A*E, AD*F-BC);
  r3.set(BC*F-AD, B*E, BD*F+AC);
  return *this;
}

/**
  Set Euler angles (order: xyz).

  \param x  Angle around x (radians)
  \param y  Angle around y (radians)
  \param z  Angle around z (radians)
 */
template<class T> 
mat3<T>& mat3<T>::setRotationXYZ(T x, T y, T z)
{
  T A = cos(x);
  T B = sin(x);
  T C = cos(y);
  T D = sin(y);
  T E = cos(z);
  T F = sin(z);
  T AE = A*E;
  T AF = A*F;
  T BE = B*E;
  T BF = B*F;

  r1.set(C*E, -C*F, D);
  r2.set(AF+BE*D, AE-BF*D, -B*C);
  r3.set(BF-AE*D, BE+AF*D, A*C);
  return *this;
}

/**
  Helper function for the _getRotation() function.
 */
template<class T> 
void mat3<T>::_eulerIndices(int i, bool neg, bool alt, int& j, int& k, int& h) const
{
  int next[4] = {1, 2, 0, 1};
  j = next[i+int(neg)];
  k = 3-i-j;
  h = next[k+(1^int(neg)^int(alt))];
}

/**
  Helper function for the _getRotation() function.
 */
template<class T> 
void mat3<T>::_eulerGivens(T a, T b, T& c, T& s, T& r) const
{
  T absa = xabs(a);
  T absb = xabs(b);
  // b=0?
  if (absb<=vec3<T>::epsilon)
  {
    if (a>=0)
      c = 1.0;
    else
      c = -1.0;
    s = 0.0;
    r = absa;
  }
  // a=0?
  else if (absa<=vec3<T>::epsilon)
  {
    c = 0.0;
    if (b>=0)
      s = 1.0;
    else
      s = -1.0;
    r = absb;
  }
  // General case
  else
  {
    if (absb>absa)
    {
      T t = a/b;
      T u = sqrt(1.0+t*t);
      if (b<0)
	u = -u;
      s = 1.0/u;
      c = s*t;
      r = b*u;
    }
    else
    {
      T t = b/a;
      T u = sqrt(1.0+t*t);
      if (a<0)
	u = -u;
      c = 1.0/u;
      s = c*t;
      r = a*u;
    }
  }
}

/**
   Get Euler angles in any of the 24 different conventions.

   The first four argument select a particular convention. The last three
   output arguments receive the angles. The order of the angles depends
   on the convention.

   See http://www.cgafaq.info/wiki/Euler_angles_from_matrix for the
   algorithm used.

   \param  i  The index of the first axis (global rotations, s) or last axis (local rotations, r). 0=XZX, 1=YXY, 2=ZYZ
   \param  neg  If true, the convention contains an odd permutation of the convention defined by i alone (i.e. the middle axis is replaced. For example, XZX -> XYX)
   \param  alt  If true, the first and last axes are different. Local rotations: The first axis changes. For example, XZX -> YZX
   \param  rev  If true, the first and last angle are exchanged. This toggles between global/local rotations. In all the concrete getRotation*() functions this is always true because all the functions assume local rotations.
   \param[out] r1  Angle around first axis (radians)
   \param[out] r2  Angle around second axis (radians)
   \param[out] r3  Angle around third axis (radians)   
 */
template<class T> 
void mat3<T>::_getRotation(int i, bool neg, bool alt, bool rev, T& r1, T& r2, T& r3) const
{
  int j, k, h;
  T v[3] = {at(0,i), at(1,i), at(2,i)};

  _eulerIndices(i, neg, alt, j, k, h);

  T a = v[h];
  T b = v[k];
  T c, s;
  _eulerGivens(a, b, c, s, v[h]);
  T s1 = c*at(k,j) - s*at(h,j);
  T c1 = c*at(k,k) - s*at(h,k);
  r1 = atan2(s1, c1);
  r2 = atan2(v[j], v[i]);
  r3 = atan2(s, c);
  if (alt)
    r3 = -r3;
  if (neg)
  {
    r1 = -r1;
    r2 = -r2;
    r3 = -r3;
  }
  if (rev)
  {
    T tmp = r1;
    r1 = r3;
    r3 = tmp;
  }
}

/**
  Get Euler angles (order: yxz).

  \param[out] x  Angle around x (radians)
  \param[out] y  Angle around y (radians)
  \param[out] z  Angle around z (radians)
 */
template<class T> 
void mat3<T>::getRotationYXZ(T& x, T& y, T& z) const
{
  _getRotation(2, 1, 1, 1, y, x, z);
}

/**
  Get Euler angles (order: zxy).

  \param[out] x  Angle around x (radians)
  \param[out] y  Angle around y (radians)
  \param[out] z  Angle around z (radians)
 */
template<class T> 
void mat3<T>::getRotationZXY(T& x, T& y, T& z) const
{
  _getRotation(1, 0, 1, 1, z, x, y);
}

/**
  Get Euler angles (order: zyx).

  \param[out] x  Angle around x (radians)
  \param[out] y  Angle around y (radians)
  \param[out] z  Angle around z (radians)
 */
template<class T> 
void mat3<T>::getRotationZYX(T& x, T& y, T& z) const
{
  _getRotation(0, 1, 1, 1, z, y, x);
}

/**
  Get Euler angles (order: yzx).

  \param[out] x  Angle around x (radians)
  \param[out] y  Angle around y (radians)
  \param[out] z  Angle around z (radians)
 */
template<class T> 
void mat3<T>::getRotationYZX(T& x, T& y, T& z) const
{
  _getRotation(0, 0, 1, 1, y, z, x);
}

/**
  Get Euler angles (order: xzy).

  \param[out] x  Angle around x (radians)
  \param[out] y  Angle around y (radians)
  \param[out] z  Angle around z (radians)
 */
template<class T> 
void mat3<T>::getRotationXZY(T& x, T& y, T& z) const
{
  _getRotation(1, 1, 1, 1, x, z, y);
}

/**
  Get Euler angles (order: xyz).

  \param[out] x  Angle around x (radians)
  \param[out] y  Angle around y (radians)
  \param[out] z  Angle around z (radians)
 */
template<class T> 
void mat3<T>::getRotationXYZ(T& x, T& y, T& z) const
{
  _getRotation(2, 0, 1, 1, x, y, z);
}

/**
   Set a rotation matrix that rotates one vector into another.

   This method is based on the code from:

   Tomas Möller, John Hughes<br>
   \em Efficiently \em Building \em a \em Matrix \em to \em Rotate \em One \em Vector \em to \em Another<br>
   Journal of Graphics Tools, 4(4):1-4, 1999<br>
   http://www.acm.org/jgt/papers/MollerHughes99/<br>

   \param from  Source vector (must be a unit vector)
   \param to  Target vector (must be a unit vector)
   \return A reference to this.
 */
template<class T> 
mat3<T>& mat3<T>::fromToRotation(const vec3<T>& from, const vec3<T>& to)
{
  const T EPSILON = 0.000001;
  T e = from*to;
  T f = (e<0)? -e:e;

  if (f>1.0-EPSILON)     /* "from" and "to"-vector almost parallel */
  {
    /* vector most nearly orthogonal to "from" */
    vec3<T> x((from.x>0.0)? from.x : -from.x,
              (from.y>0.0)? from.y : -from.y,
	      (from.z>0.0)? from.z : -from.z);

    if (x.x<x.y)
    {
      if (x.x<x.z)
      {
        x.set(1.0, 0.0, 0.0);
      }
      else
      {
        x.set(0.0, 0.0, 1.0);
      }
    }
    else
    {
      if (x.y<x.z)
      {
        x.set(0.0, 1.0, 0.0);
      }
      else
      {
        x.set(0.0, 0.0, 1.0);
      }
    }

    /* temporary storage vectors */
    vec3<T> u = x-from;
    vec3<T> v = x-to;

    T c1 = 2.0/(u*u);
    T c2 = 2.0/(v*v);
    T c3 = c1*c2*u*v;

    for(int i = 0; i<3; i++) 
    {
      for(int j = 0; j<3; j++) 
      {
        at(i,j) =  - c1 * u[i] * u[j]
	           - c2 * v[i] * v[j]
                   + c3 * v[i] * u[j];
      }
      at(i,i) += 1.0;
    }
  }
  else  /* the most common case, unless "from"="to", or "from"=-"to" */
  {
    vec3<T> v = from.cross(to);
    T h = 1.0/(1.0 + e);      /* optimization by Gottfried Chen */
    T hvx = h*v.x;
    T hvz = h*v.z;
    T hvxy = hvx*v.y;
    T hvxz = hvx*v.z;
    T hvyz = hvz*v.y;

    r1.x = e + hvx*v.x;
    r1.y = hvxy - v.z;
    r1.z = hvxz + v.y;

    r2.x = hvxy + v.z;
    r2.y = e + h*v.y*v.y;
    r2.z = hvyz - v.x;

    r3.x = hvxz - v.y;
    r3.y = hvyz + v.x;
    r3.z = e + hvz*v.z;
  }
  return *this;
}


typedef mat3<double> mat3d;
typedef mat3<float> mat3f;


} // end of namespace

#endif
