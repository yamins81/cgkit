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

#ifndef GEOMOBJECT_H
#define GEOMOBJECT_H

/** \file geomobject.h
 Contains the base GeomObject class.
 */

#include "component.h"
#include "boundingbox.h"
#include "common_exceptions.h"
#include "arrayslot.h"
#include <boost/shared_ptr.hpp>

// Define the CGKIT_SHARED variable
#ifdef DLL_EXPORT_GEOMOBJECT
  #include "shared_export.h"
#else
  #include "shared.h"
#endif

namespace support3d {

enum VarStorage { CONSTANT, UNIFORM, VARYING, VERTEX, FACEVARYING, FACEVERTEX, USER };
enum VarType { INT, FLOAT, STRING, COLOR, POINT, VECTOR, NORMAL, MATRIX, HPOINT };


/**
  Descriptor for a primitive variable.

  \see GeomObject
 */
struct PrimVarInfo
{
  /// Storage class
  VarStorage storage;
  /// Variable type
  VarType type;
  /// Multiplicity (i.e. the array length).
  int multiplicity;
  /// A pointer to the corresponding ArraySlot.
  IArraySlot* slot;
};

// Forward declaration for the convertToMesh() method
class TriMeshGeom;

/**
  Base class for all geometries.

  This is the base class for all geometry classes. A geometry class
  represents a particular kind of geometry like a particular quadric,
  a triangle mesh, a NURBS mesh, subdivision surface, etc. It also
  manages primitive variables that can be attached to the geometry
  (much in the same vein as in the RenderMan API). That functionality
  is already implemented in this base class, a derived class only has
  to provide the SizeConstraint objects that determine the size of the
  array slots that hold the values for the primitive variable.
  Additionally, a geometry class should be able to compute basic mass
  properties such as the center of gravity (cog) and the inertia
  tensor which are necessary for rigid body dynamics.

  Derived classes have to implement the following methods:

  - boundingBox()
  - drawGL()
  - slotSizeConstraint()

  Additionally they should create a vec3 slot called \em "cog" and a
  mat3 slot called \em "inertiatensor" that contain the mass
  properties of the geometry under the assumption that the total mass
  is 1.0 (the reference frame is the local coordinate system). If
  these slots are available a world object using this geometry can be
  added to a dynamics simulation.

  \see PrimVarInfo
 */
class CGKIT_SHARED GeomObject : public Component
{
  public:
  typedef std::map<string, PrimVarInfo>::const_iterator VariableIterator;

  protected:
  /// Primitive variables.
  std::map<string, PrimVarInfo> variables;

  public:
  GeomObject() : Component("geom") {};
  virtual ~GeomObject();

  /**
    Return the local axis aligned bounding box.

    The bounding box is given with respect to the local 
    transformation L (which is \em not what you get from
    the transform slot of the world object).

    \return Local bounding box.
   */
  virtual BoundingBox boundingBox() = 0;

  /**
    Draw the geometry using OpenGL commands.
   */
  virtual void drawGL() {}

  /**
    Get the center of gravity slot.
   */
//  virtual Slot<vec3d>& cogSlot() { throw ENotImplementedError(); }
//  virtual Slot<mat3d>& inertiaTensorSlot() { throw ENotImplementedError(); }

  /**
    Return the multiplicity of uniform variables.
   */
  virtual int uniformCount() const;
  /**
    Return the multiplicity of varying variables.
   */
  virtual int varyingCount() const;
    /**
    Return the multiplicity of vertex variables.
   */
  virtual int vertexCount() const;
  /**
    Return the multiplicity of facevarying variables.

    The return value is 0 by default.
   */
  virtual int faceVaryingCount() const;
  /**
    Return the multiplicity of facevertex variables.

    The return value is 0 by default.
   */
  virtual int faceVertexCount() const;

  /**
     Return a constraint object for primitive variable slots.

     This method is called when a new primitive variable is created.
     The returned constraint object is used for the array slot that holds
     the values of the variable.
     If the size of the primitive variable slot is unconstrained an empty
     pointer has to be returned.

     The method is \b not called on storage classes CONSTANT and USER.

     The default implementation returns a constraint that constrains the slot
     size to 0 (i.e. no values allowed).

     \param storage Storage class of the variable.
     \return A reference to a SizeConstraintBase or an empty pointer.
   */
  virtual boost::shared_ptr<SizeConstraintBase> slotSizeConstraint(VarStorage storage) const
    { return boost::shared_ptr<SizeConstraintBase>(new UserSizeConstraint(0)); }

  virtual void newVariable(string name, VarStorage storage, VarType type, int multiplicity=1, int user_n=0);
  virtual void deleteVariable(string name);
  virtual void deleteAllVariables();
  virtual PrimVarInfo* findVariable(string name);
  VariableIterator variablesBegin() const { return variables.begin(); }
  VariableIterator variablesEnd() const { return variables.end(); }

  /**
    Convert the geom object into another geom object.

    If possible this method converts the geom into the same
    type as the geom \a target. An exception is thrown if 
    the conversion is not possible.

    The converted geom only represents the current state of the object. 
    If the current time changes or any other state changes, the object
    \a target might not be up to date anymore.

    \param target Target geom object that will hold the result
   */
  virtual void convert(GeomObject* target) { throw ENotImplementedError("Conversion not supported."); }
};


}  // end of namespace

#endif
