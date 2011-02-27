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

#ifndef MAT4_H
#define MAT4_H

/**
   \file mat4.h

   Contains the declaration of mat4, a 4x4 matrix with 
   components of type T. 
 */

#include <iostream>
#include <algorithm>
#include "compile_switches.h"
#include <math.h>
//#include "xmath.h"
#include "vec3.h"
#include "vec4.h"
#include "mat3.h"

#include <iostream>

namespace support3d {


// Calculates m3.determinant() where m3 is a mat3 that is based on m4
// where row i and column j are left out.
template<class T>
T _subdet(T m4[16], short i, short j)
{
  T m3[9];
  T* p = m3;
  short k,l;

  // Copy the matrix into m3 leaving out row i and column j...
  for(k=0; k<4; k++)
  {
    if (k==i)
      continue;

    for(l=0; l<4; l++)
    {
      if (l==j)
	continue;
      *p = m4[k*4+l];
      p+=1;
    }
  }
 
  return m3[0]*m3[4]*m3[8]+
         m3[1]*m3[5]*m3[6]+
         m3[2]*m3[3]*m3[7]-
         m3[6]*m3[4]*m3[2]-
         m3[7]*m3[5]*m3[0]-
         m3[8]*m3[3]*m3[1];
}

/** 
   4x4 matrix with components of type T.

   mat4 is a 4x4 template matrix with components of type T. Some
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
class mat4
{
  public:
  // Constructors
  mat4();
  explicit mat4(T v);
  mat4(T a, T b, T c, T d, 
       T e, T f, T g, T h, 
       T i, T j, T k, T l, 
       T m, T n, T o, T p);
  mat4(const mat4<T>& A);

  T& at(short i, short j);

  // set_ and get_ methods
  mat4<T>& setIdentity();
  mat4<T>& setNull();
  mat4<T>& setRow(short i, const vec4<T>& r);
  mat4<T>& setRow(short i, T a, T b, T c, T d);
  mat4<T>& setColumn(short i, const vec4<T>& c);
  mat4<T>& setColumn(short i, T a, T b, T c, T d);
  mat4<T>& setDiag(const vec4<T>& d);
  mat4<T>& setDiag(T a, T b, T c, T d);
  vec4<T>  getRow(short i) const;
  void     getRow(short i, vec4<T>& dest) const;
  void     getRow(short i, T& a, T& b, T& c, T& d) const;
  vec4<T>  getColumn(short i) const;
  void     getColumn(short i, vec4<T>& dest) const;
  void     getColumn(short i, T& a, T& b, T& c, T& d) const;
  vec4<T>  getDiag() const;
  void     getDiag(vec4<T>& dest) const;
  void     getDiag(T& a, T& b, T& c, T& d) const;

  mat4<T>& setTranslation(const vec3<T>& t);
  mat4<T>& setRotation(T angle, const vec3<T>& axis);
  mat4<T>& setScaling(const vec3<T>& s);
  mat4<T>& setOrthographic(T left, T right, T bottom, T top, T nearval, T farval);
  mat4<T>& setFrustum(T left, T right, T bottom, T top, T near_, T far_);
  mat4<T>& setPerspective(T fovy, T aspect, T near_, T far_);
  mat4<T>& setLookAt(const vec3<T>& pos, const vec3<T>& target, const vec3<T>& up=vec3<T>(0,0,1));
  mat4<T>& setMat3(const mat3<T>& m3);
  mat3<T>  getMat3() const;
  mat3<T>& getMat3(mat3<T>& dest) const;

  // Operators
  mat4<T>& operator+=(const mat4<T>& A);       // matrix += matrix
  mat4<T>& operator-=(const mat4<T>& A);       // matrix -= matrix
  mat4<T>& operator*=(const mat4<T>& A);       // matrix *= matrix
  mat4<T>& operator*=(const T s);              // matrix *= scalar
  mat4<T>& operator/=(const T s);              // matrix /= scalar
  mat4<T>& operator%=(const T r);
  mat4<T>& operator%=(const mat4<T>& b);

  mat4<T> operator+(const mat4<T>& A) const;   // matrix = matrix + matrix
  mat4<T> operator-(const mat4<T>& A) const;   // matrix = matrix - matrix
  mat4<T> operator-() const;                   // matrix = -matrix

  mat4<T> operator*(const mat4<T>& A) const;   // matrix = matrix * matrix
  vec4<T> operator*(const vec4<T>& v) const;   // vector = matrix * vector
  vec3<T> operator*(const vec3<T>& v) const;   // vector = matrix * vector
  mat4<T> operator*(const T s) const;          // matrix = matrix * scalar

  mat4<T> operator/(const T s) const;          // matrix = matrix / scalar
  mat4<T> operator%(const T b) const;          // mat = mat % scalar (each component)
  mat4<T> operator%(const mat4<T>& b) const;   // mat = mat % mat

  bool operator==(const mat4<T>& A) const;     // matrix == matrix
  bool operator!=(const mat4<T>& A) const;     // matrix == matrix

  void toList(T* dest, bool rowmajor=false) const;
  void fromList(T* array, bool rowmajor);

  // determinant
  T determinant() const;

  // Inversion (*this is never changed, unless dest = *this)
  mat4<T>  inverse() const;
  mat4<T>& inverse(mat4<T>& dest) const;

  // Transposition
  mat4<T>  transpose() const;
  mat4<T>& transpose(mat4<T>& dest) const;

  mat4<T>& translate(const vec3<T>& t);
  mat4<T>& scale(const vec3<T>& s);
  mat4<T>& rotate(T angle, const vec3<T>& axis);

  mat4<T>& ortho(mat4<T>& dest) const;
  mat4<T> ortho() const;

  void decompose(vec3<T>& t, mat4<T>& rot, vec3<T>& scale) const;

  #ifdef MSVC6_TEMPLATES
    // Declaration for MSVC6
    friend vec4<T> operator*(const vec4<T>& v, const mat4<T>& M);
    friend vec3<T> operator*(const vec3<T>& v, const mat4<T>& M);
  #else
    // Declaration for gcc 3.3
    template<class T2> 
    friend vec4<T2> operator*(const vec4<T2>& v, const mat4<T2>& M);
    template<class T2> 
    friend vec3<T2> operator*(const vec3<T2>& v, const mat4<T2>& M);
  #endif
  //friend vec3<T> operator*<>(const vec3<T>& v, const mat4<T>& M);

  ///////////////////// Private stuff ///////////////////////
  private:
  // Matrix
  T m11, m12, m13, m14;
  T m21, m22, m23, m24;
  T m31, m32, m33, m34;
  T m41, m42, m43, m44;
};

// Related functions
template<class T> mat4<T> operator*(const T s, const mat4<T>& M);
template<class T> vec4<T> operator*(const vec4<T>& v, const mat4<T>& M);
template<class T> std::ostream& operator<<(std::ostream &s, const mat4<T>& M);


/////////////////////// Method definitions ///////////////////////////


/*----------------------operator* (scalar on the left)------------------*//**
  matrix=scalar*matrix.

  @return  Product of a scalar and a matrix.
*//*-----------------------------------------------------------------------*/
template<class T> 
inline mat4<T> operator*(const T s, const mat4<T>& M)
{
  return M*s;
}

/*----------------------operator* (vector on the left)------------------*//**
  vector=vector*matrix.

  @return  Product of a vector and a matrix.
*//*-----------------------------------------------------------------------*/
template<class T> 
inline vec4<T> operator*(const vec4<T>& v, const mat4<T>& M)
{
  return vec4<T>( 
                 v.x*M.m11 + v.y*M.m21 + v.z*M.m31 + v.w*M.m41,
                 v.x*M.m12 + v.y*M.m22 + v.z*M.m32 + v.w*M.m42,
                 v.x*M.m13 + v.y*M.m23 + v.z*M.m33 + v.w*M.m43,
                 v.x*M.m14 + v.y*M.m24 + v.z*M.m34 + v.w*M.m44
                );
}

/*----------------------operator* (vector on the left)------------------*//**
  vector=vec3*matrix.

  The missing component of the vec3 is implicitly set to 1.0

  @return  Product of a vector and a matrix.
*//*-----------------------------------------------------------------------*/
template<class T> 
inline vec3<T> operator*(const vec3<T>& v, const mat4<T>& M)
{
  vec3<T> res(v.x*M.m11 + v.y*M.m21 + v.z*M.m31 + M.m41,
	      v.x*M.m12 + v.y*M.m22 + v.z*M.m32 + M.m42,
	      v.x*M.m13 + v.y*M.m23 + v.z*M.m33 + M.m43);

  T w = v.x*M.m14 + v.y*M.m24 + v.z*M.m34 + M.m44;

  if (xabs(w)>vec3<T>::epsilon)
  {
    res/=w;
  }

  return res;
}

/// Output operator for matrices.
template<class T> 
std::ostream& operator<<(std::ostream &s, const mat4<T>& M)
{
  T m[16];
  M.toList(m, true);
  s<<"["<<m[0]<<", "<<m[1]<<", "<<m[2]<<", "<<m[3]<<"]\n";
  s<<"["<<m[4]<<", "<<m[5]<<", "<<m[6]<<", "<<m[7]<<"]\n";
  s<<"["<<m[8]<<", "<<m[9]<<", "<<m[10]<<", "<<m[11]<<"]\n";
  s<<"["<<m[12]<<", "<<m[13]<<", "<<m[14]<<", "<<m[15]<<"]";
  return s;
}

///////////////////////////////////////////////////////////////////////

/**
   Constructor with no initial values: Then set everything to 0.

  \verbatim

       [ 0  0  0  0]
   M = [ 0  0  0  0]
       [ 0  0  0  0] 
       [ 0  0  0  0] 
  \endverbatim

 */
template<class T>
mat4<T>::mat4() 
  : m11(0), m12(0), m13(0), m14(0),
    m21(0), m22(0), m23(0), m24(0),
    m31(0), m32(0), m33(0), m34(0),
    m41(0), m42(0), m43(0), m44(0)
{
}

/**
  Constructor with one value: Fill the diagonal.

  \verbatim

       [ v  0  0  0]
   M = [ 0  v  0  0]
       [ 0  0  v  0] 
       [ 0  0  0  v] 
  \endverbatim
*/
template<class T>
mat4<T>::mat4(T v) 
  : m11(v), m12(0), m13(0), m14(0),
    m21(0), m22(v), m23(0), m24(0),
    m31(0), m32(0), m33(v), m34(0),
    m41(0), m42(0), m43(0), m44(v)
{
}

/** 
  Constructor with 16 values in row-major order.

  \verbatim

       [ a  b  c  d] 
   M = [ e  f  g  h]
       [ i  j  k  l] 
       [ m  n  o  p] 
  \endverbatim
*/
template<class T>
mat4<T>::mat4(T a, T b, T c, T d, 
	      T e, T f, T g, T h, 
	      T i, T j, T k, T l, 
	      T m, T n, T o, T p)
  : m11(a), m12(b), m13(c), m14(d),
    m21(e), m22(f), m23(g), m24(h),
    m31(i), m32(j), m33(k), m34(l),
    m41(m), m42(n), m43(o), m44(p)
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
mat4<T>::mat4(const mat4<T>& A)
  : m11(A.m11), m12(A.m12), m13(A.m13), m14(A.m14),
    m21(A.m21), m22(A.m22), m23(A.m23), m24(A.m24),
    m31(A.m31), m32(A.m32), m33(A.m33), m34(A.m34),
    m41(A.m41), m42(A.m42), m43(A.m43), m44(A.m44)
{
}

/**
  Return element [i,j].

  The indices range from 0 to 3. Other values will cause an EIndexError
  exception to be thrown.

  @param i Row
  @param j Column
  @return Reference to element [i,j]
  @exception EIndexError
 */
template<class T>
inline T& mat4<T>::at(short i, short j)
{
  switch(i)
  {
   // Row 0
   case 0: 
     switch(j)
     {
     case 0: return m11;
     case 1: return m12;
     case 2: return m13;
     case 3: return m14;
     default: throw EIndexError();
     }
   // Row 1
   case 1:
     switch(j)
     {
     case 0: return m21;
     case 1: return m22;
     case 2: return m23;
     case 3: return m24;
     default: throw EIndexError();
     }
   // Row 2
   case 2:
     switch(j)
     {
     case 0: return m31;
     case 1: return m32;
     case 2: return m33;
     case 3: return m34;
     default: throw EIndexError();
     }
   // Row 3
   case 3:
     switch(j)
     {
     case 0: return m41;
     case 1: return m42;
     case 2: return m43;
     case 3: return m44;
     default: throw EIndexError();
     }
   default: throw EIndexError();
  }  
}


/*----------------------------setIdentity------------------------------*//**
  Set identity matrix. 

  \verbatim

       [ 1  0  0  0]
   M = [ 0  1  0  0]
       [ 0  0  1  0] 
       [ 0  0  0  1] 
  \endverbatim

  @return Reference to this.
*//*------------------------------------------------------------------------*/
template<class T> 
inline mat4<T>& mat4<T>::setIdentity()
{
  m11 = 1; m12 = 0; m13 = 0; m14 = 0;
  m21 = 0; m22 = 1; m23 = 0; m24 = 0;
  m31 = 0; m32 = 0; m33 = 1; m34 = 0;
  m41 = 0; m42 = 0; m43 = 0; m44 = 1;

  return *this;
}

/*-------------------------------setNull---------------------------------*/
/**
  Set all components to 0. 

  \verbatim

       [ 0  0  0  0]
   M = [ 0  0  0  0]
       [ 0  0  0  0] 
       [ 0  0  0  0] 
  \endverbatim

  @return Reference to this.
*/
/*-------------------------------------------------------------------------*/
template<class T> 
inline mat4<T>& mat4<T>::setNull()
{
  m11 = 0; m12 = 0; m13 = 0; m14 = 0;
  m21 = 0; m22 = 0; m23 = 0; m24 = 0;
  m31 = 0; m32 = 0; m33 = 0; m34 = 0;
  m41 = 0; m42 = 0; m43 = 0; m44 = 0;

  return *this;
}

/*-------------------------------setRow----------------------------------*/
/**
  Set row i to r. 

  \verbatim
   Example for i=0:

       [ rx ry rz rw]
   M = [ *  *  *  *]
       [ *  *  *  *]  
       [ *  *  *  *]  
  \endverbatim

  @param     i  Row index (0, 1, 2, 3)
  @param     r  Row.
  @return    Reference to this.
  @exception EIndexError Index is out of range.
  @see       getRow
*/
/*-------------------------------------------------------------------------*/
template<class T> 
mat4<T>& mat4<T>::setRow(short i, const vec4<T>& r)
{
  switch(i)
  {
   case 0: m11=r.x; m12=r.y; m13=r.z; m14=r.w; break;
   case 1: m21=r.x; m22=r.y; m23=r.z; m24=r.w; break;
   case 2: m31=r.x; m32=r.y; m33=r.z; m34=r.w; break;
   case 3: m41=r.x; m42=r.y; m43=r.z; m44=r.w; break;
   default: throw EIndexError();
  }
  return *this;
}

/*-------------------------------setRow----------------------------------*/
/**
  Set row i to [a, b, c, d]. 

  \verbatim
   Example for i=0:

       [ a  b  c  d]
   M = [ *  *  *  *]
       [ *  *  *  *] 
       [ *  *  *  *] 
  \endverbatim

  @param     i  Row index (0, 1, 2, 3)
  @param     a  First row element.
  @param     b  Second row element.
  @param     c  Third row element.
  @param     d  Fourth row element.
  @return    Reference to this.
  @exception EIndexError Index is out of range.
  @see       getRow
*/
/*-------------------------------------------------------------------------*/
template<class T> 
mat4<T>& mat4<T>::setRow(short i, T a, T b, T c, T d)
{
  switch(i)
  {
   case 0: m11=a; m12=b; m13=c; m14=d; break;
   case 1: m21=a; m22=b; m23=c; m24=d; break;
   case 2: m31=a; m32=b; m33=c; m34=d; break;
   case 3: m41=a; m42=b; m43=c; m44=d; break;
   default: throw EIndexError();
  }
  return *this;
}

/*-------------------------------setColumn------------------------------*//**
  Set column i to c. 

  \verbatim
   Example for i=0:

       [ cx *  *  *]
   M = [ cy *  *  *]
       [ cz *  *  *] 
       [ cw *  *  *] 
  \endverbatim

  @param     i  Column index (0, 1, 2, 3)
  @param     c  Column.
  @return    Reference to this.
  @exception EIndexError Index is out of range.
  @see       getColumn
*//*------------------------------------------------------------------------*/
template<class T> 
mat4<T>& mat4<T>::setColumn(short i, const vec4<T>& c)
{
  switch(i)
  {
   case 0: m11=c.x; m21=c.y; m31=c.z; m41=c.w; break;
   case 1: m12=c.x; m22=c.y; m32=c.z; m42=c.w; break;
   case 2: m13=c.x; m23=c.y; m33=c.z; m43=c.w; break;
   case 3: m14=c.x; m24=c.y; m34=c.z; m44=c.w; break;
   default: throw EIndexError();
  }
  return *this;
}

/*-------------------------------setColumn------------------------------*//**
  Set column i to [a, b, c]. 

  \verbatim
   Example for i=0:

       [ a  *  *  *]
   M = [ b  *  *  *]
       [ c  *  *  *] 
       [ d  *  *  *] 
  \endverbatim

  @param     i  Column index (0, 1, 2, 3)
  @param     a  First column element.
  @param     b  Second column element.
  @param     c  Third column element.
  @param     d  Fourth column element.
  @return    Reference to this.
  @exception EIndexError Index is out of range.
  @see       getColumn
*//*-----------------------------------------------------------------------*/
template<class T> 
mat4<T>& mat4<T>::setColumn(short i, T a, T b, T c, T d)
{
  switch(i)
  {
   case 0: m11=a; m21=b; m31=c; m41=d; break;
   case 1: m12=a; m22=b; m32=c; m42=d; break;
   case 2: m13=a; m23=b; m33=c; m43=d; break;
   case 3: m14=a; m24=b; m34=c; m44=d; break;
   default: throw EIndexError();
  }
  return *this;
}

/*-------------------------------setDiag----------------------------------*/
/**
  Set diagonal to d. 

  \verbatim

       [ dx *  *  *]
   M = [ *  dy *  *]
       [ *  *  dz *] 
       [ *  *  *  dw] 
  \endverbatim

  @param     d  New diagonal.
  @return    Reference to this.
  @see       getDiag
*/
/*-------------------------------------------------------------------------*/
template<class T> 
inline mat4<T>& mat4<T>::setDiag(const vec4<T>& d)
{
  m11 = d.x;
  m22 = d.y;
  m33 = d.z;
  m44 = d.w;
  return *this;
}

/*-------------------------------setDiag--------------------------------*//**
  Set diagonal to [a, b, c, d].

  \verbatim

       [ a  *  *  *]
   M = [ *  b  *  *]
       [ *  *  c  *] 
       [ *  *  *  d] 
  \endverbatim

  @param     a  First diagonal element.
  @param     b  Second diagonal element.
  @param     c  Third diagonal element.
  @param     d  Fourth diagonal element.
  @return    Reference to this.
  @see       getDiag
*//*-----------------------------------------------------------------------*/
template<class T>
mat4<T>& mat4<T>::setDiag(T a, T b, T c, T d)
{
  m11 = a;
  m22 = b;
  m33 = c;
  m44 = d;
  return *this;
}

/*-------------------------------getRow--------------------------------*//**
  Return row with index i. 

  @param     i  Row index (i=0,1,2,3).
  @return    Row i.
  @exception EIndexError Index is out of range.
  @see       setRow
*//*------------------------------------------------------------------------*/
template<class T> 
vec4<T> mat4<T>::getRow(short i) const
{
  switch(i)
  {
   case 0: return vec4<T>(m11, m12, m13, m14);
   case 1: return vec4<T>(m21, m22, m23, m24);
   case 2: return vec4<T>(m31, m32, m33, m34);
   case 3: return vec4<T>(m41, m42, m43, m44);
   default: throw EIndexError();
  }
}

/*-------------------------------getRow--------------------------------*//**
  Return row with index i. 

  @param     i  Row index (i=0,1,2,3).
  @param     dest  Receives result.
  @exception EIndexError Index is out of range.
  @see       setRow
*//*------------------------------------------------------------------------*/
template<class T> 
void mat4<T>::getRow(short i, vec4<T>& dest) const
{
  switch(i)
  {
   case 0: dest.set(m11, m12, m13, m14); break;
   case 1: dest.set(m21, m22, m23, m24); break;
   case 2: dest.set(m31, m32, m33, m34); break;
   case 3: dest.set(m41, m42, m43, m44); break;
   default: throw EIndexError();
  }
}

/*-------------------------------getRow--------------------------------*//**
  Return row with index i. 

  @param     i  Row index (i=0,1,2,3)
  @param     a  Element [i,0] (output).
  @param     b  Element [i,1] (output).
  @param     c  Element [i,2] (output).
  @param     d  Element [i,3] (output).
  @exception EIndexError Index is out of range.
  @see       setRow
*//*------------------------------------------------------------------------*/
template<class T>
void mat4<T>::getRow(short i, T& a, T& b, T& c, T& d) const
{
  switch(i)
  {
   case 0: a=m11; b=m12; c=m13; d=m14; break;
   case 1: a=m21; b=m22; c=m23; d=m24; break;
   case 2: a=m31; b=m32; c=m33; d=m34; break;
   case 3: a=m41; b=m42; c=m43; d=m44; break;
   default: throw EIndexError();
  }
}

/*-------------------------------getColumn------------------------------*//**
  Returns column with index i. 

  @param     i  Column index (i=0,1,2,3).
  @return    Column i.
  @exception EIndexError Index is out of range.
  @see       setColumn
*//*------------------------------------------------------------------------*/
template<class T> 
vec4<T> mat4<T>::getColumn(short i) const
{
  switch(i)
  {
   case 0: return vec4<T>(m11, m21, m31, m41);
   case 1: return vec4<T>(m12, m22, m32, m42);
   case 2: return vec4<T>(m13, m23, m33, m43);
   case 3: return vec4<T>(m14, m24, m34, m44);
   default: throw EIndexError();
  }
}

/*-------------------------------getColumn-----------------------------*//**
  Return column with index i. 

  @param     i  Column index (i=0,1,2,3).
  @param     dest  Receives result.
  @exception EIndexError Index is out of range.
  @see       setColumn
*//*------------------------------------------------------------------------*/
template<class T> 
void mat4<T>::getColumn(short i, vec4<T>& dest) const
{
  switch(i)
  {
   case 0: dest.set(m11, m21, m31, m41); break;
   case 1: dest.set(m12, m22, m32, m42); break;
   case 2: dest.set(m13, m23, m33, m43); break;
   case 3: dest.set(m14, m24, m34, m44); break;
   default: throw EIndexError();
  }
}

/*-------------------------------getColumn------------------------------*//**
  Return column with index i. 

  @param     i  Column index (i=0,1,2,3).
  @param     a  Element [0,i] (output).
  @param     b  Element [1,i] (output).
  @param     c  Element [2,i] (output).
  @param     d  Element [3,i] (output).
  @exception EIndexError Index is out of range.
  @see       setColumn
*//*------------------------------------------------------------------------*/
template<class T>
void mat4<T>::getColumn(short i, T& a, T& b, T& c, T& d) const
{
  switch(i)
  {
   case 0: a=m11; b=m21; c=m31; d=m41; break;
   case 1: a=m12; b=m22; c=m32; d=m42; break;
   case 2: a=m13; b=m23; c=m33; d=m43; break;
   case 3: a=m14; b=m24; c=m34; d=m44; break;
   default: throw EIndexError();
  }
}

/*-------------------------------getDiag--------------------------------*//**
  Return diagonal. 

  @return    Diagonal.
  @see       setDiag
*//*------------------------------------------------------------------------*/
template<class T> 
inline vec4<T> mat4<T>::getDiag() const
{
  return vec4<T>(m11, m22, m33, m44);
}

/*-------------------------------getDiag--------------------------------*//**
  Return diagonal. 

  @param     dest  Receives result.
  @return    Diagonal.
  @see       setDiag
*//*------------------------------------------------------------------------*/
template<class T> 
inline void mat4<T>::getDiag(vec4<T>& dest) const
{
  dest.set(m11, m22, m33, m44);
}

/*-------------------------------getDiag--------------------------------*//**
  Return diagonal. 

  @param     a  Element [0,0] (output).
  @param     b  Element [1,1] (output).
  @param     c  Element [2,2] (output).
  @param     d  Element [3,3] (output).
  @return    Diagonal.
  @see       setDiag
*//*------------------------------------------------------------------------*/
template<class T> 
inline void mat4<T>::getDiag(T& a, T& b, T& c, T& d) const
{
  a = m11;
  b = m22;
  c = m33;
  d = m44;
}

/*-----------------------------setTranslation----------------------------*//**
  Set a translation transformation.

  \param t Translation vector.
  \return  Reference to this.
*//*-----------------------------------------------------------------------*/
template<class T> 
mat4<T>& mat4<T>::setTranslation(const vec3<T>& t)
{
  m11=1; m12=0; m13=0; m14=t.x;
  m21=0; m22=1; m23=0; m24=t.y;
  m31=0; m32=0; m33=1; m34=t.z;
  m41=0; m42=0; m43=0; m44=1;
  return *this;
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
  mat4<float> R;
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
mat4<T>& mat4<T>::setRotation(T angle, const vec3<T>& axis)
{
  mat3<T> R;
  R.setRotation(angle, axis);
  R.getRow(0, m11, m12, m13);
  R.getRow(1, m21, m22, m23);
  R.getRow(2, m31, m32, m33);
  m14=0;
  m24=0;
  m34=0;
  m41=0; m42=0; m43=0; m44=1; 
  return *this;
}

/*-------------------------------setScaling-----------------------------*//**
  Set a scaling transformation.

  \param s Scaling vector.
  \return  Reference to this.
*//*-----------------------------------------------------------------------*/
template<class T> 
mat4<T>& mat4<T>::setScaling(const vec3<T>& s)
{
  m11=s.x; m12=0; m13=0; m14=0;
  m21=0; m22=s.y; m23=0; m24=0;
  m31=0; m32=0; m33=s.z; m34=0;
  m41=0; m42=0; m43=0; m44=1;
  return *this;
}

/*-----------------------------setOrthographic---------------------------*//**
  Set an orthographic transformation.

  This method is equivalent to the OpenGL command glOrtho().

  \return Reference to this.
  \todo Exceptions wieder aktivieren
*//*-----------------------------------------------------------------------*/
template<class T> 
mat4<T>& mat4<T>::setOrthographic(T left, T right, T bottom, T top, T nearval, T farval)
{
  double r_l = right-left;
  double t_b = top-bottom;
  double f_n = farval-nearval;
  
  if (r_l<=vec3<T>::epsilon)
  {
    //raise ValueError, "right-value must be greater than left-value";
  }
  if (t_b<=vec3<T>::epsilon)
  {
   // raise ValueError, "top-value must be greater than bottom-value"
  }
  if (f_n<=vec3<T>::epsilon)
  {
   // raise ValueError, "far-value must be greater than near-value"
  }

  m11 = 2.0/r_l;
  m12 = 0.0;
  m13 = 0.0;
  m14 = -(right+left)/r_l;
  m21 = 0.0;
  m22 = 2.0/t_b;
  m23 = 0.0;
  m24 = -(top+bottom)/t_b;
  m31 = 0.0;
  m32 = 0.0;
  m33 = -2.0/f_n;
  m34 = -(farval+nearval)/f_n;
  m41 = 0.0;
  m42 = 0.0;
  m43 = 0.0;
  m44 = 1.0;
  return *this;
}

/*-------------------------------setFrustum-----------------------------*//**
  Set a perspective transformation.

  This method is equivalent to the OpenGL command glFrustum().

  \return Reference to this.
*//*-----------------------------------------------------------------------*/
template<class T> 
mat4<T>& mat4<T>::setFrustum(T left, T right, T bottom, T top, T near_, T far_)
{
  T r_l = right-left;
  T t_b = top-bottom;
  T f_n = far_-near_;

  if (r_l<=vec3<T>::epsilon)
  {
    //raise ValueError, "right-value must be greater than left-value";
  }
  if (t_b<=vec3<T>::epsilon)
  {
   // raise ValueError, "top-value must be greater than bottom-value"
  }
  if (f_n<=vec3<T>::epsilon)
  {
   // raise ValueError, "far-value must be greater than near-value"
  }

  m11 = (2.0*near_)/r_l;
  m12 = 0.0;
  m13 = (right+left)/r_l;
  m14 = 0.0;
  m21 = 0.0;
  m22 = (2.0*near_)/t_b;
  m23 = (top+bottom)/t_b;
  m24 = 0.0;
  m31 = 0.0;
  m32 = 0.0;
  m33 = -(far_+near_)/f_n;
  m34 = (-2.0*far_*near_)/f_n;
  m41 = 0.0;
  m42 = 0.0;
  m43 = -1.0;
  m44 = 0.0;
  return *this;
}


/*--------------------------------setPerspective---------------------------*//**
  Set a perspective transformation.

  This method is equivalent to the OpenGL utility command gluPerspective().

  \return Reference to this.
*//*-----------------------------------------------------------------------*/
template<class T> 
mat4<T>& mat4<T>::setPerspective(T fovy, T aspect, T near_, T far_)
{
  T top = near_ * tan(fovy * 3.1415926535897931 / 360.0);
  T bottom = -top;
  T left = bottom*aspect;
  T right = top*aspect;

  setFrustum(left, right, bottom, top, near_, far_);
  return *this;
}

/*----------------------------------setLookAt-----------------------------*//**
  Look from pos to target.

  The resulting transformation moves the origin to pos and
  rotates so that The z-axis points to target. The y-axis is
  as close as possible to the up vector.

  \return Reference to this.
*//*-----------------------------------------------------------------------*/
template<class T> 
mat4<T>& mat4<T>::setLookAt(const vec3<T>& pos, const vec3<T>& target, const vec3<T>& up)
{
  vec3<T> dir = target - pos;
  vec3<T> vup(up);
  vec3<T> right;

  dir.normalize(dir);
  vup.normalize(vup);
  vup -= (vup*dir) * dir;

  try
  {
	vup.normalize(vup);
  }
  catch(...)
  {
    // We're looking along the up direction, so choose
    // an arbitrary direction that is perpendicular to dir
    // as new up.
    vup = dir.ortho();
  }

  right.cross(vup, dir);
  right.normalize(right);

  m11 = right.x;
  m21 = right.y;
  m31 = right.z;
  m41 = 0.0;
  m12 = vup.x;
  m22 = vup.y;
  m32 = vup.z;
  m42 = 0.0;
  m13 = dir.x;
  m23 = dir.y;
  m33 = dir.z;
  m43 = 0.0;
  m14 = pos.x;
  m24 = pos.y;
  m34 = pos.z;
  m44 = 1.0;
  return *this;
}

/*---------------------------------setMat3------------------------------*//**
  Set the mat3 part (col/row 0-2) to a given matrix.

  @return  Reference to this.
*//*-----------------------------------------------------------------------*/
template<class T> 
mat4<T>& mat4<T>::setMat3(const mat3<T>& m3)
{
  m3.getRow(0, m11, m12, m13);
  m3.getRow(1, m21, m22, m23);
  m3.getRow(2, m31, m32, m33);
  return *this;
}

/*---------------------------------getMat3------------------------------*//**
  Return the mat3 part.

  \return Column/row 0-2 of this as a mat3.
*//*-----------------------------------------------------------------------*/
template<class T> 
mat3<T> mat4<T>::getMat3() const
{
  return mat3<T>(m11, m12, m13, m21, m22, m23, m31, m32, m33);
}

/*---------------------------------getMat3------------------------------*//**
  Return the mat3 part.

  \param dest  Matrix that receives the value.
  \return A reference to dest.
*//*-----------------------------------------------------------------------*/
template<class T> 
mat3<T>& mat4<T>::getMat3(mat3<T>& dest) const
{
  dest.setRow(0, m11, m12, m13);
  dest.setRow(1, m21, m22, m23);
  dest.setRow(2, m31, m32, m33);
  return dest;
}


/*-------------------------------operator+=-----------------------------*//**
  matrix+=matrix.

  @return  Reference to this.
*//*-----------------------------------------------------------------------*/
template<class T> 
inline mat4<T>& mat4<T>::operator+=(const mat4<T>& A)
{ 
  m11+=A.m11;
  m12+=A.m12;
  m13+=A.m13;
  m14+=A.m14;
  m21+=A.m21;
  m22+=A.m22;
  m23+=A.m23;
  m24+=A.m24;
  m31+=A.m31;
  m32+=A.m32;
  m33+=A.m33;
  m34+=A.m34;
  m41+=A.m41;
  m42+=A.m42;
  m43+=A.m43;
  m44+=A.m44;
  return *this; 
}

/*-------------------------------operator-=-----------------------------*//**
  matrix-=matrix.

  @return  Reference to this.
*//*-----------------------------------------------------------------------*/
template<class T> 
inline mat4<T>& mat4<T>::operator-=(const mat4<T>& A)
{ 
  m11-=A.m11;
  m12-=A.m12;
  m13-=A.m13;
  m14-=A.m14;
  m21-=A.m21;
  m22-=A.m22;
  m23-=A.m23;
  m24-=A.m24;
  m31-=A.m31;
  m32-=A.m32;
  m33-=A.m33;
  m34-=A.m34;
  m41-=A.m41;
  m42-=A.m42;
  m43-=A.m43;
  m44-=A.m44;
  return *this; 
}

/*----------------------------operator*= (scalar)-----------------------*//**
  matrix*=scalar.

  @return  Reference to this.
*//*-----------------------------------------------------------------------*/
template<class T> 
inline mat4<T>& mat4<T>::operator*=(const T s)
{ 
  m11*=s;
  m12*=s;
  m13*=s;
  m14*=s;
  m21*=s;
  m22*=s;
  m23*=s;
  m24*=s;
  m31*=s;
  m32*=s;
  m33*=s;
  m34*=s;
  m41*=s;
  m42*=s;
  m43*=s;
  m44*=s;
  return *this; 
}

/*----------------------------operator/= (scalar)-----------------------*//**
  matrix/=scalar.

  @return     Reference to this.
  @exception  EZeroDivisionError  Is thrown when s=0.
*//*-----------------------------------------------------------------------*/
template<class T> 
inline mat4<T>& mat4<T>::operator/=(const T s)
{ 
  if (xabs(s) <= vec3<T>::epsilon) throw EZeroDivisionError("mat4: divide by zero");

  m11/=s;
  m12/=s;
  m13/=s;
  m14/=s;
  m21/=s;
  m22/=s;
  m23/=s;
  m24/=s;
  m31/=s;
  m32/=s;
  m33/=s;
  m34/=s;
  m41/=s;
  m42/=s;
  m43/=s;
  m44/=s;
  return *this; 
}

/*-----------------------------operator%=-------------------------------*//**
  matrix %= scalar.

  @return    Reference to this.
  @exception EZeroDivisionError  Is thrown when r=0.
*//*------------------------------------------------------------------------*/
template<class T>
inline mat4<T>& mat4<T>::operator%=(const T r)
{
  // r=0 ? then throw Exception
  if ( (r > vec3<T>::epsilon) || (r < -vec3<T>::epsilon) ) 
  {
    m11 = _fmod(m11,r); 
    m12 = _fmod(m12,r); 
    m13 = _fmod(m13,r); 
    m14 = _fmod(m14,r); 
    m21 = _fmod(m21,r); 
    m22 = _fmod(m22,r); 
    m23 = _fmod(m23,r); 
    m24 = _fmod(m24,r); 
    m31 = _fmod(m31,r); 
    m32 = _fmod(m32,r); 
    m33 = _fmod(m33,r); 
    m34 = _fmod(m34,r); 
    m41 = _fmod(m41,r); 
    m42 = _fmod(m42,r); 
    m43 = _fmod(m43,r); 
    m44 = _fmod(m44,r); 
  }
  else
  {
    throw EZeroDivisionError("mat4: division by zero");
  }
  return *this;
}

/*-----------------------------operator%=-------------------------------*//**
  matrix %= matrix

  @return    Reference to this.
  @exception EZeroDivisionError  Is thrown when a component of b=0.
*//*------------------------------------------------------------------------*/
template<class T>
inline mat4<T>& mat4<T>::operator%=(const mat4<T>& b)
{
  if ( ((b.m11 > vec3<T>::epsilon) || (b.m11 < -vec3<T>::epsilon)) &&
       ((b.m12 > vec3<T>::epsilon) || (b.m12 < -vec3<T>::epsilon)) &&
       ((b.m13 > vec3<T>::epsilon) || (b.m13 < -vec3<T>::epsilon)) &&
       ((b.m14 > vec3<T>::epsilon) || (b.m14 < -vec3<T>::epsilon)) &&
       ((b.m21 > vec3<T>::epsilon) || (b.m21 < -vec3<T>::epsilon)) &&
       ((b.m22 > vec3<T>::epsilon) || (b.m22 < -vec3<T>::epsilon)) &&
       ((b.m23 > vec3<T>::epsilon) || (b.m23 < -vec3<T>::epsilon)) &&
       ((b.m24 > vec3<T>::epsilon) || (b.m24 < -vec3<T>::epsilon)) &&
       ((b.m31 > vec3<T>::epsilon) || (b.m31 < -vec3<T>::epsilon)) &&
       ((b.m32 > vec3<T>::epsilon) || (b.m32 < -vec3<T>::epsilon)) &&
       ((b.m33 > vec3<T>::epsilon) || (b.m33 < -vec3<T>::epsilon)) &&
       ((b.m34 > vec3<T>::epsilon) || (b.m34 < -vec3<T>::epsilon)) &&
       ((b.m41 > vec3<T>::epsilon) || (b.m41 < -vec3<T>::epsilon)) &&
       ((b.m42 > vec3<T>::epsilon) || (b.m42 < -vec3<T>::epsilon)) &&
       ((b.m43 > vec3<T>::epsilon) || (b.m43 < -vec3<T>::epsilon)) &&
       ((b.m44 > vec3<T>::epsilon) || (b.m44 < -vec3<T>::epsilon)) )
  {
    m11 = _fmod(m11, b.m11); 
    m12 = _fmod(m12, b.m12); 
    m13 = _fmod(m13, b.m13); 
    m14 = _fmod(m14, b.m14); 
    m21 = _fmod(m21, b.m21); 
    m22 = _fmod(m22, b.m22); 
    m23 = _fmod(m23, b.m23); 
    m24 = _fmod(m24, b.m24); 
    m31 = _fmod(m31, b.m31); 
    m32 = _fmod(m32, b.m32); 
    m33 = _fmod(m33, b.m33); 
    m34 = _fmod(m34, b.m34); 
    m41 = _fmod(m41, b.m41); 
    m42 = _fmod(m42, b.m42); 
    m43 = _fmod(m43, b.m43); 
    m44 = _fmod(m44, b.m44); 
  }
  else
  {
    throw EZeroDivisionError("mat4: division by zero");
  }
  return *this;
}


/*--------------------------------operator+----------------------------*//**
  matrix=matrix+matrix.

  @return  Sum of two matrices.
*//*-----------------------------------------------------------------------*/
template<class T> 
inline mat4<T> mat4<T>::operator+(const mat4<T>& A) const
{
  return mat4<T>(m11+A.m11, m12+A.m12, m13+A.m13, m14+A.m14,
                 m21+A.m21, m22+A.m22, m23+A.m23, m24+A.m24,
                 m31+A.m31, m32+A.m32, m33+A.m33, m34+A.m34,
                 m41+A.m41, m42+A.m42, m43+A.m43, m44+A.m44);
}

/*--------------------------------operator- ----------------------------*//**
  matrix=matrix-matrix.

  @return  Difference of two matrices.
*//*-----------------------------------------------------------------------*/
template<class T> 
inline mat4<T> mat4<T>::operator-(const mat4<T>& A) const
{
  return mat4<T>(m11-A.m11, m12-A.m12, m13-A.m13, m14-A.m14,
                 m21-A.m21, m22-A.m22, m23-A.m23, m24-A.m24,
                 m31-A.m31, m32-A.m32, m33-A.m33, m34-A.m34,
                 m41-A.m41, m42-A.m42, m43-A.m43, m44-A.m44);
}

/*----------------------------operator- (unary)-------------------------*//**
  matrix=-matrix.

  @return  Negated matrix.
*//*-----------------------------------------------------------------------*/
template<class T> 
inline mat4<T> mat4<T>::operator-() const
{
  return mat4<T>(-m11, -m12, -m13, -m14,
                 -m21, -m22, -m23, -m24,
                 -m31, -m32, -m33, -m34,
                 -m41, -m42, -m43, -m44);
}


/*--------------------------------operator*----------------------------*//**
  matrix=matrix*matrix.

  @return  Product of two matrices.
*//*-----------------------------------------------------------------------*/
template<class T> 
inline mat4<T> mat4<T>::operator*(const mat4<T>& A) const
{
  return mat4<T>(m11*A.m11+m12*A.m21+m13*A.m31+m14*A.m41,
                 m11*A.m12+m12*A.m22+m13*A.m32+m14*A.m42,
                 m11*A.m13+m12*A.m23+m13*A.m33+m14*A.m43,
                 m11*A.m14+m12*A.m24+m13*A.m34+m14*A.m44,

                 m21*A.m11+m22*A.m21+m23*A.m31+m24*A.m41,
                 m21*A.m12+m22*A.m22+m23*A.m32+m24*A.m42,
                 m21*A.m13+m22*A.m23+m23*A.m33+m24*A.m43,
                 m21*A.m14+m22*A.m24+m23*A.m34+m24*A.m44,

                 m31*A.m11+m32*A.m21+m33*A.m31+m34*A.m41,
                 m31*A.m12+m32*A.m22+m33*A.m32+m34*A.m42,
                 m31*A.m13+m32*A.m23+m33*A.m33+m34*A.m43,
                 m31*A.m14+m32*A.m24+m33*A.m34+m34*A.m44,

                 m41*A.m11+m42*A.m21+m43*A.m31+m44*A.m41,
                 m41*A.m12+m42*A.m22+m43*A.m32+m44*A.m42,
                 m41*A.m13+m42*A.m23+m43*A.m33+m44*A.m43,
                 m41*A.m14+m42*A.m24+m43*A.m34+m44*A.m44);
}

/*--------------------------------operator*=----------------------------*//**
  matrix*=matrix.

  A may also be this.

  @return  Reference to this.
*//*-----------------------------------------------------------------------*/
template<class T> 
inline mat4<T>& mat4<T>::operator*=(const mat4<T>& A)
{
  double _m11 = m11*A.m11+m12*A.m21+m13*A.m31+m14*A.m41;
  double _m12 = m11*A.m12+m12*A.m22+m13*A.m32+m14*A.m42;
  double _m13 = m11*A.m13+m12*A.m23+m13*A.m33+m14*A.m43;
  double _m14 = m11*A.m14+m12*A.m24+m13*A.m34+m14*A.m44;

  double _m21 = m21*A.m11+m22*A.m21+m23*A.m31+m24*A.m41;
  double _m22 = m21*A.m12+m22*A.m22+m23*A.m32+m24*A.m42;
  double _m23 = m21*A.m13+m22*A.m23+m23*A.m33+m24*A.m43;
  double _m24 = m21*A.m14+m22*A.m24+m23*A.m34+m24*A.m44;

  double _m31 = m31*A.m11+m32*A.m21+m33*A.m31+m34*A.m41;
  double _m32 = m31*A.m12+m32*A.m22+m33*A.m32+m34*A.m42;
  double _m33 = m31*A.m13+m32*A.m23+m33*A.m33+m34*A.m43;
  double _m34 = m31*A.m14+m32*A.m24+m33*A.m34+m34*A.m44;

  double _m41 = m41*A.m11+m42*A.m21+m43*A.m31+m44*A.m41;
  double _m42 = m41*A.m12+m42*A.m22+m43*A.m32+m44*A.m42;
  double _m43 = m41*A.m13+m42*A.m23+m43*A.m33+m44*A.m43;
  double _m44 = m41*A.m14+m42*A.m24+m43*A.m34+m44*A.m44;

  m11 = _m11;
  m12 = _m12;
  m13 = _m13;
  m14 = _m14;
  m21 = _m21;
  m22 = _m22;
  m23 = _m23;
  m24 = _m24;
  m31 = _m31;
  m32 = _m32;
  m33 = _m33;
  m34 = _m34;
  m41 = _m41;
  m42 = _m42;
  m43 = _m43;
  m44 = _m44;

  return *this;
}

/*--------------------------------operator*-----------------------------*//**
  vector=matrix*vector.

  @return  Product of a matrix and a vector.
*//*-----------------------------------------------------------------------*/
template<class T> 
inline vec4<T> mat4<T>::operator*(const vec4<T>& v) const
{
  return vec4<T>(m11*v.x + m12*v.y + m13*v.z + m14*v.w,
                 m21*v.x + m22*v.y + m23*v.z + m24*v.w,
                 m31*v.x + m32*v.y + m33*v.z + m34*v.w,
                 m41*v.x + m42*v.y + m43*v.z + m44*v.w);
}

/*--------------------------------operator*-----------------------------*//**
  vector=matrix*vec3.

  The missing component in the vec3 is implicitly set to 1.0

  @return  Product of a matrix and a vector.
*//*-----------------------------------------------------------------------*/
template<class T> 
inline vec3<T> mat4<T>::operator*(const vec3<T>& v) const
{
  vec3<T> res(m11*v.x + m12*v.y + m13*v.z + m14,
	      m21*v.x + m22*v.y + m23*v.z + m24,
	      m31*v.x + m32*v.y + m33*v.z + m34);

  T w = m41*v.x + m42*v.y + m43*v.z + m44;

  if (xabs(w)>vec3<T>::epsilon)
  {
    res/=w;
  }

  return res;
}

/*--------------------------------operator*-----------------------------*//**
  matrix=matrix*scalar.

  @return  Product of a matrix and a scalar.
*//*-----------------------------------------------------------------------*/
template<class T> 
inline mat4<T> mat4<T>::operator*(const T s) const
{ 
  return mat4<T>(m11*s, m12*s, m13*s, m14*s,  
                 m21*s, m22*s, m23*s, m24*s,  
                 m31*s, m32*s, m33*s, m34*s,  
                 m41*s, m42*s, m43*s, m44*s);
}

/*--------------------------------operator/-----------------------------*//**
  matrix=matrix/scalar.

  @return     Matrix diveded by scalar (component wise).
  @exception  EZeroDivisionError  Is returned when s=0.
*//*-----------------------------------------------------------------------*/
template<class T> 
inline mat4<T> mat4<T>::operator/(const T s) const
{ 
  if (xabs(s) <= vec3<T>::epsilon) throw EZeroDivisionError("mat4: divide by zero");

  return mat4<T>(m11/s, m12/s, m13/s, m14/s,  
                 m21/s, m22/s, m23/s, m24/s,  
                 m31/s, m32/s, m33/s, m34/s,  
                 m41/s, m42/s, m43/s, m44/s);
}

/*-----------------------------operator% -------------------------------*//**
  mat = mat % scalar.

  @return    Matrix modulo scalar (component wise).
  @exception EZeroDivisionError  Is returned when r=0.
*//*------------------------------------------------------------------------*/
template<class T>
inline mat4<T> mat4<T>::operator%(const T r) const
{
  mat4<T> res(*this);
  res %= r;
  return res;
}

/*-----------------------------operator% -------------------------------*//**
  mat = mat % mat

  @return    Matrix modulo matrix (component wise).
  @exception EZeroDivisionError  Is returned when component of b=0.
*//*------------------------------------------------------------------------*/
template<class T>
inline mat4<T> mat4<T>::operator%(const mat4<T>& b) const
{
  mat4<T> res(*this);
  res %= b;
  return res;
}


/*------------------------------operator==------------------------------*//**
  matrix==matrix

  @return     True, if matrices are equal.
*//*-----------------------------------------------------------------------*/
template<class T>
inline bool mat4<T>::operator==(const mat4<T>& A) const
{
  return (fabs(m11-A.m11)<=vec3<T>::epsilon) &&
         (fabs(m12-A.m12)<=vec3<T>::epsilon) &&
         (fabs(m13-A.m13)<=vec3<T>::epsilon) &&
         (fabs(m14-A.m14)<=vec3<T>::epsilon) &&
         (fabs(m21-A.m21)<=vec3<T>::epsilon) &&
         (fabs(m22-A.m22)<=vec3<T>::epsilon) &&
         (fabs(m23-A.m23)<=vec3<T>::epsilon) &&
         (fabs(m24-A.m24)<=vec3<T>::epsilon) &&
         (fabs(m31-A.m31)<=vec3<T>::epsilon) &&
         (fabs(m32-A.m32)<=vec3<T>::epsilon) &&
         (fabs(m33-A.m33)<=vec3<T>::epsilon) &&
         (fabs(m34-A.m34)<=vec3<T>::epsilon) &&
         (fabs(m41-A.m41)<=vec3<T>::epsilon) &&
         (fabs(m42-A.m42)<=vec3<T>::epsilon) &&
         (fabs(m43-A.m43)<=vec3<T>::epsilon) &&
         (fabs(m44-A.m44)<=vec3<T>::epsilon);
}

/*------------------------------operator!=------------------------------*//**
  matrix!=matrix

  @return     True, if matrices are not equal.
*//*-----------------------------------------------------------------------*/
template<class T>
inline bool mat4<T>::operator!=(const mat4<T>& A) const
{
  return !(*this==A);
}

/*----------------------------------------------------------------------*//**
  Copy the matrix values into an array.

  \pre The size of dest is at least 16
  \param dest The target array (this must be big enough to hold 16 values)
  \param rowmajor If true the values are laid out in row major order
*//*-----------------------------------------------------------------------*/
template<class T> 
void mat4<T>::toList(T* dest, bool rowmajor) const
{
  if (rowmajor)
  {
    dest[0] = m11;
    dest[1] = m12;
    dest[2] = m13;
    dest[3] = m14;
    dest[4] = m21;
    dest[5] = m22;
    dest[6] = m23;
    dest[7] = m24;
    dest[8] = m31;
    dest[9] = m32;
    dest[10] = m33;
    dest[11] = m34;
    dest[12] = m41;
    dest[13] = m42;
    dest[14] = m43;
    dest[15] = m44;
  }
  else
  {
    dest[0] = m11;
    dest[1] = m21;
    dest[2] = m31;
    dest[3] = m41;
    dest[4] = m12;
    dest[5] = m22;
    dest[6] = m32;
    dest[7] = m42;
    dest[8] = m13;
    dest[9] = m23;
    dest[10] = m33;
    dest[11] = m43;
    dest[12] = m14;
    dest[13] = m24;
    dest[14] = m34;
    dest[15] = m44;
  }
}

/*----------------------------------------------------------------------*//**
  Initialize the matrix from an array.

  \param array  An array with 16 values
  \param rowmajor If true the values are interpreted in row major order
*//*-----------------------------------------------------------------------*/
template<class T> 
void mat4<T>::fromList(T* array, bool rowmajor)
{
  if (rowmajor)
  {
    m11 = array[0];
    m12 = array[1];
    m13 = array[2];
    m14 = array[3];
    m21 = array[4];
    m22 = array[5];
    m23 = array[6];
    m24 = array[7];
    m31 = array[8];
    m32 = array[9];
    m33 = array[10];
    m34 = array[11];
    m41 = array[12];
    m42 = array[13];
    m43 = array[14];
    m44 = array[15];
  }
  else
  {
    m11 = array[0];
    m21 = array[1];
    m31 = array[2];
    m41 = array[3];
    m12 = array[4];
    m22 = array[5];
    m32 = array[6];
    m42 = array[7];
    m13 = array[8];
    m23 = array[9];
    m33 = array[10];
    m43 = array[11];
    m14 = array[12];
    m24 = array[13];
    m34 = array[14];
    m44 = array[15];
  }
}


/*-----------------------------determinant-------------------------------*//**
  Calculate the determinant.

  @return  Determinant.
*//*-----------------------------------------------------------------------*/
template<class T> 
inline T mat4<T>::determinant() const
{
  return m11*m22*m33*m44 
         -m11*m22*m34*m43 
         +m11*m23*m34*m42 
         -m11*m23*m32*m44 
         +m11*m24*m32*m43 
         -m11*m24*m33*m42 
         -m12*m23*m34*m41 
         +m12*m23*m31*m44 
         -m12*m24*m31*m43 
         +m12*m24*m33*m41 
         -m12*m21*m33*m44 
         +m12*m21*m34*m43 
         +m13*m24*m31*m42 
         -m13*m24*m32*m41 
         +m13*m21*m32*m44 
         -m13*m21*m34*m42 
         +m13*m22*m34*m41 
         -m13*m22*m31*m44 
         -m14*m21*m32*m43 
         +m14*m21*m33*m42 
         -m14*m22*m33*m41 
         +m14*m22*m31*m43 
         -m14*m23*m31*m42 
         +m14*m23*m32*m41;
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
inline mat4<T>& mat4<T>::inverse(mat4<T>& dest) const
{
  T m4[16];
  T m4_res[16];

  T det = determinant();
  if (xabs(det)<=vec3<T>::epsilon) throw EZeroDivisionError("mat4.inverse(): divide by zero");
  T _det = 1.0/det;

  toList(m4, true);

  for(short i=0; i<4; i++)
  {
    for(short j=0; j<4; j++)
    {
      short sign = 1-((i+j)%2)*2;
      T d = _subdet(m4,i,j);
      m4_res[j*4+i] = sign*d*_det;
    }
  }

  dest.fromList(m4_res, true);
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
inline mat4<T> mat4<T>::inverse() const
{
  mat4<T> res;
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
inline mat4<T>& mat4<T>::transpose(mat4<T>& dest) const
{
  if (&dest==this)
  {
    T buf[16];
    toList(buf, true);
    fromList(buf, false);
  }
  else
  {
    dest.m11 = m11;
    dest.m12 = m21;
    dest.m13 = m31;
    dest.m14 = m41;
    dest.m21 = m12;
    dest.m22 = m22;
    dest.m23 = m32;
    dest.m24 = m42;
    dest.m31 = m13;
    dest.m32 = m23;
    dest.m33 = m33;
    dest.m34 = m43;
    dest.m41 = m14;
    dest.m42 = m24;
    dest.m43 = m34;
    dest.m44 = m44;
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
inline mat4<T> mat4<T>::transpose() const
{
  return mat4<T>(m11, m21, m31, m41, 
		 m12, m22, m32, m42,
		 m13, m23, m33, m43,
		 m14, m24, m34, m44);
}

/*--------------------------------translate-----------------------------*//**
  Concatenate a translation.

  This method is equivalent to this*T where T is the translation matrix.

  \return Reference to this.
*//*-----------------------------------------------------------------------*/
template<class T> 
inline mat4<T>& mat4<T>::translate(const vec3<T>& t)
{
  m14 = m11*t.x + m12*t.y + m13*t.z + m14;
  m24 = m21*t.x + m22*t.y + m23*t.z + m24;
  m34 = m31*t.x + m32*t.y + m33*t.z + m34;
  m44 = m41*t.x + m42*t.y + m43*t.z + m44;
  return *this;
}

/*----------------------------------scale-------------------------------*//**
  Concatenate a scaling transformation.

  This method is equivalent to this*S where S is the scaling matrix.

  \return Reference to this.
*//*-----------------------------------------------------------------------*/
template<class T> 
inline mat4<T>& mat4<T>::scale(const vec3<T>& s)
{
  T sx=s.x, sy=s.y, sz=s.z;

  m11 *= sx;
  m12 *= sy;
  m13 *= sz;
  m21 *= sx;
  m22 *= sy;
  m23 *= sz;
  m31 *= sx;
  m32 *= sy;
  m33 *= sz;
  m41 *= sx;
  m42 *= sy;
  m43 *= sz;
  return *this;
}

/*----------------------------------rotate------------------------------*//**
  Concatenate a rotation transformation.

  This method is equivalent to this*S where S is the scaling matrix.

  \param  axis   Rotate around this vector
  \param  angle  Angle in radians
  \return Reference to this.
*//*-----------------------------------------------------------------------*/
template<class T> 
mat4<T>& mat4<T>::rotate(T angle, const vec3<T>& axis)
{
  mat4<T> R;
  R.setRotation(angle, axis);
  (*this)*=R;
  return *this;
}


/*----------------------------------ortho------------------------------*//**
  Make base vectors orthogonal.

  Makes the x-, y- and z-axis orthogonal.
  The fourth column and row remain untouched.

  \param  dest Destination matrix that receives the result (may be this).
  \return Reference to dest
*//*-----------------------------------------------------------------------*/
template<class T> 
mat4<T>& mat4<T>::ortho(mat4<T>& dest) const
{
  mat3<T> M(m11, m12, m13,  m21, m22, m23,  m31, m32, m33);
  M.ortho(M);
  M.getRow(0, dest.m11, dest.m12, dest.m13); 
  M.getRow(1, dest.m21, dest.m22, dest.m23); 
  M.getRow(2, dest.m31, dest.m32, dest.m33); 
  dest.m14 = m14;
  dest.m24 = m24;
  dest.m34 = m34;
  dest.m44 = m44;
  dest.m41 = m41;
  dest.m42 = m42;
  dest.m43 = m43;
  return dest;
}

/*----------------------------------ortho------------------------------*//**
  Make base vectors orthogonal.

  this remains unchanged.

  \return Matrix with orthogonal vectors.
*//*-----------------------------------------------------------------------*/
template<class T> 
mat4<T> mat4<T>::ortho() const
{
  mat4<T> res;
  return ortho(res);
}

/*---------------------------------decompose-----------------------------*//**
  Decompose the matrix into a translation, rotation and scaling part.

  If there was a scaling part that was zero then 
  a EZeroDivisionError exception is thrown.

  \param t  Receives the translation part
  \param rot  Receives the rotation part
  \param scale Receives the scaling part
*//*-----------------------------------------------------------------------*/
template<class T> 
void mat4<T>::decompose(vec3<T>& t, mat4<T>& rot, vec3<T>& scale) const
{
  vec3<T> a,b,c;
  T al, bl, cl;

  ortho(rot);
  rot.m14 = 0;
  rot.m24 = 0;
  rot.m34 = 0;
  rot.m44 = 1;
  rot.m41 = 0;
  rot.m42 = 0;
  rot.m43 = 0;

  // a,b,c = Column 0,1,2 of m
  a.set(rot.m11, rot.m21, rot.m31);
  b.set(rot.m12, rot.m22, rot.m32);
  c.set(rot.m13, rot.m23, rot.m33);

  // al,bl,cl will be the scaling factors
  al = a.length();
  bl = b.length();
  cl = c.length();
  if ( (al<=vec3<T>::epsilon) || (bl<=vec3<T>::epsilon) || (cl<=vec3<T>::epsilon))
    throw EZeroDivisionError("mat4.decompose(): divide by zero");

  scale.set(al, bl, cl);

  // Normalize a,b,c
  a /= al;
  b /= bl;
  c /= cl;

  a.get(rot.m11, rot.m21, rot.m31);
  b.get(rot.m12, rot.m22, rot.m32);
  c.get(rot.m13, rot.m23, rot.m33);
  if (rot.determinant()<0)
  {
    rot.m11 = -rot.m11;
    rot.m21 = -rot.m21;
    rot.m31 = -rot.m31;
    scale.x = -scale.x;
  }

  t.set(m14, m24, m34);
}


typedef mat4<double> mat4d;
typedef mat4<float> mat4f;

} // end of namespace

#endif
