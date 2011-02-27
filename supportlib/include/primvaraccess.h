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

#ifndef PRIMVARACCESS_H
#define PRIMVARACCESS_H

/** \file primvaraccess.h
 Contains the PrimVarAccess class.
 */

#include <string>
#include "geomobject.h"

namespace support3d {

/**
  Helper class to access primitive variables during rendering.

  The class takes the geom and information about the variable in
  question as input to its constructor. During rendering, the 
  onFace() method has to be called for each face and onVertex() for
  each vertex. These methods return the next value if available.

  The class supports the following variable types:

  - constant (in which case onFace() returns a value each time)
  - uniform (onFace() returns a value)
  - varying (onVertex() returns a value)
  - facevarying (onVertex() returns a value)
  - user + facevarying faces variable (onVertex() returns a value)
  - Optional: user + uniform int[3] faces variable (onVertex() returns a value). This mode can only be used with TriMeshGeom classes!

  Example:

  \code 
  PrimVarAccess<vec3d> normals(geom, std::string("N"), NORMAL, 1, std::string("Nfaces"));
  vec3d* N;

  for(...each face...)
  {
    if (normals.onFace(N))
    {
      // ...do something with the normal...
    }

    for(...each vertex i...)
    {
	if (normals.onVertex(i, N))
	{
	  // ...do something with the normal...
        }
    }
  }
  \endcode
 */
template<class T>
class PrimVarAccess
{
  public:
  /** 
    Access mode.

    - 0: No access. Variable is not available or of wrong type.
    - 1: Variable is constant
    - 2: Variable is uniform
    - 3: Variable is varying
    - 4: Variable is facevarying
    - 5: Variable is user (+ faces, either facevarying or uniform int[3] (trimesh only!))
  */
  int mode;

  /**
     A pointer to the raw variable data.
  */
  T* var_ptr;

  /**
     A pointer to the raw variable data that contains the "XYZfaces" variable.
  */
  int* varfaces_ptr;

  /**
    Multiplicity of the variable.
   */
  int mult;

  /**
     The size of the ArraySlot containing the values.

     This is used for checking the range. The value might be used as
     a counter (i.e. it may be decreased inside the onFace() or onVertex()
     methods.
  */
  int var_size;

  /**
     The size of the ArraySlot containing the "faces" values.

     This is used for checking the range. The value might be used as
     a counter (i.e. it may be decreased inside the onFace() or onVertex()
     methods.
  */
  int varfaces_size;

  public:
  PrimVarAccess(GeomObject& geom, std::string varname, VarType vartype, int varmult, std::string varfacesname=std::string(""), bool trimesh_flag=false);
  
  bool onFace(T*& value);
  bool onVertex(int idx, T*& value);
  
  private:
  void initMode(GeomObject& geom, std::string varname, VarType vartype, int varmult, std::string varfacesname, bool trimesh_flag);

};

//////////////////////////////////////////////////////////////////////

/**
  Constructor.

  If the varfacesname argument is an empty string, then the mode using
  the "faces" variable is disabled. When the class is used for a 
  TriMeshGeom the \a trimesh_flag argument should be true to enable
  faces that are declared as "uniform int[3]".

  \param geom Geometry that is currently being processed
  \param varname The name of the variable that should be read
  \param vartype The required type of the variable (INT, FLOAT, ...)
  \param varmult The required multiplicty of the variable
  \param varfacesname The name of the corresponding "faces" variable
  \param trimesh_flag Determines whether uniform int[3] faces are allowed
 */
template<class T>
PrimVarAccess<T>::PrimVarAccess(GeomObject& geom, std::string varname, VarType vartype, int varmult, std::string varfacesname, bool trimesh_flag)
  : mode(0), var_ptr(0), varfaces_ptr(0), mult(varmult), 
    var_size(0), varfaces_size(0)
{
  initMode(geom, varname, vartype, varmult, varfacesname, trimesh_flag);
}

/**
  Helper method to initialize the class.

  This method is called inside the constructor.

  \param geom Geometry that is currently being processed
  \param varname The name of the variable that should be read
  \param vartype The required type of the variable (INT, FLOAT, ...)
  \param varmult The required multiplicty of the variable
  \param varfacesname The name of the corresponding "faces" variable
  \param trimesh_flag Determines whether uniform int[3] faces are allowed
 */
template<class T>
void PrimVarAccess<T>::initMode(GeomObject& geom, std::string varname, VarType vartype, int varmult, std::string varfacesname, bool trimesh_flag)
{
  PrimVarInfo* info;

  // Default mode: No access
  mode = 0;

  // Check if the variable is available...
  info = geom.findVariable(varname);
  if (info==0)
    return;
  
  // Is the variable of the wrong type or multiplicity?
  if (info->type!=vartype || info->multiplicity!=varmult)
    return;

  // Get a pointer to the data...
  var_ptr = dynamic_cast<ArraySlot<T>* >(info->slot)->dataPtr();
  if (var_ptr==0)
    return;

  // Obtain the array size...
  var_size = info->slot->size();

  // Determine the mode...
  switch(info->storage)
  {
    // "constant"?
    case CONSTANT: if (var_size>0) mode = 1; break;
    // "uniform"?
    case UNIFORM: mode = 2; break;
    // "varying"?
    case VARYING: mode = 3; break;
    // "facevarying"?
    case FACEVARYING: mode = 4; break;
    // "user"?
    case USER:
      if (varfacesname!="")
      {
	// Check if the corresponding "faces" variable is available...
	info = geom.findVariable(varfacesname);
	if (info==0 || info->type!=INT)
	  return;

	varfaces_ptr = dynamic_cast<ArraySlot<int>* >(info->slot)->dataPtr();
	if (varfaces_ptr==0)
	  return;

	// Obtain the array size...
	varfaces_size = info->slot->size();

	if (info->storage==UNIFORM && info->multiplicity==3 && trimesh_flag)
	{
	  // Multiply the varfaces size with 3 because of its multiplicity
	  // (i.e. there are 3 times more elements in the array than 
	  // reported by the size)
	  varfaces_size *= 3;
	  mode = 5;
	}
	else if (info->storage==FACEVARYING && info->multiplicity==1)
	{
	  mode = 5;
	}
      }
  }
}

/**
   Per face method.

   This method must be called before a face is processed. If there is a
   value available for the entire face then this value is passed back
   via \a value and \c true is returned.
   It is assumed that faces are processed in order (i.e. the first call
   will return the value for the first face, the second call the value
   for the second face and so on).
   The method performs range checking and returns false if any index is 
   out of range.

   \param[out] value Variable value for the next face
   \return True if there was a value, otherwise false.
 */
template<class T>  
bool PrimVarAccess<T>::onFace(T*& value)
{
  switch(mode)
  {
    // constant?
   case 1: value = var_ptr;
           return true;
     
   // uniform?
   case 2: if (var_size==0)
             return false;
           var_size--;
           value = var_ptr;
           var_ptr += mult;
           return true;

   default: return false;
  }
}

/**
  Per vertex method.

  This method must be called before a vertex is processed. If there is
  a value available for the vertex then this value is passed back via
  \a value and \c true is returned.
  It is assumed that the vertices are processed in order (unless the storage
  class of the variable is "varying").
  The method performs range checking and returns false if any index is 
  out of range.

  \param idx This is the index of the vertex (required for "varying" variables)
  \param[out] value Variable value for the next vertex
  \return True if there was a value, otherwise false.
 */
template<class T>  
bool PrimVarAccess<T>::onVertex(int idx, T*& value)
{
  int vidx;
  switch(mode)
  {
    // varying?
    case 3: if (idx>=var_size || idx<0)
              return false;
            value = var_ptr + mult*idx; 
            return true;

    // facevarying?
    case 4: if (var_size==0)
              return false;
            var_size--;
            value = var_ptr; 
            var_ptr += mult; 
            return true;

    // user (+uniform int[3] or facevarying faces)?
    // (both cases can be treated identically)
    case 5: if (varfaces_size==0)
              return false;
            varfaces_size--;
            vidx = *varfaces_ptr;
            varfaces_ptr++;
	    if (vidx>=var_size || vidx<0)
	      return false;
            value = var_ptr + mult*vidx;
            return true;

    // default: no value
    default: return false;
  }
}


} // end of namespace

#endif

