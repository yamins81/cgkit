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

#ifndef QUAT_H
#define QUAT_H

/**
   \file quat.h

   Contains the declaration of the quaternion class.
 */

#include <iostream>
#include "vec3.h"
#include "mat3.h"
#include "mat4.h"
#include "common_exceptions.h"
#include "math.h"

namespace support3d {

/** 
   Quaternion class with components of type T.
 */
template<class T> 
class quat
{
  public:
  /// w component
  T w;
  /// x component
  T x;
  /// y component
  T y;
  /// z component
  T z;

  public:
  // Constructors
  quat() : w(0), x(0), y(0), z(0) {}
  explicit quat(T a) : w(a), x(0), y(0), z(0) {}
  quat(T aw, T ax, T ay, T az) : w(aw), x(ax), y(ay), z(az) {}
  quat(const quat<T>& q) : w(q.w), x(q.x), y(q.y), z(q.z) {}

  quat<T>& set(T aw, T ax, T ay, T az);
  void get(T& aw, T& ax, T& ay, T& az) const;

  quat<T>& operator+=(const quat<T>& q) { w+=q.w; x+=q.x; y+=q.y; z+=q.z; return *this; }
  quat<T>& operator-=(const quat<T>& q) { w-=q.w; x-=q.x; y-=q.y; z-=q.z; return *this; };
  quat<T>& operator*=(const T r) { w*=r; x*=r; y*=r; z*=r; return *this; };
  quat<T>& operator/=(const T r);

  quat<T> operator+(const quat<T>& b) const { return quat<T>(w+b.w, x+b.x, y+b.y, z+b.z); }
  quat<T> operator-(const quat<T>& b) const { return quat<T>(w-b.w, x-b.x, y-b.y, z-b.z); }
  quat<T> operator-() const { return quat<T>(-w, -x, -y, -z); }
  quat<T> operator*(const T r) const { return quat<T>(r*w, r*x, r*y, r*z); }
  quat<T> operator*(const quat<T>& b) const { return quat<T>(w*b.w - x*b.x - y*b.y - z*b.z, 
                                                             w*b.x + x*b.w + y*b.z - z*b.y,
                                                             w*b.y + y*b.w - x*b.z + z*b.x,
                                                             w*b.z + z*b.w + x*b.y - y*b.x); }
  quat<T> operator/(const T r) const;

  bool operator==(const quat<T>& q) const;
  bool operator!=(const quat<T>& q) const;

  T abs() const { return sqrt(w*w+x*x+y*y+z*z); }

  quat<T>  normalize() const;
  quat<T>& normalize(quat<T>& dest);

  T dot(const quat<T>& q) const { return w*q.w+x*q.x+y*q.y+z*q.z; }

  quat<T>  conjugate() const { return quat<T>(w, -x, -y, -z); }
  quat<T>& conjugate(quat<T>& dest) { dest.w=w; dest.x=-x; dest.y=-y; dest.z=-z; return dest; }

  quat<T>  inverse() const;
  quat<T>& inverse(quat<T>& dest);

  quat<T>& fromMat(const mat3<T>& m);
  quat<T>& fromMat(const mat4<T>& m);

  void toMat3(mat3<T>& mat) const;
  void toMat4(mat4<T>& mat) const;
  mat3<T> toMat3() const;
  mat4<T> toMat4() const;

  void toAngleAxis(T& angle, vec3<T>& axis) const;
  quat<T>& fromAngleAxis(T angle, const vec3<T>& axis);

  void log(quat<T>& q);
  quat<T> log() const;

  void exp(quat<T>& q);
  quat<T> exp() const;

  void rotateVec(const vec3<T>& v, vec3<T>& dest) const;
  vec3<T> rotateVec(const vec3<T>& v) const;

  //////////////////// Alternatives to operators /////////////////////

  quat<T>& add(const quat<T>& a, const quat<T>& b) { w=a.w+b.w; x=a.x+b.x; y=a.y+b.y; z=a.z+b.z; return *this; }
  quat<T>& sub(const quat<T>& a, const quat<T>& b) { w=a.w-b.w; x=a.x-b.x; y=a.y-b.y; z=a.z-b.z; return *this; }
  quat<T>& neg(const quat<T>& a) { w=-a.w; x=-a.x; y=-a.y; z=-a.z; return *this; }
  quat<T>& mul(const T r, const quat<T>& a) { w=r*a.w; x=r*a.x; y=r*a.y; z=r*a.z; return *this; }
  quat<T>& mul(const quat<T>& a, const quat<T>& b) { w = a.w*b.w - a.x*b.x - a.y*b.y - a.z*b.z;
                                                     x = a.w*b.x + a.x*b.w + a.y*b.z - a.z*b.y;
                                                     y = a.w*b.y + a.y*b.w - a.x*b.z + a.z*b.x;
                                                     z = a.w*b.z + a.z*b.w + a.x*b.y - a.y*b.x;
                                                     return *this; }
};


/// Output operator for quats.
template<class T> 
std::ostream& operator<<(std::ostream& s, const quat<T>& q)
{
//  s<<vec3<T>::leftBracket<<q.w<<vec3<T>::separator<<" "<<q.x<<vec3<T>::separator<<" "<<q.y<<vec3<T>::separator<<" "<<q.z<<vec3<T>::rightBracket;
  s<<"("<<q.w<<", "<<q.x<<", "<<q.y<<", "<<q.z<<")";
  return s;
}

// Scalar multiplication from left: scalar*quat
template<class T> 
inline quat<T> operator*(const T r,const quat<T>& q) 
{
  return quat<T>(r*q.w, r*q.x, r*q.y, r*q.z);
}

/**
  Spherical linear interpolation between two quaternions.

  The return value is an interpolation between q0 and q1. For t=0.0
  the return value equals q0, for t=1.0 it equals q1.
  q0 and q1 must be unit quaternions.

  \param t Interpolation value
  \param q0 First quaternion (t=0.0)
  \param q1 Second quaternion (t=1.0)
  \param shortest Always interpolate along the shortest path (negating q0 or q1 will lead to the same interpolation)
  \return Interpolated quaternion
 */

template<class T>
quat<T> slerp(T t, const quat<T>& q0, const quat<T>& q1, bool shortest=true)
{
  T o,so,a,b;
  bool neg_q1 = false;   // Does q1 have to be negated (so that the shortest path is taken)?

  T ca = q0.dot(q1);
  if (shortest && ca<0)
  {
    ca = -ca;
    neg_q1 = true;
  }
  o = acos(ca);
  so = sin(o);

  if (xabs(so)<vec3<T>::epsilon)
    return q0;

  a = sin(o*(1.0-t)) / so;
  b = sin(o*t) / so;
  if (neg_q1)
    return q0*a - q1*b;
  else
    return q0*a + q1*b;
}

/**
  Spherical cubic interpolation.
 */
template<class T>
quat<T> squad(T t, const quat<T>& a, const quat<T>& b, const quat<T>& c, const quat<T>& d)
{
  return slerp(2*t*(1.0-t), slerp(t,a,d), slerp(t,b,c));
}


///////////////////////////////////////////////////////////////////////7

/**
  Set the individual components of the quaternion.

  \param aw w component
  \param ax x component
  \param ay y component
  \param az z component
  \return A reference to this.
 */
template<class T> 
quat<T>& quat<T>::set(T aw, T ax, T ay, T az) 
{ 
  w = aw; 
  x = ax; 
  y = ay; 
  z = az; 
  return *this;
}

/**
  Get the individual components of the quaternion.

  Note: You can also access the components via the public 
  attributes w, x, y, z.

  \param[out] aw Receives the w component
  \param[out] ax Receives the x component
  \param[out] ay Receives the y component
  \param[out] az Receives the z component
*/
template<class T> 
void quat<T>::get(T& aw, T& ax, T& ay, T& az) const
{ 
  aw = w; 
  ax = x; 
  ay = y; 
  az = z; 
}

/**
  /= operator

  \return    Reference to this.
  \exception EZeroDivisionError  Is returned when r=0.
*/
template<class T>
inline quat<T>& quat<T>::operator/=(const T r)
{
  // r=0 ? then throw Exception
  if ( (r > vec3<T>::epsilon) || (r < -vec3<T>::epsilon) ) 
  {
    w/=r; x/=r; y/=r; z/=r;
  }
  else
  {
    throw EZeroDivisionError("quat: divide by zero");
  }
  return *this;
}

/**
  quat = quat / scalar

  \return Quat divided by scalar 
  \exception EZeroDivisionError  Is returned when r=0.
*/
template<class T>
inline quat<T> quat<T>::operator/(const T r) const
{
  if (xabs(r)<=vec3<T>::epsilon) 
  { 
    throw EZeroDivisionError("quat: divide by zero");
  }
  return quat<T>(w/r, x/r, y/r, z/r);
}

/**
  == operator

  \return True if both quats are equal.
*/
template<class T>
inline bool quat<T>::operator==(const quat<T>& q) const
{  
  return (xabs(x-q.x)<=vec3<T>::epsilon) &&
         (xabs(y-q.y)<=vec3<T>::epsilon) &&
         (xabs(z-q.z)<=vec3<T>::epsilon) &&
         (xabs(w-q.w)<=vec3<T>::epsilon);
}

/**
  != operator

  \return True if both quats are not equal.
*//*------------------------------------------------------------------------*/
template<class T>
inline bool quat<T>::operator!=(const quat<T>& q) const
{
  return !(*this==q);
}



/**
  Normalize the quaternion.

  If the quaternion can't be normalized a EDivideByZero exception is thrown.

  \return Normalized quaternion.
 */
template<class T>
quat<T> quat<T>::normalize() const
{
  T len = abs();
  if (len<=vec3<T>::epsilon)
    throw EZeroDivisionError("quat: divide by zero");

  return (*this)*(1.0/len);
}

/**
  Normalize the quaternion.

  If the quaternion can't be normalized a EDivideByZero exception is thrown.

  \param[out] dest Receives the result (may be *this)
  \return Reference to dest
 */
template<class T>
quat<T>& quat<T>::normalize(quat<T>& dest)
{
  T len = abs();
  if (len<=vec3<T>::epsilon)
    throw EZeroDivisionError("quat.normalize(): divide by zero");

  dest.mul(1.0/len, *this);
  return dest;
}

/**
  Invert the quaternion.

  If the quaternion can't be inverted a EDivideByZero exception is thrown.

  \return Inverse quaternion
 */
template<class T>
quat<T> quat<T>::inverse() const
{
  T len_2 = dot(*this);
  if (len_2<=vec3<T>::epsilon)
    throw EZeroDivisionError("quat is not invertible");

  len_2 = 1.0/len_2;
  return quat<T>(w*len_2, -x*len_2, -y*len_2, -z*len_2);
}

/**
  Invert the quaternion.

  If the quaternion can't be inverted a EDivideByZero exception is thrown.

  \param[out] dest Receives the result (may be *this)
  \return Reference to dest
 */
template<class T>
quat<T>& quat<T>::inverse(quat<T>& dest)
{
  T len_2 = dot(*this);
  if (len_2<=vec3<T>::epsilon)
    throw EZeroDivisionError("quat is not invertible");

  len_2 = 1.0/len_2;
  dest.w = w*len_2;
  dest.x = -x*len_2;
  dest.y = -y*len_2;
  dest.z = -z*len_2;
  return dest;
}


/**
  Initialize the quaternion from a mat3.

  \pre The matrix has to be a rotation matrix.
  \param m Rotation matrix.
  \return Reference to this
 */
template<class T>
quat<T>& quat<T>::fromMat(const mat3<T>& m)
{
  T m11, m12, m13;
  T m21, m22, m23;
  T m31, m32, m33;
  T ad1, ad2, ad3, t, s;

  m.getRow(0, m11, m12, m13);
  m.getRow(1, m21, m22, m23);
  m.getRow(2, m31, m32, m33);

  t = m11+m22+m33+1.0;
  if (t>vec3<T>::epsilon)
  {
    s = 0.5/sqrt(t);
    w = 0.25/s;
    x = (m32-m23)*s;
    y = (m13-m31)*s;
    z = (m21-m12)*s;
  }
  else
  {
    ad1 = m11;
    ad2 = m22;
    ad3 = m33;
    if (ad1>=ad2 && ad1>=ad3)
    {
      s = sqrt(1.0+m11-m22-m33)*2.0;
      x = 0.25*s;
      y = (m12+m21)/s;
      z = (m13+m31)/s;
      w = (m23+m32)/s;
    }
    else if (ad2>=ad1 && ad2>=ad3)
    {
      s = sqrt(1.0+m22-m11-m33)*2.0;
      x = (m12+m21)/s;
      y = 0.25*s;
      z = (m23+m32)/s;
      w = (m13+m31)/s;
    }
    else
    {
      s = sqrt(1.0+m33-m11-m22)*2.0;
      x = (m13+m31)/s;
      y = (m23+m32)/s;
      z = 0.25*s;
      w = (m12+m21)/s;
    }
  }
  return *this;
}

/**
  Return rotation matrix as a mat3.

  \param[out] mat  Rotation matrix
 */
template<class T>
void quat<T>::toMat3(mat3<T>& mat) const
{
  T xx = 2.0*x*x;
  T yy = 2.0*y*y;
  T zz = 2.0*z*z;
  T xy = 2.0*x*y;
  T zw = 2.0*z*w;
  T xz = 2.0*x*z;
  T yw = 2.0*y*w;
  T yz = 2.0*y*z;
  T xw = 2.0*x*w;

  mat.setRow(0, 1.0-yy-zz, xy-zw, xz+yw);
  mat.setRow(1, xy+zw, 1.0-xx-zz, yz-xw);
  mat.setRow(2, xz-yw, yz+xw, 1.0-xx-yy);
}

/**
  Return rotation matrix as a mat3.

  \return Rotation matrix
 */
template<class T>
mat3<T> quat<T>::toMat3() const
{
  mat3<T> res;
  toMat3(res);
  return res;
}

/**
  Return rotation matrix as a mat4.

  \param[out] mat  Rotation matrix
 */
template<class T>
void quat<T>::toMat4(mat4<T>& mat) const
{
  T xx = 2.0*x*x;
  T yy = 2.0*y*y;
  T zz = 2.0*z*z;
  T xy = 2.0*x*y;
  T zw = 2.0*z*w;
  T xz = 2.0*x*z;
  T yw = 2.0*y*w;
  T yz = 2.0*y*z;
  T xw = 2.0*x*w;

  mat.setRow(0, 1.0-yy-zz, xy-zw, xz+yw, 0);
  mat.setRow(1, xy+zw, 1.0-xx-zz, yz-xw, 0);
  mat.setRow(2, xz-yw, yz+xw, 1.0-xx-yy, 0);
  mat.setRow(3, 0,0,0,1);
}

/**
  Return rotation matrix as a mat4.

  \return  Rotation matrix
 */
template<class T>
mat4<T> quat<T>::toMat4() const
{
  mat4<T> res;
  toMat4(res);
  return res;
}

/**
  Initialize the quaternion from a mat4.

  The method only takes the mat3 part into account. Any translation is ignored.

  \pre The mat3 part of the matrix has to be a rotation matrix.
  \param m Rotation matrix.
  \return Reference to this
 */
template<class T>
quat<T>& quat<T>::fromMat(const mat4<T>& m)
{
  mat3<T> m3;
  m.getMat3(m3);
  return fromMat(m3);
}

/**
  Return angle (in radians) and rotation axis.

  \param[out] angle  Angle (in radians) around axis
  \param[out] axis  Rotation axis
 */
template<class T>
void quat<T>::toAngleAxis(T& angle, vec3<T>& axis) const
{
  quat<T> q = normalize();
  T s,w;

  // Clamp nself.w (since the quat has to be normalized it should
  // be between -1 and 1 anyway, but it might be slightly off due
  // to numerical inaccuracies)
  if (q.w<-1.0)
     w = -1.0;
  else 
  {
    if (q.w>1.0)
      w = 1.0;
    else
      w = q.w;
  }

  w = acos(w);
  s = sin(w);
  if (s<=vec3<T>::epsilon)
  {
    angle = 0;
    axis.set(0.0, 0.0, 0.0);
  }
  else
  {
    angle = 2*w;
    axis.set(q.x/s, q.y/s, q.z/s);
  }
}

/**
  Initialize from an angle (in radians) and an axis.

  This method always returns a normalized quaternion.

  \param angle  Angle (in radians) around axis
  \param axis  Rotation axis
  \return Reference to this
 */
template<class T>
quat<T>& quat<T>::fromAngleAxis(T angle, const vec3<T>& axis)
{
  T a = angle/2;
  T x = axis.x;
  T y = axis.y;
  T z = axis.z;
  T n = sqrt(x*x + y*y + z*z);

  // axis==0? (treat this the same as angle==0 with an arbitrary axis)
  if (n<=vec3<T>::epsilon)
  {
    this->w = 1.0;
    this->x = 0.0;
    this->y = 0.0;
    this->z = 0.0;
  }
  else
  {
    T s = sin(a)/n;

    this->w = cos(a);
    this->x = x*s;
    this->y = y*s;
    this->z = z*s;

    normalize(*this);
  }
  return *this;
}

/**
   Return the natural logarithm.

  \param[out] q Natural logarithm of this (may be *this)
 */
template<class T>
void quat<T>::log(quat<T>& q)
{
  T b = sqrt(x*x + y*y + z*z);

  if (xabs(b)<=vec3<T>::epsilon)
  {
    if (w<=vec3<T>::epsilon)
      throw EValueError("math domain error");

    q.set(::log(w), 0, 0, 0);
  }
  else
  {
    T t = atan2(b, w);
    T f = t/b;

    T ct = cos(t);
    if (xabs(ct)<=vec3<T>::epsilon)
      throw EValueError("math domain error");

    T r = w/ct;
    if (r<=vec3<T>::epsilon)
      throw EValueError("math domain error");

    q.set(::log(r), f*x, f*y, f*z);
  }
}

/**
   Return the natural logarithm.

  \return Natural logarithm of this
 */
template<class T>
quat<T> quat<T>::log() const
{
  quat<T> q;

  T b = sqrt(x*x + y*y + z*z);

  if (xabs(b)<=vec3<T>::epsilon)
  {
    if (w<=vec3<T>::epsilon)
      throw EValueError("math domain error");

    q.set(::log(w), 0, 0, 0);
  }
  else
  {
    T t = atan2(b, w);
    T f = t/b;

    T ct = cos(t);
    if (xabs(ct)<=vec3<T>::epsilon)
      throw EValueError("math domain error");

    T r = w/ct;
    if (r<=vec3<T>::epsilon)
      throw EValueError("math domain error");

    q.set(::log(r), f*x, f*y, f*z);
  }
  return q;
}

/**
   Return the exponential of this.

  \param[out] q Exponential of this (may be *this)
 */
template<class T>
void quat<T>::exp(quat<T>& q)
{
  T b = sqrt(x*x + y*y + z*z);

  if (xabs(b)<=vec3<T>::epsilon)
  {
    q.set(::exp(w), 0,0,0);
  }
  else
  {
    T f = sin(b)/b;
    q.set(::exp(w)*cos(b), f*x, f*y, f*z);
  }
}

/**
   Return the exponential of this.

  \return Exponential of this (may be *this)
 */
template<class T>
quat<T> quat<T>::exp() const
{
  quat<T> q;
  
  T b = sqrt(x*x + y*y + z*z);

  if (xabs(b)<=vec3<T>::epsilon)
  {
    q.set(::exp(w), 0,0,0);
  }
  else
  {
    T f = sin(b)/b;
    q.set(::exp(w)*cos(b), f*x, f*y, f*z);
  }
  return q;
}

/**
   Rotate a vector.

   This operation is equivalent to this*v*this->conjugate()
   (where v is turned into a quaternion).

   Note: If you have to do a lot of rotations with the same
   quaternion it will be more efficient to convert the quat
   into a matrix and do the rotation via matrix multiplication.

   \pre The quaternion must be a unit quaternion.
   \pre dest must not be the same reference as v!
   \param v The vector to rotate.
   \param[out] dest Rotate vector v
 */
template<class T>
void quat<T>::rotateVec(const vec3<T>& v, vec3<T>& dest) const
{
  T ww = w*w;
  T xx = x*x;
  T yy = y*y;
  T zz = z*z;
  T wx = w*x;
  T wy = w*y;
  T wz = w*z;
  T xy = x*y;
  T xz = x*z;
  T yz = y*z;

  dest.x = ww*v.x + xx*v.x - yy*v.x - zz*v.x + 2*((xy-wz)*v.y + (xz+wy)*v.z);
  dest.y = ww*v.y - xx*v.y + yy*v.y - zz*v.y + 2*((xy+wz)*v.x + (yz-wx)*v.z);
  dest.z = ww*v.z - xx*v.z - yy*v.z + zz*v.z + 2*((xz-wy)*v.x + (yz+wx)*v.y);
}

/**
   Rotate a vector.

   This operation is equivalent to this*v*this->conjugate()
   (where v is turned into a quaternion).

   Note: If you have to do a lot of rotations with the same
   quaternion it will be more efficient to convert the quat
   into a matrix and do the rotation via matrix multiplication.
   
   \pre The quaternion must be a unit quaternion.
   \param v The vector to rotate.
   \return Rotated vector v
 */
template<class T>
vec3<T> quat<T>::rotateVec(const vec3<T>& v) const
{
  T ww = w*w;
  T xx = x*x;
  T yy = y*y;
  T zz = z*z;
  T wx = w*x;
  T wy = w*y;
  T wz = w*z;
  T xy = x*y;
  T xz = x*z;
  T yz = y*z;

  return vec3<T>(ww*v.x + xx*v.x - yy*v.x - zz*v.x + 2*((xy-wz)*v.y + (xz+wy)*v.z),
		 ww*v.y - xx*v.y + yy*v.y - zz*v.y + 2*((xy+wz)*v.x + (yz-wx)*v.z),
		 ww*v.z - xx*v.z - yy*v.z + zz*v.z + 2*((xz-wy)*v.x + (yz+wx)*v.y));
}


typedef quat<double> quatd;
typedef quat<float> quatf;


} // end of namespace

#endif
