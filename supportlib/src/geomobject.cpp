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

#define DLL_EXPORT_GEOMOBJECT
#include "geomobject.h"
#include "slot.h"
#include "arrayslot.h"
#include "vec3.h"
#include "vec4.h"
#include "mat4.h"

namespace support3d {

GeomObject::~GeomObject()
{
  deleteAllVariables();
}

// Return the uniform count by retrieving a constraint object
int GeomObject::uniformCount() const
{
  boost::shared_ptr<SizeConstraintBase> c = slotSizeConstraint(UNIFORM);
  if (c.get()==0)
    return -1;
  return c->getSize(); 
}

// Return the varying count by retrieving a constraint object
int GeomObject::varyingCount() const
{
  boost::shared_ptr<SizeConstraintBase> c = slotSizeConstraint(VARYING);
  if (c.get()==0)
    return -1;
  return c->getSize(); 
}

// Return the vertex count by retrieving a constraint object
int GeomObject::vertexCount() const
{
  boost::shared_ptr<SizeConstraintBase> c = slotSizeConstraint(VERTEX);
  if (c.get()==0)
    return -1;
  return c->getSize(); 
}

// Return the facevarying count by retrieving a constraint object
int GeomObject::faceVaryingCount() const
{
  boost::shared_ptr<SizeConstraintBase> c = slotSizeConstraint(FACEVARYING);
  if (c.get()==0)
    return -1;
  return c->getSize(); 
}

// Return the facevertex count by retrieving a constraint object
int GeomObject::faceVertexCount() const
{
  boost::shared_ptr<SizeConstraintBase> c = slotSizeConstraint(FACEVERTEX);
  if (c.get()==0)
    return -1;
  return c->getSize(); 
}


/**
  Create a new primitive variable.

  A new primitive variable is created which will also create a corresponding
  ArraySlot of the same name. If you want to manipulate the data items you
  have to retrieve the slot after you have created the variable.
  
  The meaning of the storage class and type arguments are borrowed from the 
  RenderMan API (except for the USER storage class). The storage class determines
  how man individual data items will be stored. The exact number depends on the
  primitive type. For example, if you have a triangle mesh and you create a 
  variable with storage class UNIFORM you will be able to store one data item
  per triangle while storage classes VARYING and VERTEX will result in one data
  item per vertex (the difference between VARYING and VERTEX is how the values
  are interpolated in between vertices).

  \param name The name of the new variable
  \param storage Storage class of the new variable
  \param type The type of the new variable
  \param multiplicity The array length of an individual item (1=no array)
  \param user_n The number of elements for storage class USER.
  \see slot (), findVariable (), deleteVariable (), deleteAllVariables (), variablesBegin (), variablesEnd ()
  \todo Was wenn Name schon existiert?
 */
void GeomObject::newVariable(string name, VarStorage storage, VarType type, int multiplicity, int user_n)
{
  PrimVarInfo info;
  ArraySlot<int>* intslot;
  ArraySlot<double>* doubleslot;
  ArraySlot<vec3d>* vec3slot;
  ArraySlot<vec4d>* vec4slot;
  ArraySlot<mat4d>* mat4slot;
  ArraySlot<std::string>* strslot;
  auto_ptr<ISlot> slot;

  info.storage = storage;
  info.type = type;
  info.multiplicity = multiplicity;

  // Obtain an appropriate constraint object for the slot
  boost::shared_ptr<SizeConstraintBase> constraint;
  switch(storage)
  {
   case CONSTANT: 
     // Always constrain the size to 1
     constraint = boost::shared_ptr<SizeConstraintBase>(new UserSizeConstraint(1)); 
     break;
   case UNIFORM:
   case VARYING:
   case VERTEX:
   case FACEVARYING:
   case FACEVERTEX:
     // The constraint depends on the geometry...
     constraint = slotSizeConstraint(storage);
     break;
   case USER:
     // No constraint
     constraint = boost::shared_ptr<SizeConstraintBase>(); 
     break;
  }

  // Create the slot...
  switch(type)
  {
   case INT: intslot = new ArraySlot<int>(multiplicity, constraint);
             info.slot = intslot;
             slot = auto_ptr<ISlot>(intslot);
             break;
   case FLOAT: doubleslot = new ArraySlot<double>(multiplicity, constraint);
             info.slot = doubleslot;
             slot = auto_ptr<ISlot>(doubleslot);
             break;
   case COLOR:
   case NORMAL:
   case POINT:
   case VECTOR: vec3slot = new ArraySlot<vec3d>(multiplicity, constraint);
             info.slot = vec3slot;
             slot = auto_ptr<ISlot>(vec3slot);
             break;
   case HPOINT: vec4slot = new ArraySlot<vec4d>(multiplicity, constraint);
             info.slot = vec4slot;
             slot = auto_ptr<ISlot>(vec4slot);
             break;
   case MATRIX: mat4slot = new ArraySlot<mat4d>(multiplicity, constraint);
             info.slot = mat4slot;
             slot = auto_ptr<ISlot>(mat4slot);
             break;
   case STRING: strslot = new ArraySlot<std::string>(multiplicity, constraint);
             info.slot = strslot;
             slot = auto_ptr<ISlot>(strslot);
             break;
  }

  // Resize if it's a user slot...
  if (storage==USER)
    info.slot->resize(user_n);

  variables[name] = info;
  addSlot(name, slot);
}

/**
  Delete a primitive variable.

  \param name Name of the primitive variable
  \see newVariable (), findVariable (), deleteAllVariables (), variablesBegin (), variablesEnd ()
  \todo What happens if name does not exist?
 */
void GeomObject::deleteVariable(string name)
{
  std::map<string, PrimVarInfo>::iterator it = variables.find(name);
  if (it!=variables.end())
  {
    removeSlot(name);
    variables.erase(it);
  }
}

/**
  Delete all primitive variables.

  \see newVariable (), findVariable (), deleteVariable (), variablesBegin (), variablesEn d()
 */
void GeomObject::deleteAllVariables()
{
  while(variables.begin()!=variables.end())
  {
    deleteVariable(variables.begin()->first);
  }
}

/**
  Search for a particular primitive variable and return its variable descriptor.

  \param name The name of the primitive variable.
  \return Primitive variable descriptor or 0 if the variable wasn't found.
  \see newVariable (), deleteVariable (), deleteAllVariables (), variablesBegin (), variablesEn d()
 */
PrimVarInfo* GeomObject::findVariable(string name)
{
  std::map<string, PrimVarInfo>::iterator it = variables.find(name);
  if (it!=variables.end())
  {
    return &(it->second);
  }
  else
  {
    return 0;
  }
}


}  // end of namespace
