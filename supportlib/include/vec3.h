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

#ifndef VEC3_H
#define VEC3_H

/**
   \file vec3.h

   Contains the declaration of vec3, a vector with 3 components of type T.
 */


/*
 Operatoren:

 Negation                           c=-a                c.neg(a)
 Addition                           c=a+b  c+=a         c.add(a,b)
 Subtraktion                        c=a-b  c-=b         c.sub(a,b)
 Multiplikation (vector-scalar)     c=a*s  c=s*a  c*=s  c.mul(a,s)
 Division       (vector-scalar)     c=a/s  c/=s         c.div(a,s)
 Kreuzprodukt   (vector-vector)     c=a%b               c.cross(a,b)
 Skalarproduct  (vector-vector)     s=a*b
 Vergleich      (vector-vector)     a==b a!=b a<b a>b a<=b a>=b a.nullvec()
 Länge                              s=a.length()
 Normieren                          c=a.normalize()  a.normalize(c) 
 Ortho                              c=a.ortho()
 Winkel bestimmen                   s=angle(a,b)  s=sangle(a,b,axis)
 */

#include <iostream>
#include <algorithm>
#include <math.h>
#include "common_exceptions.h"

// Define the CGKIT_SHARED variable
/*#ifdef DLL_EXPORT_VEC3
  #include "shared_export.h"
#else
  #include "shared.h"
#endif*/

namespace support3d {

/// Returns absolute value of v.
template<class T> 
inline T xabs(const T v)
  { return (v<0)? -v : v; }

// _fmod: A modulo function that also works with negative a
inline double _fmod(double a, double b)
{
  int n = int(a/b);
  a = a-n*b;
  if (a<0)
    a=a+b;
  return a;
}


/** 
   Vector with 3 components of type T.

   vec3 is a template vector (x,y,z) with 3 components of type T. Its main 
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
class vec3
{
  public:
  /// x component.
  T x;
  /// y component.
  T y;
  /// z component.
  T z;
  static char leftBracket;  // Linke Klammer für Ein-/Ausgabe (Default '<')
  static char rightBracket; // Rechte Klammer (Default '>')
  static char separator;    // Trennzeichen für Komponenten (Default ',')
  static T epsilon;         // Schranke für Vergleiche

  public:
  // Constructors
  vec3() : x(0), y(0), z(0) {}
  explicit vec3(const T a) : x(a), y(a), z(a) {}
  vec3(const T ax,const T ay) : x(ax),y(ay),z(0.0) {}
  vec3(const T ax,const T ay,const T az) : x(ax),y(ay),z(az) {}
  vec3(const vec3<T>& v) : x(v.x), y(v.y), z(v.z) {}
  // Use defaults for copy constructor and assignment

  // Conversion operators 
  // (vec3<float> and vec3<double> can now be mixed)
  operator vec3<float>() const;
  operator vec3<double>() const;

  vec3<T>& set(const T ax, const T ay, const T az);
  vec3<T>& set_polar(const T r, const T theta, const T phi);
  void get(T& ax, T& ay, T& az) const;
  void get_polar(T& r, T& theta, T& phi) const;

  T& operator[](int);
  const T& operator[](int) const;

  vec3<T>& operator+=(const vec3<T>& v);
  vec3<T>& operator-=(const vec3<T>& v);
  vec3<T>& operator*=(const T r);
  vec3<T>& operator/=(const T r);
  vec3<T>& operator%=(const T r);
  vec3<T>& operator%=(const vec3<T>& b);

  vec3<T> operator+(const vec3<T>& b) const; // vec = vec + vec
  vec3<T> operator-(const vec3<T>& b) const; // vec = vec - vec
  vec3<T> operator-() const;                 // vec = -vec
  vec3<T> operator*(const T r) const;        // vec = vec * scalar
  T operator*(const vec3<T>& b) const;       // scalar = vec * vec (dot prod.)
  vec3<T> operator^(const vec3<T>& b) const;    // vec = vec * vec (cross prod.)
  vec3<T> operator/(const T r) const;        // vec = vec / scalar
  vec3<T> operator%(const T b) const;        // vec = vec % scalar (each component)
  vec3<T> operator%(const vec3<T>& b) const; // vec = vec % vec

  bool operator==(const vec3<T>& b) const;
  bool operator!=(const vec3<T>& b) const;
  bool operator<(const vec3<T>& b) const;
  bool operator>(const vec3<T>& b) const;
  bool operator<=(const vec3<T>& b) const;
  bool operator>=(const vec3<T>& b) const;
  bool nullvec() const;                      // *this == 0

  T length() const;

  vec3<T>  normalize() const;
  vec3<T>& normalize(vec3<T>& dest);
  // Return a vector that's perpendicular to this
  vec3<T>  ortho() const;

  vec3<T>  reflect(const vec3<T>& n) const;
  vec3<T>  refract(const vec3<T>& n, T eta) const;

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

  vec3<T>& add(const vec3<T>& a, const vec3<T>& b);   // this = a+b
  vec3<T>& sub(const vec3<T>& a, const vec3<T>& b);   // this = a-b
  vec3<T>& neg(const vec3<T>& a);                     // this = -a 
  vec3<T>& mul(const vec3<T>& a, const T r);          // this = a*r
  vec3<T>& div(const vec3<T>& a, const T r);          // this = a/r
  vec3<T>& cross(const vec3<T>& a, const vec3<T>& b); // this = a*b

  vec3<T> cross(const vec3<T>& b) const; // vec3 = this x b
};


// Return angle between a and b.
template<class T> 
T angle(const vec3<T> &a, const vec3<T> &b);

// Return signed angle between a and b.
template<class T> 
T sangle(const vec3<T> &a, const vec3<T> &b, const vec3<T> &axis);

/// Output operator for vectors.
template<class T> 
std::ostream& operator<<(std::ostream& s, const vec3<T>& a)
{
//  s<<a.leftBracket<<a.x<<a.separator<<" "<<a.y<<a.separator<<" "<<a.z<<a.rightBracket;
  s<<"("<<a.x<<", "<<a.y<<", "<<a.z<<")";
  return s;
}

/** 
   \var vec3::epsilon
   Threshold used for comparing two values.

   Two values are considered equal if the absolute value of their
   difference is less than or equal epsilon. The default value for
   float is 1E-7, for double and long double 1E-12 and for all integer
   types it is 0.
*/
   

///////////////////// Method definitions ///////////////////

// Skalarmultiplikation von links   Skalar*Vektor
template<class T> 
inline vec3<T> operator*(const T r,const vec3<T>& v) 
{
  return vec3<T>(r*v.x, r*v.y, r*v.z);
}

// Spezialisierungen für float und double (das double auch bei float
// definiert ist und umgekehrt).
inline vec3<float> operator*(const double r, const vec3<float>& v)
{
  return vec3<float>(float(r*v.x), float(r*v.y), float(r*v.z));
}
inline vec3<double> operator*(const float r, const vec3<double>& v)
{
  return vec3<float>(float(r*v.x), float(r*v.y), float(r*v.z));
}


/*-------------------------Conversion operator--------------------------*//**
   Convert to vec3<float>.
*//*------------------------------------------------------------------------*/
template<class T>
inline vec3<T>::operator vec3<float>() const
{
  return vec3<float>(x,y,z);
}

/*-------------------------Conversion operator--------------------------*//**
   Convert to vec3<double>.
*//*------------------------------------------------------------------------*/
template<class T>
inline vec3<T>::operator vec3<double>() const
{
  return vec3<double>(x,y,z);
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
inline vec3<T>& vec3<T>::set(const T ax, const T ay, const T az) 
{ 
  x=ax; 
  y=ay; 
  z=az; 
  return *this;
}

/*------------------------------set_polar--------------------------------*//**
  Set new components using polar coordinates.

  @param   r      Radius.
  @param   theta  Rotation around y-axis (angle between positive z-axis 
                  and vector).
  @param   phi    Rotation around z-axis (after above rotation).
  @return  Reference to this.
*//*------------------------------------------------------------------------*/
template<class T>
inline vec3<T>& vec3<T>::set_polar(const T r, const T theta, const T phi)
{
  x = r*sin(theta)*cos(phi);
  y = r*sin(theta)*sin(phi);
  z = r*cos(theta);
  return *this;
}

/*--------------------------------get-----------------------------------*//**
  Return components.

  @param ax  X component (output).
  @param ay  Y component (output).
  @param az  Z component (output).
*//*------------------------------------------------------------------------*/
template<class T>
inline void vec3<T>::get(T& ax, T& ay, T& az) const
{
  ax = x;
  ay = y;
  az = z;
}


/*------------------------------get_polar--------------------------------*//**
  Return the polar coordinates of the vector.

  @param   r      Output: Radius.
  @param   theta  Output: Rotation around y-axis (angle between 
                  positive z-axis and vector). Ranges from 0 to PI.
  @param   phi    Output: Rotation around z-axis (after above rotation).
*//*------------------------------------------------------------------------*/
template<class T>
inline void vec3<T>::get_polar(T& r, T& theta, T& phi) const
{
  r     = length();

  // r==0? (Null vector)
  if (r<=epsilon)
  {
    r=theta=phi=0.0;
    return;
  }
  theta = acos(z/r);

  // x==0? (then atan can't be computed -> phi is +PI/2 or -PI/2)
  if (xabs(x)<=epsilon)
  {
    if (y<0) phi=-M_PI/2.0; else phi = M_PI/2.0;
    return;
  }
  phi = atan(y/x);
  // Does phi need some correction?
  if (x<0)
  {
    // x<0
    if (y<0) phi += -M_PI; else phi = M_PI+phi;
  }
}


/*-----------------------------operator[]-------------------------------*//**
  Index operator.

  Return component i. The index is checked.

  @param     index  Component index (0,1 or 2).
  @return    Component i.
  @exception EIndexError  Index is out of range.
*//*------------------------------------------------------------------------*/
template<class T>
T& vec3<T>::operator[](int index)
{
  switch (index)
  {
   case 0: return x;
   case 1: return y;
   case 2: return z;
   default: throw EIndexError();
  }
}

/*-----------------------------operator[]-------------------------------*//**
  Index operator.

  Return component i. The index is checked.

  @param     index  Component index (0,1 or 2).
  @return    Component i.
  @exception EIndexError  Index is out of range.
*//*------------------------------------------------------------------------*/
template<class T>
const T& vec3<T>::operator[](int index) const
{
  switch (index)
  {
   case 0: return x;
   case 1: return y;
   case 2: return z;
   default: throw EIndexError();
  }
}

/*-----------------------------operator+=-------------------------------*//**
  vector += vector.

  @return    Reference to this.
*//*------------------------------------------------------------------------*/
template<class T>
inline vec3<T>& vec3<T>::operator+=(const vec3<T>& v)
{ 
  x+=v.x; 
  y+=v.y; 
  z+=v.z;
  return *this; 
}

/*-----------------------------operator-=-------------------------------*//**
  vector -= vector.

  @return    Reference to this.
*//*------------------------------------------------------------------------*/
template<class T>
inline vec3<T>& vec3<T>::operator-=(const vec3<T>& v) 
{ 
  x-=v.x; 
  y-=v.y; 
  z-=v.z; 
  return *this; 
}

/*-----------------------------operator*=-------------------------------*//**
  vector *= scalar.

  @return    Reference to this.
*//*------------------------------------------------------------------------*/
template<class T>
inline vec3<T>& vec3<T>::operator*=(const T r)
{ 
  x*=r; 
  y*=r; 
  z*=r; 
  return *this; 
}

/*-----------------------------operator/=-------------------------------*//**
  vector /= scalar.

  @return    Reference to this.
  @exception EZeroDivisionError  Is thrown when r=0.
*//*------------------------------------------------------------------------*/
template<class T>
inline vec3<T>& vec3<T>::operator/=(const T r)
{
  // r=0 ? then throw Exception
  if ( (r > epsilon) || (r < -epsilon) ) 
  {
    x/=r; y/=r; z/=r;
  }
  else
  {
    throw EZeroDivisionError("vec3: division by zero");
  }
  return *this;
}

/*-----------------------------operator%=-------------------------------*//**
  vector %= scalar.

  @return    Reference to this.
  @exception EZeroDivisionError  Is thrown when r=0.
*//*------------------------------------------------------------------------*/
template<class T>
inline vec3<T>& vec3<T>::operator%=(const T r)
{
  // r=0 ? then throw Exception
  if ( (r > epsilon) || (r < -epsilon) ) 
  {
    x = _fmod(x,r); 
    y = _fmod(y,r);
    z = _fmod(z,r);
  }
  else
  {
    throw EZeroDivisionError("vec3: division by zero");
  }
  return *this;
}

/*-----------------------------operator%=-------------------------------*//**
  vector %= vector

  @return    Reference to this.
  @exception EZeroDivisionError  Is thrown when a component of b=0.
*//*------------------------------------------------------------------------*/
template<class T>
inline vec3<T>& vec3<T>::operator%=(const vec3<T>& b)
{
  if ( ((b.x > epsilon) || (b.x < -epsilon)) &&
       ((b.y > epsilon) || (b.y < -epsilon)) &&
       ((b.z > epsilon) || (b.z < -epsilon)) )
  {
    x = _fmod(x,b.x); 
    y = _fmod(y,b.y);
    z = _fmod(z,b.z);
  }
  else
  {
    throw EZeroDivisionError("vec3: division by zero");
  }
  return *this;
}

/*-----------------------------operator+ -------------------------------*//**
  vector = vector+vector

  @return   Sum of two vectors.
*//*------------------------------------------------------------------------*/
template<class T> 
inline vec3<T> vec3<T>::operator+(const vec3<T>& b) const
{ 
//  vec3<T> res(*this);
//  return res+=b; 
  return vec3<T>(x+b.x, y+b.y, z+b.z);
}

/*-----------------------------operator- -------------------------------*//**
  vector = vector-vector

  @return   Difference of two vectors.
*//*------------------------------------------------------------------------*/
template<class T>
inline vec3<T> vec3<T>::operator-(const vec3<T>& b) const 
{ 
  return vec3<T>(x-b.x, y-b.y, z-b.z);
}

/*-----------------------------operator- -------------------------------*//**
  vector = -vector

  @return   Negated vector.
*//*------------------------------------------------------------------------*/
template<class T>
inline vec3<T> vec3<T>::operator-() const
{ 
  return vec3<T>(-x, -y, -z); 
}

/*-----------------------------operator* -------------------------------*//**
  vector = vector*scalar

  @return   Product of a vector and a scalar.
*//*------------------------------------------------------------------------*/
template<class T>
inline vec3<T> vec3<T>::operator*(const T r) const
{ 
  return vec3<T>(r*x, r*y, r*z); 
}

/*-----------------------------operator* -------------------------------*//**
  scalar = vector*vector (dot product).

  @return   Dot product of two vectors.
*//*------------------------------------------------------------------------*/
template<class T>
inline T vec3<T>::operator*(const vec3<T>& b) const
{ 
  return (x*b.x + y*b.y + z*b.z); 
}

/*-----------------------------operator^ -------------------------------*//**
  vector = vector*vector (cross product).

  Calculate cross product of two vectors. To determine the direction
  of the resulting vector use the same hand as you do for the coordinate
  system. Point the thumb in direction of the first vector a and the
  index finger in direction of the second vector b then the middle 
  finger shows the direction of a%b. The length of a^b is the surface
  area of the parallelogram spanned up by a and b. The cross product
  is zero if either a or b is zero or if a and b have the same
  direction.

  @return  Cross product of two vectors.
*//*------------------------------------------------------------------------*/
template<class T>
inline vec3<T> vec3<T>::operator^(const vec3<T>& b) const     
{ 
  return vec3<T>(y*b.z - z*b.y, z*b.x - x*b.z, x*b.y - y*b.x); 
}

/*-----------------------------operator/ -------------------------------*//**
  vector = vector / scalar.

  @return    Vector divided by scalar (component wise).
  @exception EZeroDivisionError  Is returned when r=0.
*//*------------------------------------------------------------------------*/
template<class T>
inline vec3<T> vec3<T>::operator/(const T r) const
{
  if (xabs(r)<=epsilon) 
  { 
    throw EZeroDivisionError("vec3: divide by zero");
  }
  return vec3<T>(x/r, y/r, z/r);
}

/*-----------------------------operator% -------------------------------*//**
  vector = vector % scalar.

  @return    Vector modulo scalar (component wise).
  @exception EZeroDivisionError  Is returned when r=0.
*//*------------------------------------------------------------------------*/
template<class T>
inline vec3<T> vec3<T>::operator%(const T r) const
{
  if (xabs(r)<=epsilon) 
  { 
    throw EZeroDivisionError("vec3: divide by zero");
  }
  return vec3<T>(_fmod(x,r), _fmod(y,r), _fmod(z,r));
}

/*-----------------------------operator% -------------------------------*//**
  vector = vector % vector

  @return    Vector modulo vector (component wise).
  @exception EZeroDivisionError  Is returned when component of b=0.
*//*------------------------------------------------------------------------*/
template<class T>
inline vec3<T> vec3<T>::operator%(const vec3<T>& b) const
{
  if ((xabs(b.x)<=epsilon) || (xabs(b.y)<=epsilon) || (xabs(b.z)<=epsilon))
  { 
    throw EZeroDivisionError("vec3: divide by zero");
  }
  return vec3<T>(_fmod(x,b.x), _fmod(y,b.y), _fmod(z,b.z));
}

/*-----------------------------nullvec----------------------------------*//**
  Check if vector = 0.

  @return    True, if each component is 0.
*//*------------------------------------------------------------------------*/
template<class T>
inline bool vec3<T>::nullvec() const
{
  return (xabs(x)<=epsilon) &&
         (xabs(y)<=epsilon) && 
         (xabs(z)<=epsilon);
}

/*-----------------------------operator==-------------------------------*//**
  vector == vector

  @return    True if both vectors are equal.
*//*------------------------------------------------------------------------*/
template<class T>
inline bool vec3<T>::operator==(const vec3<T>& b) const
{  
  return (xabs(x-b.x)<=epsilon) &&
         (xabs(y-b.y)<=epsilon) &&
         (xabs(z-b.z)<=epsilon);
}

/*-----------------------------operator!=-------------------------------*//**
  vector != vector

  @return    True if both vectors are not equal.
*//*------------------------------------------------------------------------*/
template<class T>
inline bool vec3<T>::operator!=(const vec3<T>& b) const
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
inline bool vec3<T>::operator<(const vec3<T>& b) const
{  
  return (x<b.x) && (y<b.y) && (z<b.z);
}

/*-----------------------------operator> -------------------------------*//**
  vector > vector

  For this test the epsilon threshold is \b not used!

  @return   True if each component of the first vector is greater than the
            according component of the second vector.
*//*------------------------------------------------------------------------*/
template<class T>
inline bool vec3<T>::operator>(const vec3<T>& b) const
{  
  return (x>b.x) && (y>b.y) && (z>b.z);
}

/*-----------------------------operator<=-------------------------------*//**
  vector <= vector

  For this test the epsilon threshold is \b not used!

  @return   True if each component of the first vector is less than or 
            equal the according component of the second vector.
*//*------------------------------------------------------------------------*/
template<class T>
inline bool vec3<T>::operator<=(const vec3<T>& b) const
{  
  return (x<=b.x) && (y<=b.y) && (z<=b.z);
}

/*-----------------------------operator>=-------------------------------*//**
  vector >= vector

  For this test the epsilon threshold is \b not used!

  @return   True if each component of the first vector is greater than or 
            equal the according component of the second vector.
*//*------------------------------------------------------------------------*/
template<class T>
inline bool vec3<T>::operator>=(const vec3<T>& b) const
{  
  return (x>=b.x) && (y>=b.y) && (z>=b.z);
}

/*------------------------------length----------------------------------*//**
  Return length of the vector.

  @return   Length of vector.
*//*------------------------------------------------------------------------*/
template<class T>
inline T vec3<T>::length() const 
{ 
  return sqrt(x*x + y*y + z*z); 
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
inline vec3<T>& vec3<T>::normalize(vec3<T>& dest)
{
  T vl=length();     // vl: Länge des (eigenen) Vektors

  // vl=0? dann Exception
  if (vl<=epsilon) throw EZeroDivisionError("vec3.normalize(): divide by zero");

  vl = 1 / vl;
  dest.x = vl*x;
  dest.y = vl*y;
  dest.z = vl*z;
  return dest;
}

/*-----------------------------normalize-------------------------------*//**
  Normalize vector.

  See normalize.

  @return    Normalized vector.
  @exception EZeroDivisionError  Is thrown if the length of the vector is zero.
*//*------------------------------------------------------------------------*/
template<class T>
inline vec3<T> vec3<T>::normalize() const
{
  T vl=length();     // vl: Länge des (eigenen) Vektors

  // vl=0? dann Exception
  if (vl<=epsilon) throw EZeroDivisionError("vec3.normalize(): divide by zero");

  vl = 1 / vl;
  return vec3<T>(vl*x, vl*y, vl*z);
}

/*--------------------------------ortho--------------------------------*//**
  Return a vector that is orthogonal to *this.

  Returns an arbitrary vector that is orthogonal to *this (dot product
  is zero).

  The implementation uses the algorithm as described in:

  John F. Hughes and Tomas Möller<br> 
  \em Building \em an \em Orthonormal \em Basis \em from \em a \em Unit \em Vector<br>
  Journal of graphics tools, 4(4):33-35, 1999<br>
  http://www.acm.org/jgt/papers/HughesMoller99/<br>

  \return    Orthogonal vector.
*//*------------------------------------------------------------------------*/
template<class T> 
inline vec3<T> vec3<T>::ortho() const
{
  double ax = xabs(x);
  double ay = xabs(y);
  double az = xabs(z);

  // Is x the smallest component?
  if (ax<ay && ax<az)
    return vec3<T>(0, -z, y);

  // Is y the smallest component?
  if (ay<az)
    return vec3<T>(-z, 0, x);

  // z is the smallest component
  return vec3<T>(-y, x, 0);
}

/**
 Return reflection vector.

 Return the reflection vector given an incident direction (this) 
 and a (normalized) normal vector n.

 (Taken from the Renderman specs)

 @return Reflection vector.
 */
template<class T>
vec3<T> vec3<T>::reflect(const vec3<T>& n) const
{
  return (*this)-2.0*( (*this)*n )*n;
}

/**
 Return transmitted vector.

 Return the transmitted vector given an incident direction (this), the
 normal vector n and the relative index of refraction eta. eta is the
 ratio of the index of refraction in the volume containing the
 incident vector to that of the volume being entered. This vector is
 computed using Snell's law. If the returned vector has zero length,
 then there is no transmitted light because of total internal
 reflection.

 (Taken from the Renderman specs)
 
 @return Transmitted vector.
*/
template<class T>
vec3<T> vec3<T>::refract(const vec3<T>& n, T eta) const
{
  T IdotN = (*this)*n;
  T k     = 1.0 - eta*eta*(1.0 - IdotN*IdotN);

  if (k<epsilon)
  {
    return vec3<T>(0,0,0);
  }
  else
  {
    return eta*(*this) - (eta*IdotN + sqrt(k))*n;
  }
}


/*--------------------------------max----------------------------------*//**
  Return component with maximum value.

  @return    Maximum value.
*//*------------------------------------------------------------------------*/
template<class T> 
inline T vec3<T>::max() const
{
  return std::max(std::max(x,y),z);
}

/*--------------------------------maxIndex----------------------------*//**
  Return index of component with maximum value.

  @return  Index (0,1 or 2).
*//*------------------------------------------------------------------------*/
template<class T> 
inline int vec3<T>::maxIndex() const
{
  // Is x maximum?
  if ((x>=y) && (x>=z))
  {
    return 0;
  }
  else
  {
    if (y>=z) return 1; else return 2;
  }
}

/*--------------------------------min----------------------------------*//**
  Return component with minimum value.

  @return    Minimum value.
*//*------------------------------------------------------------------------*/
template<class T> 
inline T vec3<T>::min() const
{
  return std::min(std::min(x,y),z);
}

/*--------------------------------minIndex----------------------------*//**
  Return index of component with minimum value.

  @return   Index (0,1 or 2).
*//*------------------------------------------------------------------------*/
template<class T> 
inline int vec3<T>::minIndex() const
{
  // Is x minimum?
  if ((x<=y) && (x<=z))
  {
    return 0;
  }
  else
  {
    if (y<=z) return 1; else return 2;
  }
}

/*-------------------------------maxAbs--------------------------------*//**
  Return component with maximum absolute value.

  @return    Maximum absolute value.
*//*------------------------------------------------------------------------*/
template<class T> 
inline T vec3<T>::maxAbs() const
{
  return std::max(std::max(xabs(x),xabs(y)),xabs(z));
}

/*-----------------------------maxAbsIndex-----------------------------*//**
  Return index of component with maximum absolute value.

  @return    Index (0,1 or 2).
*//*------------------------------------------------------------------------*/
template<class T> 
inline int vec3<T>::maxAbsIndex() const
{
  T ax = xabs(x);
  T ay = xabs(y);
  T az = xabs(z);

  // Is ax maximum?
  if ((ax>=ay) && (ax>=az))
  {
    return 0;
  }
  else
  {
    if (ay>=az) return 1; else return 2;
  }
}

/*-------------------------------minAbs--------------------------------*//**
  Return component with minimum absolute value.

  @return    Minimum absolute value.
*//*------------------------------------------------------------------------*/
template<class T> 
inline T vec3<T>::minAbs() const
{
  return std::min(std::min(xabs(x),xabs(y)),xabs(z));
}

/*-----------------------------minAbsIndex-----------------------------*//**
  Return index of component with minimum absolute value.

  @return    Index (0,1 or 2).
*//*------------------------------------------------------------------------*/
template<class T> 
inline int vec3<T>::minAbsIndex() const
{
  T ax = xabs(x);
  T ay = xabs(y);
  T az = xabs(z);

  // Is ax minimum?
  if ((ax<=ay) && (ax<=az))
  {
    return 0;
  }
  else
  {
    if (ay<=az) return 1; else return 2;
  }
}


/*---------------------------------add----------------------------------*//**
  this = a+b

  If speed is critical you can use this function instead of the
  + operator. This way you save one copy operation.

  @return   Reference to this.
*//*------------------------------------------------------------------------*/
template<class T>
inline vec3<T>& vec3<T>::add(const vec3<T>& a, const vec3<T>& b)
{ 
  x=a.x+b.x; 
  y=a.y+b.y; 
  z=a.z+b.z; 
  return *this; 
}

/*---------------------------------sub---------------------------------*//**
  this = a-b

  If speed is critical you can use this function instead of the
  - operator. This way you save one copy operation.

  @return   Reference to this.
*//*------------------------------------------------------------------------*/
template<class T>
inline vec3<T>& vec3<T>::sub(const vec3<T>& a, const vec3<T>& b)
{ 
  x=a.x-b.x; 
  y=a.y-b.y; 
  z=a.z-b.z; 
  return *this; 
}

/*---------------------------------neg---------------------------------*//**
  this = -a

  If speed is critical you can use this function instead of the
  - operator. This way you save one copy operation.

  @return   Reference to this.
*//*------------------------------------------------------------------------*/
template<class T>
inline vec3<T>& vec3<T>::neg(const vec3<T>& a)
{ 
  x=-a.x; 
  y=-a.y; 
  z=-a.z; 
  return *this; 
}

/*--------------------------------cross--------------------------------*//**
  this = a x b (cross product).

  If speed is critical you can use this function instead of the
  % operator. This way you save one copy operation.
  It is not allowed to pass this as either a or b!

  @return   Reference to this.
*//*------------------------------------------------------------------------*/
template<class T>
inline vec3<T>& vec3<T>::cross(const vec3<T>& a, const vec3<T>& b)
{ 
  x=a.y*b.z-a.z*b.y; 
  y=a.z*b.x-a.x*b.z; 
  z=a.x*b.y-a.y*b.x; 
  return *this; 
}

template<class T>
inline vec3<T> vec3<T>::cross(const vec3<T>& b) const
{
  return vec3<T>(y*b.z-z*b.y,
                 z*b.x-x*b.z,
                 x*b.y-y*b.x);
}


/*--------------------------------mul----------------------------------*//**
  this = a * r

  If speed is critical you can use this function instead of the
  * operator. This way you save one copy operation.

  @return   Reference to this.
*//*------------------------------------------------------------------------*/
template<class T>
inline vec3<T>& vec3<T>::mul(const vec3<T>& a, const T r)
{ 
  x=a.x*r;
  y=a.y*r;
  z=a.z*r;
  return *this;
}

/*--------------------------------div----------------------------------*//**
  this = a / r

  If speed is critical you can use this function instead of the
  / operator. This way you save one copy operation.

  @return   Reference to this.
*//*------------------------------------------------------------------------*/
template<class T>
inline vec3<T>& vec3<T>::div(const vec3<T>& a, const T r)
{ 
  x=a.x/r;
  y=a.y/r;
  z=a.z/r;
  return *this;
}


/*-------------------------------angle---------------------------------*//**
  Determine angle (in radians) between vectors a and b.
  If either a or b is 0 then an exception is thrown. The order of
  the two vectors doesn't matter, angle(a,b) is the same as angle(b,a).

  @param     a  1st vector.
  @param     b  2nd vector.
  @return    Angle in radians, lies between 0 and pi.
  @exception EZeroDivisionError  Is thrown if either a or b is zero.
*//*------------------------------------------------------------------------*/
template<class T> 
inline T angle(const vec3<T> &a, const vec3<T> &b)
{
  T ab = a.length()*b.length();
  if (ab<=vec3<T>::epsilon) throw EZeroDivisionError("vec3.angle(): divide by zero");
  T c = (a*b) / ab;
  // The following if's guarantee that the argument for acos() is valid.
  // (it should be anyway but because of rounding errors it can fall 
  // out of [-1,1])
  if (c>1) c=1;
  if (c<-1) c=-1;
  return acos(c);
}

/*-------------------------------sangle---------------------------------*//**
  Determine signed angle (in radians) between vectors a and b relative
  to a given axis. If the axis points in the same direction (in the same
  half space) than the cross product of a and b then the angle is
  positive otherwise negative.
  If either a or b is 0 then an exception is thrown. 
  The order of the two vectors in the function call determine the
  sign of the result: sangle(a,b,axis) = -sangle(b,a,axis)

  @param     a     1st vector.
  @param     b     2nd vector.
  @param     axis  Reference axis. Defines the positive direction of 
                   a rotation (counterclockwise).
  @return    Angle (in radians), lies between -pi and pi.
  @exception EZeroDivisionError  Is thrown if either a or b is zero.
*//*------------------------------------------------------------------------*/
template<class T> 
inline T sangle(const vec3<T> &a, const vec3<T> &b, const vec3<T> &axis) 
{
  T w = angle(a,b);      // Angle (unsigned)

  vec3<T> n=a%b;         // Orthogonal direction (can still be zero!)

  // Either a or b was zero or they had the same (or opposite) direction.
  if (n.nullvec()) return w;

  // If the angle between n and the reference axis is greater than
  // 90 degrees then the angle is negative (switch sign).
  // If n or axis is zero an exception is thrown by angle.
  return (angle(n,axis)>M_PI_2)? -w : w;
}

typedef vec3<double> vec3d;
typedef vec3<float> vec3f;

} // end of namespace


#endif
