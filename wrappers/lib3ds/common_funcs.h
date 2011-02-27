/*
  Common helper functions.
 */


#ifndef COMMON_FUNCS_H
#define COMMON_FUNCS_H

#include <lib3ds/types.h>
#include <vec3.h>
#include <vec4.h>
#include <mat4.h>
#include <quat.h>

support3d::mat4d matrix2mat4(Lib3dsMatrix& m);
support3d::vec3d vector2vec3(Lib3dsVector& v);
support3d::quatd quat2quat(Lib3dsQuat& q);
support3d::vec3d color2vec3(Lib3dsRgb& c);
support3d::vec4d color2vec4(Lib3dsRgba& c);

#endif
