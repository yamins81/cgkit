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

#ifndef WORLDOBJECT_H
#define WORLDOBJECT_H

/** \file worldobject.h
 Contains the base WorldObject class.
 */

#include <vector>
#include <boost/shared_ptr.hpp>
//#include <boost/enable_shared_from_this.hpp>
#include "nulldeleter.h"
#include "component.h"
#include "proceduralslot.h"
#include "vec3.h"
#include "mat3.h"
#include "mat4.h"
#include "geomobject.h"
#include "material.h"

namespace support3d {

class TransformSlot;

/** 
  Position slot that accesses the pos part of a transform.

  This class is part of the \a transform, \a pos, \a rot and \a scale
  compound.

  \see \ref comp_slots, TransformSlot, RotationSlot, ScaleSlot
 */
class PositionSlot : public Slot<vec3d>
{
  protected:
  TransformSlot* depctrl;

  public:
  PositionSlot() : Slot<vec3d>(), depctrl(0) { }
  ~PositionSlot();

  void setDependencyController(TransformSlot* ts) { depctrl = ts; }

  void computeValue();
  void setValue(const vec3d& val);
  void onValueChanged();
  void onDependentValueChanged();
};

/** 
  Rotation slot that accesses the rot part of a transform.

  This class is part of the \a transform, \a pos, \a rot and \a scale
  compound.

  \see \ref comp_slots, TransformSlot, PositionSlot, ScaleSlot
 */
class RotationSlot : public Slot<mat3d>
{
  protected:
  TransformSlot* depctrl;

  public:
  RotationSlot() : Slot<mat3d>(mat3d(1),0), depctrl(0) {}
  ~RotationSlot();

  void setDependencyController(TransformSlot* ts) { depctrl = ts; }

  void computeValue();
  void setValue(const mat3d& val);
  void onValueChanged();
  void onDependentValueChanged();
};

/** 
  Scale slot that accesses the scale part of a transform.

  This class is part of the \a transform, \a pos, \a rot and \a scale
  compound.

  \see \ref comp_slots, TransformSlot, PositionSlot, RotationSlot
 */
class ScaleSlot : public Slot<vec3d>
{
  protected:
  TransformSlot* depctrl;

  public:
  ScaleSlot() : Slot<vec3d>(vec3d(1),0), depctrl(0) {}
  ~ScaleSlot();

  void setDependencyController(TransformSlot* ts) { depctrl = ts; }

  void computeValue();
  void setValue(const vec3d& val);
  void onValueChanged();
  void onDependentValueChanged();
};

/**
  Transform slot and dependency controller.

  This slot is used for the \a transform attribute of a WorldObject.
  The individual parts of this matrix can be accessed via the \a pos,
  \a rot and \a scale slots.

  This class also serves as dependency controller for managing the
  interdependencies between \a transform, \a pos, \a rot and \a scale
  (see \ref interdeps).

  \see \ref comp_slots, PositionSlot, RotationSlot, ScaleSlot
 */
class TransformSlot : public Slot<mat4d>
{
  protected:
  PositionSlot* pos;
  RotationSlot* rot;
  ScaleSlot* scale;

  public:
  TransformSlot(PositionSlot* apos, RotationSlot* arot, ScaleSlot* ascale);
  virtual ~TransformSlot();

  void disconnectSlots();

  const mat4d& getValue();
  void setValue(const mat4d& val);
  void onValueChanged();
  void onDependentValueChanged();

  // Dependency controller methods:
  void getTransform();
  void getPos();
  void getRot();
  void getScale();

  void onTransformChanged();
  void onPosChanged();
  void onRotChanged();
  void onScaleChanged();

};

/**
  Base class for the geometric 3D scene.

  Everything that's part of the \em world (i.e. everything that has a
  3D position and orientation and that can usually be rendered) has 
  to be derived from this base class.

  This class contains the basic functionality and slots that are common
  to all world objects.

  \todo Support name changes of the children object (connect to their name slots)
 */
class WorldObject : public Component  //, public boost::enable_shared_from_this<WorldObject>
{
  public:
  // Slots
  PositionSlot pos;
  RotationSlot rot;
  ScaleSlot scale;
  TransformSlot transform;
  ProceduralSlot<mat4d, WorldObject> worldtransform;

  ProceduralSlot<vec3d, WorldObject> cog;
  ProceduralSlot<mat3d, WorldObject> inertiatensor;
  Slot<double> mass;
  ProceduralSlot<double, WorldObject> totalmass;
  Slot<bool> visible;

  Slot<vec3d> linearvel;
  Slot<vec3d> angularvel;

  WorldObject* parent;
//  boost::shared_ptr<WorldObject> parent;
  std::map<string, boost::shared_ptr<WorldObject> > childs;
  typedef std::map<string, boost::shared_ptr<WorldObject> >::iterator ChildIterator;
  typedef std::map<string, boost::shared_ptr<WorldObject> >::const_iterator ConstChildIterator;

  protected:
  /// Associated geometry.
  boost::shared_ptr<GeomObject> geom;
  /// Associated material.
  //boost::shared_ptr<Material> material;
  std::vector<boost::shared_ptr<Material> > materials;

  // worldtransform   Slot<mat4d>

  protected:
  /// This is a buffer for the return value of localTransform().
  mat4d _localTransform;
  /// The current offset transformation.
  mat4d _offsetTransform;
  /// The inverse of the current offset transformation.
  mat4d _inverseOffsetTransform;

  ////////////////////////////////////////
  public:
  WorldObject(string aname="");
  virtual ~WorldObject();

  virtual void setName(string aname);

  virtual BoundingBox boundingBox();

  const mat4d& localTransform();

  const mat4d& getOffsetTransform();
  void setOffsetTransform(const mat4d& ot);

  boost::shared_ptr<GeomObject> getGeom() const { return geom; }
  void setGeom(boost::shared_ptr<GeomObject> ageom);

  int getNumMaterials() const;
  void setNumMaterials(int num);
  boost::shared_ptr<Material> getMaterial(int idx=0) const;
  void setMaterial(boost::shared_ptr<Material> amaterial, int idx=0);

  /**
    Return the number of children world objects.

    \return Number of children
   */
  int lenChilds() const { return childs.size(); }

//  WorldObject* getParent() const { return parent; }

  bool hasChild(string name) const;
  boost::shared_ptr<WorldObject> child(string name);
  void addChild(boost::shared_ptr<WorldObject> child);
  void removeChild(boost::shared_ptr<WorldObject> child);
  void removeChild(string name);

  /**
    Return an iterator that points to the first children.
   */
  ChildIterator childsBegin() { return childs.begin(); }
  /**
    Return an iterator that points one after the last children.
   */
  ChildIterator childsEnd() { return childs.end(); }

  string makeChildNameUnique(string name) const;

  protected:
  void onRenameChild(const WorldObject& child, string newname);
  void computeCog(vec3d& cog);
  void computeInertiaTensor(mat3d& tensor);
  mat3d _translateI(const vec3d& oldcog, const vec3d& a);
  mat3d _rotateI(const mat3d& I, const mat3d& R);
  mat3d _crossmat(const vec3d& a);
  void computeTotalMass(double& massvalue);
  void computeWorldTransform(mat4d& WT);
};


}  // end of namespace

#endif
