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

#include <ctype.h>
#include <stdlib.h>
#include <sstream>
#include "worldobject.h"
#include "debuginfo.h"

namespace support3d {

PositionSlot::~PositionSlot()
{
//  std::cout<<"0x"<<std::hex<<(long)this<<std::dec<<": PositionSlot<T>::~PositionSlot() begin"<<std::endl;
//  std::cout<<std::hex<<(long)depctrl<<std::dec<<std::endl;
  if (depctrl!=0) depctrl->disconnectSlots();
//  std::cout<<"0x"<<std::hex<<(long)this<<std::dec<<": PositionSlot<T>::~PositionSlot() end"<<std::endl;
}

void PositionSlot::computeValue()
{
  if (depctrl!=0)
    depctrl->getPos();
}

void PositionSlot::setValue(const vec3d& val)
{
  // Check if the new value is really a *new* value
  if ((flags & CACHE_VALID) && value==val)
    return;

  if (controller!=0)
  {
    flags &= ~CACHE_VALID;
    controller->setValue(val);
  }
  else
  {
    flags |= CACHE_VALID;
    value = val;
    notifyDependents();
    if (depctrl!=0)
      depctrl->onPosChanged();
  }
}

void PositionSlot::onValueChanged()
{
  Slot<vec3d>::onValueChanged();
  if (depctrl!=0)
    depctrl->onPosChanged();
}

// This method is called whenever the transform value changes
void PositionSlot::onDependentValueChanged()
{
  flags &= ~CACHE_VALID;
  // Is there a controller set?
  if (controller!=0)
  {
    // ...then get the value and send it up to the controller
    if (depctrl!=0)
      depctrl->getPos();
    controller->setValue(value);
  }
  else
  {
    // no controller, then just notify the dependents about a value change
    notifyDependents();
  }
}


///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

RotationSlot::~RotationSlot()
{
//  std::cout<<"0x"<<std::hex<<(long)this<<std::dec<<": RotationSlot<T>::~RotationSlot() begin"<<std::endl;
//  std::cout<<std::hex<<(long)depctrl<<std::dec<<std::endl;
  if (depctrl!=0) depctrl->disconnectSlots();
//  std::cout<<"0x"<<std::hex<<(long)this<<std::dec<<": RotationSlot<T>::~RotationSlot() end"<<std::endl;
}


void RotationSlot::computeValue()
{
  if (depctrl!=0)
    depctrl->getRot();
}

void RotationSlot::setValue(const mat3d& val)
{
  // Check if the new value is really a *new* value
  if ((flags & CACHE_VALID) && value==val)
    return;

  if (controller!=0)
  {
    flags &= ~CACHE_VALID;
    controller->setValue(val);
  }
  else
  {
    flags |= CACHE_VALID;
    value = val;
    notifyDependents();
    if (depctrl!=0)
    {
      depctrl->onRotChanged();
    }
  }
}

void RotationSlot::onValueChanged()
{
  Slot<mat3d>::onValueChanged();
  if (depctrl!=0)
    depctrl->onRotChanged();
}

// This method is called whenever the transform value changes
void RotationSlot::onDependentValueChanged()
{
  flags &= ~CACHE_VALID;
  // Is there a controller set?
  if (controller!=0)
  {
    // ...then get the value and send it up to the controller
    if (depctrl!=0)
      depctrl->getRot();
    controller->setValue(value);
  }
  else
  {
    // no controller, then just notify the dependents about a value change
    notifyDependents();
  }
}


///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

ScaleSlot::~ScaleSlot()
{
//  std::cout<<"0x"<<std::hex<<(long)this<<std::dec<<": ScaleSlot<T>::~ScaleSlot() begin"<<std::endl;
//  std::cout<<std::hex<<(long)depctrl<<std::dec<<std::endl;
  if (depctrl!=0) depctrl->disconnectSlots();
//  std::cout<<"0x"<<std::hex<<(long)this<<std::dec<<": ScaleSlot<T>::~ScaleSlot() end"<<std::endl;
}


void ScaleSlot::computeValue()
{
  if (depctrl!=0)
    depctrl->getScale();
}

void ScaleSlot::setValue(const vec3d& val)
{
  // Check if the new value is really a *new* value
  if ((flags & CACHE_VALID) && value==val)
    return;

  if (controller!=0)
  {
    flags &= ~CACHE_VALID;
    controller->setValue(val);
  }
  else
  {
    flags |= CACHE_VALID;
    value = val;
    notifyDependents();
    if (depctrl!=0)
      depctrl->onScaleChanged();
  }
}

void ScaleSlot::onValueChanged()
{
  Slot<vec3d>::onValueChanged();
  if (depctrl!=0)
    depctrl->onScaleChanged();
}

// This method is called whenever the transform value changes
void ScaleSlot::onDependentValueChanged()
{
  flags &= ~CACHE_VALID;
  // Is there a controller set?
  if (controller!=0)
  {
    // ...then get the value and send it up to the controller
    if (depctrl!=0)
      depctrl->getScale();
    controller->setValue(value);
  }
  else
  {
    // no controller, then just notify the dependents about a value change
    notifyDependents();
  }
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

TransformSlot::TransformSlot(PositionSlot* apos, RotationSlot* arot, ScaleSlot* ascale)
 : Slot<mat4d>(), pos(apos), rot(arot), scale(ascale)
{
  setFlags(CACHE_VALID, false);
  pos->setDependencyController(this);
  rot->setDependencyController(this);
  scale->setDependencyController(this);
  // Initialize with the identity matrix
  setValue(mat4d(1));
}

TransformSlot::~TransformSlot()
{ 
//  std::cout<<"0x"<<std::hex<<(long)this<<std::dec<<": TransformSlot<T>::~TransformSlot() begin"<<std::endl;
  if (pos!=0)
  {
    disconnectSlots();
  }
//  std::cout<<"0x"<<std::hex<<(long)this<<std::dec<<": TransformSlot<T>::~TransformSlot() end"<<std::endl;
}

/**
  Break all connections between all involved slots.

  This is only called during destruction of the corresponding component.
 */
void TransformSlot::disconnectSlots()
{
  //std::cout<<"TransformSlot::disconnectSlots()"<<std::endl;
  pos->setDependencyController(0);
  rot->setDependencyController(0);
  scale->setDependencyController(0);
  pos = 0;
  rot = 0;
  scale = 0;
}

const mat4d& TransformSlot::getValue()
{
  // Is the value in the cache still valid?
  if (flags & CACHE_VALID)
    return value; 

  // Obtain a new value and store it in the cache
  if (controller!=0)
  {
    value = controller->getValue();
  }
//  computeValue();
  getTransform();

  flags |= CACHE_VALID;
  return value;
}

void TransformSlot::setValue(const mat4d& val)
{
  // Check if the new value is really a *new* value
  if ((flags & CACHE_VALID) && value==val)
    return;

  if (controller!=0)
  {
    flags &= ~CACHE_VALID;
    controller->setValue(val);
  }
  else
  {
    // Take the value...
    value = val;
    // ...but only mark the cache as valid if no sub slot has a controller set.
    // If there is a controller some parts of the matrix may be invalid.
    // The valid parts will be picked up anyway (even though the cache is
    // invalid) as by default the transform values dominate the sub slot 
    // values if noone has a controller).
    if (pos!=0 && (pos->controller!=0 || rot->controller!=0 || scale->controller!=0))
      flags &= ~CACHE_VALID;
    else
      flags |= CACHE_VALID;
    notifyDependents();
    onTransformChanged();
  }
}

void TransformSlot::onValueChanged()
{
  DEBUGINFO(this, "TransformSlot::onValueChanged()");
  Slot<mat4d>::onValueChanged();
  onTransformChanged();
}


/**
  Get the entire transformation as mat4 and store it in the slot value.

  This method is called from getValue().
 */
void TransformSlot::getTransform()
{
  DEBUGINFO(this, "TransformSlot::getTransform()");
  vec3d t;
  vec3d s;
  mat3d r;
  mat4d r4;
  int tests = 0;

  if (rot==0) return;

  // Has the pos slot a valid value?
  if ((pos->flags & CACHE_VALID) || (pos->controller!=0))
  {
    const vec3d& p = pos->getValue();
    value.setColumn(3, p.x, p.y, p.z, 1.0);
  }

  // Has the rot slot a valid value?
  if ((rot->flags & CACHE_VALID) || (rot->controller!=0))
  {
    tests = 0x01;
  }
  // Has the scale slot a valid value?
  if ((scale->flags & CACHE_VALID) || (scale->controller!=0))
  {
    tests |= 0x02;
  }

  switch(tests)
  {
    // Neither rot nor scale have a valid value, so keep the current value
    case 0: break;
    // Only rot has a valid value
    case 1: { 
              try
              {
                value.decompose(t,r4,s);
                r4.getMat3(r);
              }
              catch(...)
              {
              };
              r = rot->getValue();
              r.scale(s);
              value.setMat3(r);
	      break;
            }
    // Only scale has a valid value
    case 2: { 
              try
              {
                value.decompose(t,r4,s);
                r4.getMat3(r);
              }
              catch(...)
              {
              };
              s = scale->getValue();
              r.scale(s);
              value.setMat3(r);
	      break;
            }
    // Both have valid values
    case 3: { 
              r = rot->getValue();
              r.scale(scale->getValue());
              value.setMat3(r);
	      break;
            }
  };
}

/**
  Update the position slot value.

  This method is called by the getValue() method of the position slot,
  but only if it has no valid cache value or controller set.

  This method stores the updated value in the position cache (but doesn't
  set the CACHE_VALID flag).
 */
void TransformSlot::getPos()
{
  DEBUGINFO(this, "TransformSlot::getPos()");
  if (rot==0) return;

  // Has the transform slot a valid value?
  //  if ((flags & CACHE_VALID) || (controller!=0))
  /* 
     The above test is disabled as the method is only called when there's
     no better value anyway. And to deal with the situation when the
     transform slot has no controller but a sub slot has, we must take
     the value from the transform slot even when the cache is currently
     marked as invalid (which only indicates that *some* parts are invalid
     but others may be so we better take them).
  */

  const mat4d& m = getValue();
  double dummy;
  m.getColumn(3, pos->value.x, pos->value.y, pos->value.z, dummy);
}

/**
  Update the rotation slot value.

  This method is called by the getValue() method of the rotation slot,
  but only if it has no valid cache value or controller set.

  This method stores the updated value in the rotation cache (but doesn't
  set the CACHE_VALID flag).
 */
void TransformSlot::getRot()
{
  DEBUGINFO(this, "TransformSlot::getRot()");
  if (rot==0) return;

  // Has the transform slot a valid value?
  //  if ((flags & CACHE_VALID) || (controller!=0))
  /* 
     The above test is disabled as the method is only called when there's
     no better value anyway. And to deal with the situation when the
     transform slot has no controller but a sub slot has, we must take
     the value from the transform slot even when the cache is currently
     marked as invalid (which only indicates that *some* parts are invalid
     but others may be so we better take them).
  */

  const mat4d& m = getValue();
  vec3d t;
  mat4d r4;
  vec3d s;
  m.decompose(t, r4, s);
  r4.getMat3(rot->value);
}

/**
  Update the scale slot value.

  This method is called by the getValue() method of the scale slot,
  but only if it has no valid cache value or controller set.

  This method stores the updated value in the scale cache (but doesn't
  set the CACHE_VALID flag).

  \todo Ergebnis von decompose() aus rot/scale zwischenspeichern, so dass nicht 2x zerlegt werden muss.
  \todo Statt CACHE_VALID und controller selbst zu testen eine entsprechende Methode aufrufen 
        (damit könnte flags und controller wieder protected werden)
 */
void TransformSlot::getScale()
{
  DEBUGINFO(this, "TransformSlot::getScale()");
  if (scale==0) return;

  // Has the transform slot a valid value?
  //  if ((flags & CACHE_VALID) || (controller!=0))
  /* 
     The above test is disabled as the method is only called when there's
     no better value anyway. And to deal with the situation when the
     transform slot has no controller but a sub slot has, we must take
     the value from the transform slot even when the cache is currently
     marked as invalid (which only indicates that *some* parts are invalid
     but others may be so we better take them).
  */

  const mat4d& m = getValue();
  vec3d t;
  mat4d r4;
  m.decompose(t, r4, scale->value);

}

/**
  Notify the dependency controller that the transform slot has changed.

  This method has to be called whenever the value of the \a transform
  slot has changed externally so that the interdependent slots (pos,
  rot, scale) can be notified about this change.
 */
void TransformSlot::onTransformChanged()
{
  DEBUGINFO(this, "TransformSlot::onTransformChanged()");
  if (pos!=0)
  {
    pos->onDependentValueChanged();
    rot->onDependentValueChanged();
    scale->onDependentValueChanged();
  }
}

/**
  Notify the dependency controller that the pos slot has changed.

  This method has to be called whenever the value of the \a pos slot
  has changed externally so that the interdependent slots (transform) can be
  notified about this change.
 */
void TransformSlot::onPosChanged()
{
  DEBUGINFO(this, "TransformSlot::onPosChanged()");
  onDependentValueChanged();
}

/**
  Notify the dependency controller that the rot slot has changed.

  This method has to be called whenever the value of the \a rot slot
  has changed externally so that the interdependent slots (transform) can be
  notified about this change.
 */
void TransformSlot::onRotChanged()
{
  DEBUGINFO(this, "TransformSlot::onRotChanged()");
  onDependentValueChanged();
}

/**
  Notify the dependency controller that the scale slot has changed.

  This method has to be called whenever the value of the \a scale slot
  has changed externally so that the interdependent slots (transform) can be
  notified about this change.
 */
void TransformSlot::onScaleChanged()
{
  DEBUGINFO(this, "TransformSlot::onScaleChanged()");
  onDependentValueChanged();
}

// This method is called whenever the transform value changes
void TransformSlot::onDependentValueChanged()
{
  DEBUGINFO(this, "TransformSlot::onDependentValueChanged()");

  flags &= ~CACHE_VALID;
  // Is there a controller set?
  if (controller!=0)
  {
    // ...then get the value and send it up to the controller
    getTransform();
    controller->setValue(value);
  }
  else
  {
    // no controller, then just notify the dependents about a value change
    notifyDependents();
  }
}

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////


WorldObject::WorldObject(string aname)
  : Component(aname), 
    pos(), rot(), scale(),
    transform(&pos, &rot, &scale),
    worldtransform(),
    cog(), inertiatensor(), mass(), totalmass(),  
    visible(true, 0),
    linearvel(), angularvel(),
    parent(0), childs(), geom(), materials(), 
    _localTransform(1),
    _offsetTransform(1), _inverseOffsetTransform(1)
{
  DEBUGINFO1(this, "WorldObject::WorldObject(\"%s\")", aname.c_str());

  cog.setProcedure(this, &WorldObject::computeCog);
  inertiatensor.setProcedure(this, &WorldObject::computeInertiaTensor);
  totalmass.setProcedure(this, &WorldObject::computeTotalMass);
  mass.addDependent(&totalmass);
  mass.addDependent(&inertiatensor);
  // Actually the cog is only dependent on the mass if the object has children,
  // otherwise the mass cancels out.
  mass.addDependent(&cog);

  worldtransform.setProcedure(this, &WorldObject::computeWorldTransform);
  // Actually the worldtransform is dependent on L, but as L is no slot we
  // use T instead (this might lead to some unnecessary re-calculations of
  // the worldtransform because L might not have changed if T changes,
  // but so what...)
  transform.addDependent(&worldtransform);

  addSlot("transform", transform);
  addSlot("pos", pos);
  addSlot("rot", rot);
  addSlot("scale", scale);
  addSlot("cog", cog);
  addSlot("inertiatensor", inertiatensor);
  addSlot("mass", mass);
  addSlot("totalmass", totalmass);
  addSlot("worldtransform", worldtransform);
  addSlot("visible", visible);
  addSlot("linearvel", linearvel);
  addSlot("angularvel", angularvel);

  setNumMaterials(1);
}

WorldObject::~WorldObject()
{
  DEBUGINFO(this, "WorldObject::~WorldObject()  (\""+name+"\")");

  // Remove from the parent...
  if (parent!=0)
  {
    parent->removeChild(getName());
  }

  // Remove children objects (which will reset their parent attribute)...
  while(childs.size()>0)
  {
    removeChild(childsBegin()->second);
  }

  // Remove the geom object so that any established dependency is removed
  setGeom(boost::shared_ptr<GeomObject>());

  transform.removeDependent(&worldtransform);

  // Remove dependency between mass and inertiatensor
  mass.removeDependent(&inertiatensor);
  mass.removeDependent(&cog);
  mass.removeDependent(&totalmass);
}

void WorldObject::setName(string aname)
{
  // Is the new name identical with the current name? Then do nothing.
  if (name==aname)
    return;

  // Check if the new name doesn't already exist
  if (parent!=0)
  {
    parent->onRenameChild(*this, aname);
  }

  // Set the new name
  Component::setName(aname);
}

/**
  Return the local axis aligned bounding box.

  The bounding box is given with respect to the local 
  transformation L (which is \em not what you get from
  the transform slot of the world object).

  \return Local bounding box.
*/
BoundingBox WorldObject::boundingBox()
{
  BoundingBox res;
  BoundingBox childbb;
  
  // Begin with the bounding box of the geom in this object
  if (geom.get()!=0)
  {
    res = geom->boundingBox();
  }

  // Take the children into account...
  for(ChildIterator it=childsBegin(); it!=childsEnd(); it++)
  {
    childbb = it->second->boundingBox();
    if (!childbb.isEmpty())
    {
      const mat4d& L = it->second->localTransform();
      childbb.transform(L, childbb);
      res.addBoundingBox(childbb);
    }
  }
  return res;
}

/**
  Return the local transformation that has to be used for rendering.

  The returned transformation L is calculated as follows:

  L = T*P^-1

  where T is the current transform (taken from the transform slot) and 
  P is the offset transform.

  \return Local transformation
  \todo Optimierung: Matrixmultiplikation nur dann, wenn sich T geändert hat.
  */
const mat4d& WorldObject::localTransform()
{
  _localTransform = transform.getValue();
  _localTransform *= _inverseOffsetTransform;
  return _localTransform;
}

/**
  Return the current offset transformation.

  This transformation is given relative to the local object transformation.

  \return Offset transformation.
  \see setOffsetTransform
 */
const mat4d& WorldObject::getOffsetTransform()
{
  return _offsetTransform;
}

/**
  Set the offset transformation.

  The transformation has to be given relative to the local object transformation.

  \param ot  New Offset transformation.
  \see getOffsetTransform
 */
void WorldObject::setOffsetTransform(const mat4d& ot)
{
  try
  {
    // Get the local transformation L
    const mat4d& L = localTransform();

    // Try to invert the input offset matrix. If it can't be inverted
    // an exception is thrown which is catched later and nothing
    // has been changed internally.
    ot.inverse(_inverseOffsetTransform);

    // Store the offset transform
    _offsetTransform = ot;
    // Update the transform slot
    transform.setValue(L*_offsetTransform);

    // Invalidate the cog and inertiatensor slots
    cog.onValueChanged();
    inertiatensor.onValueChanged();
  }
  catch(...)
  {
  }
}

/**
  Set a new geom object.

  Dependencies are established if the geom has a cog and a inertiatensor slot.

  \param ageom The new geom object to set (or an empty pointer).
 */
void WorldObject::setGeom(boost::shared_ptr<GeomObject> ageom)
{
  // Remove dependency from the previous geom...
  if (geom.get()!=0)
  {
    if (geom->hasSlot("cog"))
    {
      ISlot& cogslot = geom->slot("cog");
      cogslot.removeDependent(&cog);
    }
    if (geom->hasSlot("inertiatensor"))
    {
      ISlot& itslot = geom->slot("inertiatensor");
      itslot.removeDependent(&inertiatensor);
    }
  }

  geom = ageom;

  // Establish the cog and inertiatensor dependencies...
  if (geom.get()!=0)
  {
    if (geom->hasSlot("cog"))
    {
      ISlot& cogslot = geom->slot("cog");
      cogslot.addDependent(&cog);
    }
    if (geom->hasSlot("inertiatensor"))
    {
      ISlot& itslot = geom->slot("inertiatensor");
      itslot.addDependent(&inertiatensor);
    }
  }
}

/**
  Return the current size of the material array.

  \return Number of materials
 */
int WorldObject::getNumMaterials() const
{
  return materials.size();
}

/**
  Set a new size for the material array.

  \param num New number of materials
 */
void WorldObject::setNumMaterials(int num)
{
  if (num<1)
    num=1;
  materials.resize(num);
}


/**
  Get a stored material.

  The method returns an empty pointer if the given index \a idx is out of range 
  or there is no material stored at that position.

  \param idx The index of the material (can be negative to count from the end of the list)
  \return The material object
 */
boost::shared_ptr<Material> WorldObject::getMaterial(int idx) const
{
  if (idx<0)
    idx = materials.size()+idx;
  if ((idx<0) || (idx>=int(materials.size())))
    return boost::shared_ptr<Material>();

  return materials[idx];
}

/**
  Set a new material.

  A EIndexError exception is thrown if the index is out of range.

  \param amaterial The new material object
  \param idx The index of the material (can be negative to count from the end of the list)
 */
void WorldObject::setMaterial(boost::shared_ptr<Material> amaterial, int idx)
{
  if (idx<0)
    idx = materials.size()+idx;
  if ((idx<0) || (idx>=int(materials.size())))
    throw EIndexError();

  materials[idx] = amaterial;
}

/**
  Check if a children with a particular name does exist.

  \param name The name of the children
  \return True if the name exists
 */
bool WorldObject::hasChild(string name) const
{
  return childs.find(name)!=childs.end();
}

/**
  Return the children with the given name.

  A EKeyError exception is thrown if there's no children with the specified name.

  \param name The name of the children.
 */
boost::shared_ptr<WorldObject> WorldObject::child(string name)
{
  ConstChildIterator it = childs.find(name);
  if (it==childs.end())
  {
    throw EKeyError("Object \""+getName()+"\" has no children \""+name+"\".");
  }
  else
  {
    return it->second;
  }
}

/**
  Add a new children world object to this object.

  An EValueError exception is thrown if \a child was already added to another object.
  In this case you have to remove the object from its previous parent yourself.
  You also have to make sure that the name of \a child is unique among the children
  of this object, otherwise an EKeyError exception is thrown.

  \param child Children world object
 */
void WorldObject::addChild(boost::shared_ptr<WorldObject> child)
{
  // Was child already added to another object?
  if (child->parent!=0)
  {
    throw EValueError("Object \""+child->getName()+"\" is already a children of object \""+child->parent->getName()+"\".");
  }
  // Is there already a children with the same name?
  if (childs.find(child->getName())!=childs.end())
  {
    throw EKeyError("Object \""+getName()+"\" already has a children called \""+child->getName()+"\".");
  }
  childs[child->getName()] = child;
  child->parent = this;

  // Create the worldtransform dependency
  worldtransform.addDependent(&child->worldtransform);

  // Create cog/inertiatensor/totalmass dependencies
  child->mass.addDependent(&cog);
  child->totalmass.addDependent(&totalmass);
  child->cog.addDependent(&cog);
  child->cog.addDependent(&inertiatensor);
  child->inertiatensor.addDependent(&inertiatensor);
  child->transform.addDependent(&cog);
  child->transform.addDependent(&inertiatensor);
}

/**
  Remove a children world object from this object.

  A EKeyError exception is thrown if \a child is not a children of this object.

  \param child Children world object
 */
void WorldObject::removeChild(boost::shared_ptr<WorldObject> child)
{
  ChildIterator it = childs.find(child->getName());
  if (it==childs.end())
  {
    throw EKeyError("Object \""+getName()+"\" has no children called \""+child->getName()+"\".");
  }
  child->parent = 0;
  childs.erase(child->getName());
  // Remove the worldtransform dependency
  worldtransform.removeDependent(&child->worldtransform);
  // Remove cog/inertiatensor/totalmass dependencies
  child->mass.removeDependent(&cog);
  child->totalmass.removeDependent(&totalmass);
  child->cog.removeDependent(&cog);
  child->cog.removeDependent(&inertiatensor);
  child->inertiatensor.removeDependent(&inertiatensor);
  child->transform.removeDependent(&cog);
  child->transform.removeDependent(&inertiatensor);
}

/**
  Remove a children world object from this object.

  A EKeyError exception is thrown if there is no children with the specified name.

  \param name Name of the children world object
 */
void WorldObject::removeChild(string name)
{
  ChildIterator it = childs.find(name);
  if (it==childs.end())
  {
    throw EKeyError("Object \""+getName()+"\" has no children called \""+name+"\".");
  }
  boost::shared_ptr<WorldObject> child = it->second;
  child->parent = 0;
  childs.erase(name);
  // Remove the worldtransform dependency
  worldtransform.removeDependent(&child->worldtransform);
  // Remove cog/inertiatensor dependencies
  child->mass.removeDependent(&cog);
  child->totalmass.removeDependent(&totalmass);
  child->cog.removeDependent(&cog);
  child->cog.removeDependent(&inertiatensor);
  child->inertiatensor.removeDependent(&inertiatensor);
  child->transform.removeDependent(&cog);
  child->transform.removeDependent(&inertiatensor);
}

/**
  Modify a name so that it's unique among the children names.

  If \a name is already the name of a children object, then it's modified
  by adding/increasing a trailing number, otherwise it's returned unchanged.

  \param name A name
  \return The modified name that is unique
 */
string WorldObject::makeChildNameUnique(string name) const
{
  string base;
  long num;
  int numpos = name.size();
  std::ostringstream resstr;
  string res;

  // Determine the position where a trailing number begins
  while((numpos>0) && isdigit(name[numpos-1]))
  {
    numpos--;
  }

  // Copy the first part of the name (without the trailing number)
  base = name.substr(0, numpos);

  // Convert the trailing number to a long
  num = atol(name.substr(numpos, name.size()-numpos).c_str()); 

  // Increase the number until the name is unique
  // (but start with the original name (that might have no number at all))
  res = name;
  while(hasChild(res))
  {
    num++;
    resstr.str("");
    resstr<<base<<num;
    res = resstr.str();
  }

  return res;
}

/**
  Notify a parent about an attempt to rename a child.

  This method is called in the setName() method of a children object. The call
  has to be made \em before the actual name change. If the name isn't accepted by
  the parent (because it would result in duplicate names), an exception is thrown
  in which case the name must not be changed by the child.

  If this method does not throw an exception it updates its own data to reflect
  the name change, so the child \em must set the new name after calling this method.

  \pre newname != child.getName()
  \param child Children object
  \param newname The new name of the child
 */
void WorldObject::onRenameChild(const WorldObject& child, string newname)
{
  if (hasChild(newname))
  {
    throw EValueError("Object \""+getName()+"\" already has a children called \""+newname+"\".");
  }
  
  boost::shared_ptr<WorldObject> c = childs.find(child.getName())->second;
  childs.erase(child.getName());
  childs[newname] = c;
}


/**
  Compute the center of gravity.

  This is the callback for the cog procedural slot.

  The computed center of gravity is given in the pivot coordinate system.

  Only children objects whose \em dynamics attribute is true are taken
  into account.

  \see ProceduralSlot
 */
void WorldObject::computeCog(vec3d& cog)
{
  // Does the current geom object have a "cog" slot?
  if ((geom.get()!=0) && (geom->hasSlot("cog")))
  {
    ISlot& slot = geom->slot("cog");
    slot.getVal(cog);
    cog = _inverseOffsetTransform*cog;
  }
  else
  {
    // No geom or it doesn't have a "cog" slot...
    cog = vec3d(0,0,0);
  }

  // Take the children into account...
  if (lenChilds()>0)
  {
    double totalmass = mass.getValue();
    cog *= totalmass;

    for(ChildIterator it=childsBegin(); it!=childsEnd(); it++)
    {
      bool dynamics = false;
      // No dynamics slot? Then this is no rigid body anyway
      if (!it->second->hasSlot("dynamics"))
	continue;
      // Only take children into account whose dynamics attribute is true...
      it->second->slot("dynamics").getVal(dynamics);
      if (!dynamics)
	continue;

      double m = it->second->mass.getValue();
      vec3d childcog = it->second->cog.getValue();
      mat4d childtransform = it->second->transform.getValue();
      totalmass += m;
      cog += m*(_inverseOffsetTransform*childtransform*childcog);
    }
    if (totalmass>vec3d::epsilon)
    {
      cog/=totalmass;
    }
  }
}

/**
  Compute the inertia tensor.

  This is the callback for the inertia tensor procedural slot.

  Only children objects whose \em dynamics attribute is true are taken
  into account.

  \see ProceduralSlot
 */
void WorldObject::computeInertiaTensor(mat3d& tensor)
{
  if ((geom.get()!=0) && (geom->hasSlot("inertiatensor")))
  {
    // Get the tensor from the geom
    ISlot& slot = geom->slot("inertiatensor");
    slot.getVal(tensor);

    // Rotate
    tensor = _rotateI(tensor, _inverseOffsetTransform.getMat3());

    // Translate
    vec3d cog;
    vec4d c4;
    _inverseOffsetTransform.getColumn(3, c4);
    vec3d a(c4.x, c4.y, c4.z);
    ISlot& cogslot = geom->slot("cog");
    cogslot.getVal(cog);
    tensor += _translateI(cog, a);

    // Adjust for mass
    tensor *= mass.getValue();
  }
  else
  {
    tensor = mat3d(0);
  }

  // Take the children into account...
  if (lenChilds()>0)
  {
    for(ChildIterator it=childsBegin(); it!=childsEnd(); it++)
    {
      bool dynamics = false;
      // No dynamics slot? Then this is no rigid body anyway
      if (!it->second->hasSlot("dynamics"))
	continue;
      // Only take children into account whose dynamics attribute is true...
      it->second->slot("dynamics").getVal(dynamics);
      if (!dynamics)
	continue;

      mat3d childI = it->second->inertiatensor.getValue();
      mat4d childT = _inverseOffsetTransform*it->second->transform.getValue();

      // Rotate
      childI = _rotateI(childI, childT.getMat3());

      // Translate
      vec3d cog = it->second->cog.getValue();
      vec4d c4;
      childT.getColumn(3, c4);
      vec3d t(c4.x, c4.y, c4.z);
      childI += _translateI(cog, t);
      
      // Add
      tensor += childI;
    }
  }
}

mat3d WorldObject::_translateI(const vec3d& oldcog, const vec3d& a)
{
  mat3d chat = _crossmat(oldcog);
  mat3d ahat = _crossmat(oldcog+a);
  chat = chat*chat;
  ahat = ahat*ahat;
  return chat-ahat;
}

mat3d WorldObject::_rotateI(const mat3d& I, const mat3d& R)
{
  return R*I*R.transpose();
}

mat3d WorldObject::_crossmat(const vec3d& a)
{
  mat3d res(0);

  res.at(0,1) = -a.z;
  res.at(0,2) = a.y;
  res.at(1,0) = a.z;
  res.at(1,2) = -a.x;
  res.at(2,0) = -a.y;
  res.at(2,1) = a.x;
  return res;
}


/**
  Compute the total mass of this object.

  This is the callback for the totalmass procedural slot.

  \see ProceduralSlot
 */
void WorldObject::computeTotalMass(double& massvalue)
{
  // Start with the mass of the object itself...
  massvalue = mass.getValue();

  // ...and add the mass of the  children...
  for(ChildIterator it=childsBegin(); it!=childsEnd(); it++)
  {
    massvalue += it->second->totalmass.getValue();
  }
}


/**
  Compute the world transform of this object.

  The result is the concatenation of all local transformations L.
  The offset transform is not taken into account.

  This is the callback for the worldtransform procedural slot.

  \see ProceduralSlot
 */
void WorldObject::computeWorldTransform(mat4d& WT)
{
  // Start with the local transform of the object itself...
  WT = localTransform();

  // ...and walk up the hierarchy
  WorldObject* p = parent;
  while(p!=0)
  {
    WT = p->localTransform()*WT;
    p = p->parent;
  }
}

}  // end of namespace
