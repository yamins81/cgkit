/*
  Common helper functions.
 */

#include "common_funcs.h"


/**
  Convert a Lib3dsMatrix into a cgkit mat4.

  (it's converted so that the translation part is on the 4th \em column.

  \param m Matrix
  \return mat4 matrix
 */
support3d::mat4d matrix2mat4(Lib3dsMatrix& m)
{
  return support3d::mat4d(m[0][0], m[1][0], m[2][0], m[3][0],
			  m[0][1], m[1][1], m[2][1], m[3][1],
			  m[0][2], m[1][2], m[2][2], m[3][2],
			  m[0][3], m[1][3], m[2][3], m[3][3]);
}

/**
  Convert a Lib3dsVector into a cgkit vec3.

  \param v Vector
  \return vec3 vector
 */
support3d::vec3d vector2vec3(Lib3dsVector& v)
{
  return support3d::vec3d(v[0], v[1], v[2]);
}

/**
  Convert a Lib3dsQuat into a cgkit quat.

  \param q Quat
  \return cgkit quat
 */
support3d::quatd quat2quat(Lib3dsQuat& q)
{
  return support3d::quatd(q[3], q[0], q[1], q[2]);
}

/**
  Convert a Lib3dsRgb into a cgkit vec3.

  \param c Color
  \return vec3 vector
 */
support3d::vec3d color2vec3(Lib3dsRgb& c)
{
  return support3d::vec3d(c[0], c[1], c[2]);
}

/**
  Convert a Lib3dsRgba into a cgkit vec4.

  \param c Color
  \return vec4 vector
 */
support3d::vec4d color2vec4(Lib3dsRgba& c)
{
  return support3d::vec4d(c[0], c[1], c[2], c[3]);
}

