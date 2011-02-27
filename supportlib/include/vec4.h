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

#ifndef VEC4_H
#define VEC4_H

/**
   \file vec4.h

   Contains the declaration of vec4, a vector with 4 components of type T.
 */

#include <iostream>
#include <algorithm>
#include "common_exceptions.h"
#include "vec3.h"

namespace support3d {

/** 
   Vector with 4 components of type T.

   vec4 is a template vector (x,y,z,w) with 4 components of type T. Its main 
   purpose is to be used with floating point types but it can also
   be used with integers.

   Math operations:

   \verbatim

   Negation        vector  = -vector
   Addition        vector  = vector+vector
                   vector += vector
                   vector.add(vector,vector)
   Subtraction     vector  = vector-vector
                   vector -= vector
                   vector.sub(vector,vector)
   Multiplication  vector  = scalar*vector 
                   vector  = vector*scalar
                   vector *= scalar
                   vector.mul(vector,scalar)
                   vector  = vector ^ vector       (= cross product)
                   scalar  = vector*vector         (= dot product)
   Division        vector  = vector/scalar         (can throw an exception!)
                   vector /= scalar                (can throw an exception!)
   Length          scalar = vector.length()
   Normalize       vector = vector.normalize()     (can throw an exception!)
                   vector.normalize(destination)   (can throw an exception!)
   Ortho           vector = vector.ortho()

   Angle           angle = angle(a,b)              (can throw an exception!)
                   angle = sangle(a,b,axis)        (can throw an exception!)

   Comparison      vector == vector
                   vector != vector
                   vector <  vector
                   vector <= vector
                   vector >  vector
                   vector >= vector 
                   vector.nullvec()
   \endverbatim

   @author Matthias Baas
 */
template<class T> 
class vec4
{
  public:
  /// x component.
  T x;
  /// y component.
  T y;
  /// z component.
  T z;
  /// w component.
  T w;
  //  static char leftBracket;  // Linke Klammer für Ein-/Ausgabe (Default '<')
  //  static char rightBracket; // Rechte Klammer (Default '>')
  //  static char separator;    // Trennzeichen für Komponenten (Default ',')
  //  static T epsilon;         // Schranke für Vergleiche

  public:
  // Constructors
  vec4() : x(0), y(0), z(0), w(0) {}
  explicit vec4(T a) : x(a), y(a), z(a), w(a) {}
  vec4(T ax, T ay) : x(ax),y(ay),z(0.0),w(0.0) {}
  vec4(T ax, T ay, T az) : x(ax),y(ay),z(az),w(0.0) {}
  vec4(T ax, T ay, T az, T aw) : x(ax),y(ay),z(az),w(aw) {}
  vec4(const vec4<T>& v) : x(v.x), y(v.y), z(v.z), w(v.w) {}
  // Use defaults for copy constructor and assignment

  // Conversion operators 
  // (vec4<float> and vec4<double> can now be mixed)
  operator vec4<float>() const;
  operator vec4<double>() const;

  vec4<T>& set(T ax, T ay, T az, T aw);
  void get(T& ax, T& ay, T& az, T& aw) const;

  T& operator[](int);

  vec4<T>& operator+=(const vec4<T>& v);
  vec4<T>& operator-=(const vec4<T>& v);
  vec4<T>& operator*=(const T r);
  vec4<T>& operator/=(const T r);
  vec4<T>& operator%=(const T r);
  vec4<T>& operator%=(const vec4<T>& b);

  vec4<T> operator+(const vec4<T>& b) const; // vec = vec + vec
  vec4<T> operator-(const vec4<T>& b) const; // vec = vec - vec
  vec4<T> operator-() const;                 // vec = -vec
  vec4<T> operator*(const T r) const;        // vec = vec * scalar
  T operator*(const vec4<T>& b) const;       // scalar = vec * vec (dot prod.)
  vec4<T> operator/(const T r) const;        // vec = vec / scalar
  vec4<T> operator%(const T b) const;        // vec = vec % scalar (each component)
  vec4<T> operator%(const vec4<T>& b) const; // vec = vec % vec

  bool operator==(const vec4<T>& b) const;
  bool operator!=(const vec4<T>& b) const;
  bool operator<(const vec4<T>& b) const;
  bool operator>(const vec4<T>& b) const;
  bool operator<=(const vec4<T>& b) const;
  bool operator>=(const vec4<T>& b) const;
  bool nullvec() const;                      // *this == 0

  T length() const;

  vec4<T>  normalize() const;
  vec4<T>& normalize(vec4<T>& dest);

  // Return component/index with maximum value
  T max() const;
  int maxIndex() const;
  // Return component/index with minimum value
  T min() const;
  int minIndex() const;
  // Return component/index with maximum absolute value
  T maxAbs() const;
  int maxAbsIndex() const;
  // Return component/index with minimum absolute value
  T minAbs() const;
  int minAbsIndex() const;

 //char* to_str(char *s) const;

  //////////////////// Alternatives to operators /////////////////////

  vec4<T>& add(const vec4<T>& a, const vec4<T>& b);   // this = a+b
  vec4<T>& sub(const vec4<T>& a, const vec4<T>& b);   // this = a-b
  vec4<T>& neg(const vec4<T>& a);                     // this = -a 
  vec4<T>& mul(const vec4<T>& a, const T r);          // this = a*r
  vec4<T>& div(const vec4<T>& a, const T r);          // this = a/r
};


/// Output operator for vectors.
template<class T> 
std::ostream& operator<<(std::ostream& s, const vec4<T>& a)
{
//  s<<vec3<T>::leftBracket<<a.x<<vec3<T>::separator<<" "<<a.y<<vec3<T>::separator<<" "<<a.z<<vec3<T>::separator<<" "<<a.w<<vec3<T>::rightBracket;
  s<<"("<<a.x<<", "<<a.y<<", "<<a.z<<", "<<a.w<<")";
  return s;
}

///////////////////// Method definitions ///////////////////

// Scalar multiplication from left: scalar*vec
template<class T> 
inline vec4<T> operator*(const T r,const vec4<T>& v) 
{
  return vec4<T>(r*v.x, r*v.y, r*v.z, r*v.w);
}

// Spezialisierungen für float und double (das double auch bei float
// definiert ist und umgekehrt).
inline vec4<float> operator*(const double r, const vec4<float>& v)
{
  return vec4<float>(float(r*v.x), float(r*v.y), float(r*v.z), float(r*v.w));
}
inline vec4<double> operator*(const float r, const vec4<double>& v)
{
  return vec4<float>(float(r*v.x), float(r*v.y), float(r*v.z), float(r*v.w));
}


/*-------------------------Conversion operator--------------------------*//**
   Convert to vec4<float>.
*//*------------------------------------------------------------------------*/
template<class T>
inline vec4<T>::operator vec4<float>() const
{
  return vec4<float>(x,y,z,w);
}

/*-------------------------Conversion operator--------------------------*//**
   Convert to vec4<double>.
*//*------------------------------------------------------------------------*/
template<class T>
inline vec4<T>::operator vec4<double>() const
{
  return vec4<double>(x,y,z,w);
}


/*--------------------------------set-----------------------------------*//**
   Set new components.

  \verbatim

       [ ax ]
   v = [ ay ]
       [ az ] 
  \endverbatim

  @return  Reference to this.
*//*------------------------------------------------------------------------*/
template<class T> 
inline vec4<T>& vec4<T>::set(T ax, T ay, T az, T aw) 
{ 
  x=ax; 
  y=ay; 
  z=az; 
  w=aw;
  return *this;
}


/*--------------------------------get-----------------------------------*//**
  Return components.

  @param ax  X component (output).
  @param ay  Y component (output).
  @param az  Z component (output).
  @param aw  W component (output).
*//*------------------------------------------------------------------------*/
template<class T>
inline void vec4<T>::get(T& ax, T& ay, T& az, T& aw) const
{
  ax = x;
  ay = y;
  az = z;
  aw = w;
}


/*-----------------------------operator[]-------------------------------*//**
  Index operator.

  Return component i. The index is checked.

  @param     index  Component index (0,1,2,3).
  @return    Component i.
  @exception EIndexError  Index is out of range.
*//*------------------------------------------------------------------------*/
template<class T>
T& vec4<T>::operator[](int index)
{
  switch (index)
  {
   case 0: return x;
   case 1: return y;
   case 2: return z;
   case 3: return w;
   default: throw EIndexError();
  }
}

/*-----------------------------operator+=-------------------------------*//**
  vector += vector.

  @return    Reference to this.
*//*------------------------------------------------------------------------*/
template<class T>
inline vec4<T>& vec4<T>::operator+=(const vec4<T>& v)
{ 
  x+=v.x; 
  y+=v.y; 
  z+=v.z;
  w+=v.w;
  return *this; 
}

/*-----------------------------operator-=-------------------------------*//**
  vector -= vector.

  @return    Reference to this.
*//*------------------------------------------------------------------------*/
template<class T>
inline vec4<T>& vec4<T>::operator-=(const vec4<T>& v) 
{ 
  x-=v.x; 
  y-=v.y; 
  z-=v.z; 
  w-=v.w;
  return *this; 
}

/*-----------------------------operator*=-------------------------------*//**
  vector *= scalar.

  @return    Reference to this.
*//*------------------------------------------------------------------------*/
template<class T>
inline vec4<T>& vec4<T>::operator*=(const T r)
{
  x*=r; 
  y*=r; 
  z*=r; 
  w*=r; 
  return *this; 
}

/*-----------------------------operator/=-------------------------------*//**
  vector /= scalar.

  @return    Reference to this.
  @exception EZeroDivisionError  Is returned when r=0.
*//*------------------------------------------------------------------------*/
template<class T>
inline vec4<T>& vec4<T>::operator/=(const T r)
{
  // r=0 ? then throw Exception
  if ( (r > vec3<T>::epsilon) || (r < -vec3<T>::epsilon) ) 
  {
    x/=r; y/=r; z/=r; w/=r;
  }
  else
  {
    throw EZeroDivisionError("vec4: divide by zero");
  }
  return *this;
}

/*-----------------------------operator%=-------------------------------*//**
  vector %= scalar.

  @return    Reference to this.
  @exception EZeroDivisionError  Is thrown when r=0.
*//*------------------------------------------------------------------------*/
template<class T>
inline vec4<T>& vec4<T>::operator%=(const T r)
{
  // r=0 ? then throw Exception
  if ( (r > vec3<T>::epsilon) || (r < -vec3<T>::epsilon) ) 
  {
    x = _fmod(x,r); 
    y = _fmod(y,r);
    z = _fmod(z,r);
    w = _fmod(w,r);
  }
  else
  {
    throw EZeroDivisionError("vec4: division by zero");
  }
  return *this;
}

/*-----------------------------operator%=-------------------------------*//**
  vector %= vector

  @return    Reference to this.
  @exception EZeroDivisionError  Is thrown when a component of b=0.
*//*------------------------------------------------------------------------*/
template<class T>
inline vec4<T>& vec4<T>::operator%=(const vec4<T>& b)
{
  if ( ((b.x > vec3<T>::epsilon) || (b.x < -vec3<T>::epsilon)) &&
       ((b.y > vec3<T>::epsilon) || (b.y < -vec3<T>::epsilon)) &&
       ((b.z > vec3<T>::epsilon) || (b.z < -vec3<T>::epsilon)) &&
       ((b.w > vec3<T>::epsilon) || (b.w < -vec3<T>::epsilon)) )
  {
    x = _fmod(x,b.x); 
    y = _fmod(y,b.y);
    z = _fmod(z,b.z);
    w = _fmod(w,b.w);
  }
  else
  {
    throw EZeroDivisionError("vec4: division by zero");
  }
  return *this;
}


/*-----------------------------operator+ -------------------------------*//**
  vector = vector+vector

  @return   Sum of two vectors.
*//*------------------------------------------------------------------------*/
template<class T> 
inline vec4<T> vec4<T>::operator+(const vec4<T>& b) const
{ 
  return vec4<T>(x+b.x, y+b.y, z+b.z, w+b.w);
}

/*-----------------------------operator- -------------------------------*//**
  vector = vector-vector

  @return   Difference of two vectors.
*//*------------------------------------------------------------------------*/
template<class T>
inline vec4<T> vec4<T>::operator-(const vec4<T>& b) const 
{ 
  return vec4<T>(x-b.x, y-b.y, z-b.z, w-b.w);
}

/*-----------------------------operator- -------------------------------*//**
  vector = -vector

  @return   Negated vector.
*//*------------------------------------------------------------------------*/
template<class T>
inline vec4<T> vec4<T>::operator-() const
{ 
  return vec4<T>(-x, -y, -z, -w); 
}

/*-----------------------------operator* -------------------------------*//**
  vector = vector*scalar

  @return   Product of a vector and a scalar.
*//*------------------------------------------------------------------------*/
template<class T>
inline vec4<T> vec4<T>::operator*(const T r) const
{ 
  return vec4<T>(r*x, r*y, r*z, r*w); 
}

/*-----------------------------operator* -------------------------------*//**
  scalar = vector*vector (dot product).

  @return   Dot product of two vectors.
*//*------------------------------------------------------------------------*/
template<class T>
inline T vec4<T>::operator*(const vec4<T>& b) const
{ 
  return (x*b.x + y*b.y + z*b.z + w*b.w); 
}

/*-----------------------------operator/ -------------------------------*//**
  vector = vector / scalar.

  @return    Vector divided by scalar (component wise).
  @exception EZeroDivisionError  Is returned when r=0.
*//*------------------------------------------------------------------------*/
template<class T>
inline vec4<T> vec4<T>::operator/(const T r) const
{
  if (xabs(r)<=vec3<T>::epsilon) 
  { 
    throw EZeroDivisionError("vec4: divide by zero");
  }
  return vec4<T>(x/r, y/r, z/r, w/r);
}

/*-----------------------------operator% -------------------------------*//**
  vector = vector % scalar.

  @return    Vector modulo scalar (component wise).
  @exception EZeroDivisionError  Is returned when r=0.
*//*------------------------------------------------------------------------*/
template<class T>
inline vec4<T> vec4<T>::operator%(const T r) const
{
  if (xabs(r)<=vec3<T>::epsilon) 
  { 
    throw EZeroDivisionError("vec4: divide by zero");
  }
  return vec4<T>(_fmod(x,r), _fmod(y,r), _fmod(z,r), _fmod(w,r));
}

/*-----------------------------operator% -------------------------------*//**
  vector = vector % vector

  @return    Vector modulo vector (component wise).
  @exception EZeroDivisionError  Is returned when component of b=0.
*//*------------------------------------------------------------------------*/
template<class T>
inline vec4<T> vec4<T>::operator%(const vec4<T>& b) const
{
  if ((xabs(b.x)<=vec3<T>::epsilon) || (xabs(b.y)<=vec3<T>::epsilon) || 
      (xabs(b.z)<=vec3<T>::epsilon) || (xabs(b.w)<=vec3<T>::epsilon))
  { 
    throw EZeroDivisionError("vec4: divide by zero");
  }
  return vec4<T>(_fmod(x,b.x), _fmod(y,b.y), _fmod(z,b.z), _fmod(w,b.w));
}

/*-----------------------------nullvec----------------------------------*//**
  Check if vector = 0.

  @return    True, if each component is 0.
*//*------------------------------------------------------------------------*/
template<class T>
inline bool vec4<T>::nullvec() const
{
  return (xabs(x)<=vec3<T>::epsilon) &&
         (xabs(y)<=vec3<T>::epsilon) && 
         (xabs(z)<=vec3<T>::epsilon) &&
         (xabs(w)<=vec3<T>::epsilon);
}

/*-----------------------------operator==-------------------------------*//**
  vector == vector

  @return    True if both vectors are equal.
*//*------------------------------------------------------------------------*/
template<class T>
inline bool vec4<T>::operator==(const vec4<T>& b) const
{  
  return (xabs(x-b.x)<=vec3<T>::epsilon) &&
         (xabs(y-b.y)<=vec3<T>::epsilon) &&
         (xabs(z-b.z)<=vec3<T>::epsilon) &&
         (xabs(w-b.w)<=vec3<T>::epsilon);
}

/*-----------------------------operator!=-------------------------------*//**
  vector != vector

  @return    True if both vectors are not equal.
*//*------------------------------------------------------------------------*/
template<class T>
inline bool vec4<T>::operator!=(const vec4<T>& b) const
{
  return !(*this==b);
}

/*-----------------------------operator< -------------------------------*//**
  vector < vector

  For this test the epsilon threshold is \b not used!

  @return   True if each component of the first vector is less than the
            according component of the second vector.
*//*------------------------------------------------------------------------*/
template<class T>
inline bool vec4<T>::operator<(const vec4<T>& b) const
{  
  return (x<b.x) && (y<b.y) && (z<b.z) && (w<b.w);
}

/*-----------------------------operator> -------------------------------*//**
  vector > vector

  For this test the epsilon threshold is \b not used!

  @return   True if each component of the first vector is greater than the
            according component of the second vector.
*//*------------------------------------------------------------------------*/
template<class T>
inline bool vec4<T>::operator>(const vec4<T>& b) const
{  
  return (x>b.x) && (y>b.y) && (z>b.z) && (w>b.w);
}

/*-----------------------------operator<=-------------------------------*//**
  vector <= vector

  For this test the epsilon threshold is \b not used!

  @return   True if each component of the first vector is less than or 
            equal the according component of the second vector.
*//*------------------------------------------------------------------------*/
template<class T>
inline bool vec4<T>::operator<=(const vec4<T>& b) const
{  
  return (x<=b.x) && (y<=b.y) && (z<=b.z) && (w<=b.w);
}

/*-----------------------------operator>=-------------------------------*//**
  vector >= vector

  For this test the epsilon threshold is \b not used!

  @return   True if each component of the first vector is greater than or 
            equal the according component of the second vector.
*//*------------------------------------------------------------------------*/
template<class T>
inline bool vec4<T>::operator>=(const vec4<T>& b) const
{  
  return (x>=b.x) && (y>=b.y) && (z>=b.z) && (w>=b.w);
}

/*------------------------------length----------------------------------*//**
  Return length of the vector.

  @return   Length of vector.
*//*------------------------------------------------------------------------*/
template<class T>
inline T vec4<T>::length() const 
{ 
  return sqrt(x*x + y*y + z*z + w*w); 
}


/*-----------------------------normalize-------------------------------*//**
  Normalize vector.

  Normalize *this and store result in dest. *this remains unchanged.
  It is allowed to pass *this as dest.
  If *this is 0 then an exception is thrown (DivideByZero).

  @param     dest  The normalized vector is stored in dest.
  @return    Reference to dest.
  @exception EZeroDivisionError  Is thrown if the length of the vector is zero.
*//*------------------------------------------------------------------------*/
template<class T>
inline vec4<T>& vec4<T>::normalize(vec4<T>& dest)
{
  T vl=length();     // vl: Länge des (eigenen) Vektors

  // vl=0? dann Exception
  if (vl<=vec3<T>::epsilon) throw EZeroDivisionError("vec4.normalize(): divide by zero");

  vl = 1 / vl;
  dest.x = vl*x;
  dest.y = vl*y;
  dest.z = vl*z;
  dest.w = vl*w;
  return dest;
}

/*-----------------------------normalize-------------------------------*//**
  Normalize vector.

  See normalize.

  @return    Normalized vector.
  @exception EZeroDivisionError  Is thrown if the length of the vector is zero.
*//*------------------------------------------------------------------------*/
template<class T>
inline vec4<T> vec4<T>::normalize() const
{
  T vl=length();     // vl: Länge des (eigenen) Vektors

  // vl=0? dann Exception
  if (vl<=vec3<T>::epsilon) throw EZeroDivisionError("vec4.normalize(): divide by zero");

  vl = 1 / vl;
  return vec4<T>(vl*x, vl*y, vl*z, vl*w);
}


/*--------------------------------max----------------------------------*//**
  Return component with maximum value.

  @return    Maximum value.
*//*------------------------------------------------------------------------*/
template<class T> 
inline T vec4<T>::max() const
{
  return std::max(std::max(std::max(x,y),z),w);
}

/*--------------------------------maxIndex----------------------------*//**
  Return index of component with maximum value.

  @return   Index (0,1,2 or 3).
*//*------------------------------------------------------------------------*/
template<class T> 
inline int vec4<T>::maxIndex() const
{
  // Is x maximum?
  if ((x>=y) && (x>=z) && (x>=w))
  {
    return 0;
  }
  else
  {
    // Is y maximum?
    if ((y>=z) && (y>=w))
      return 1; 
    else 
    {
      if (z>=w) return 2; else return 3;
    }
  }
}

/*--------------------------------min----------------------------------*//**
  Return component with minimum value.

  @return    Minimum value.
*//*------------------------------------------------------------------------*/
template<class T> 
inline T vec4<T>::min() const
{
  return std::min(std::min(std::min(x,y),z),w);
}

/*--------------------------------minIndex----------------------------*//**
  Return index of component with minimum value.

  @return   Index (0,1,2 or 3).
*//*------------------------------------------------------------------------*/
template<class T> 
inline int vec4<T>::minIndex() const
{
  // Is x minimum?
  if ((x<=y) && (x<=z) && (x<=w))
  {
    return 0;
  }
  else
  {
    if ((y<=z) && (y<=w)) 
      return 1; 
    else 
    {
      if (z<=w)
	return 2;
      else
	return 3;
    }
  }
}

/*-------------------------------maxAbs--------------------------------*//**
  Return component with maximum absolute value.

  @return  Maximum absolute value.
*//*------------------------------------------------------------------------*/
template<class T> 
inline T vec4<T>::maxAbs() const
{
  return std::max(std::max(std::max(xabs(x),xabs(y)),xabs(z)),xabs(w));
}

/*-----------------------------maxAbsIndex-----------------------------*//**
  Return index of component with maximum absolute value.

  @return    Index (0,1,2 or 3).
*//*------------------------------------------------------------------------*/
template<class T> 
inline int vec4<T>::maxAbsIndex() const
{
  T ax = xabs(x);
  T ay = xabs(y);
  T az = xabs(z);
  T aw = xabs(w);

  // Is ax maximum?
  if ((ax>=ay) && (ax>=az) && (ax>=aw))
  {
    return 0;
  }
  else
  {
    // Is ay maximum?
    if ((ay>=az) && (ay>=aw))
      return 1; 
    else 
    {
      if (az>=aw)
	return 2;
      else
	return 3;
    }
  }
}

/*-------------------------------minAbs--------------------------------*//**
  Return component with minimum absolute value.

  @return    Minimum absolute value.
*//*------------------------------------------------------------------------*/
template<class T> 
inline T vec4<T>::minAbs() const
{
  return std::min(std::min(std::min(xabs(x),xabs(y)),xabs(z)),xabs(w));
}

/*-----------------------------minAbsIndex-----------------------------*//**
  Return index of component with minimum absolute value.

  @return    Index (0,1,2 or 3).
*//*------------------------------------------------------------------------*/
template<class T> 
inline int vec4<T>::minAbsIndex() const
{
  T ax = xabs(x);
  T ay = xabs(y);
  T az = xabs(z);
  T aw = xabs(w);

  // Is ax minimum?
  if ((ax<=ay) && (ax<=az) && (ax<=aw))
  {
    return 0;
  }
  else
  {
    // Is ay minimum?
    if ((ay<=az) && (ay<=aw))
      return 1; 
    else 
    {
      if (az<=aw)
	return 2;
      else
	return 3;
    }
  }
}


/*---------------------------------add----------------------------------*//**
  this = a+b

  If speed is critical you can use this function instead of the
  + operator. This way you save one copy operation.

  @return   Reference to this.
*//*------------------------------------------------------------------------*/
template<class T>
inline vec4<T>& vec4<T>::add(const vec4<T>& a, const vec4<T>& b)
{ 
  x=a.x+b.x; 
  y=a.y+b.y; 
  z=a.z+b.z; 
  w=a.w+b.w; 
  return *this; 
}

/*---------------------------------sub---------------------------------*//**
  this = a-b

  If speed is critical you can use this function instead of the
  - operator. This way you save one copy operation.

  @return   Reference to this.
*//*------------------------------------------------------------------------*/
template<class T>
inline vec4<T>& vec4<T>::sub(const vec4<T>& a, const vec4<T>& b)
{ 
  x=a.x-b.x; 
  y=a.y-b.y; 
  z=a.z-b.z; 
  w=a.w-b.w; 
  return *this; 
}

/*---------------------------------neg---------------------------------*//**
  this = -a

  If speed is critical you can use this function instead of the
  - operator. This way you save one copy operation.

  @return   Reference to this.
*//*------------------------------------------------------------------------*/
template<class T>
inline vec4<T>& vec4<T>::neg(const vec4<T>& a)
{ 
  x=-a.x; 
  y=-a.y; 
  z=-a.z; 
  w=-a.w; 
  return *this; 
}

/*--------------------------------mul----------------------------------*//**
  this = a * r

  If speed is critical you can use this function instead of the
  * operator. This way you save one copy operation.

  @return   Reference to this.
*//*------------------------------------------------------------------------*/
template<class T>
inline vec4<T>& vec4<T>::mul(const vec4<T>& a, const T r)
{ 
  x=a.x*r;
  y=a.y*r;
  z=a.z*r;
  w=a.w*r;
  return *this;
}

/*--------------------------------div----------------------------------*//**
  this = a / r

  If speed is critical you can use this function instead of the
  / operator. This way you save one copy operation.

  @return   Reference to this.
*//*------------------------------------------------------------------------*/
template<class T>
inline vec4<T>& vec4<T>::div(const vec4<T>& a, const T r)
{ 
  x=a.x/r;
  y=a.y/r;
  z=a.z/r;
  w=a.w/r;
  return *this;
}


typedef vec4<double> vec4d;
typedef vec4<float> vec4f;


} // end of namespace

#endif
