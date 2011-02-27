/*
  WorldObject wrapper
 */

#include "py_worldobject.h"


// get/set for "transform" property
mat4d getTransform(WorldObject* self)
{
  return self->transform.getValue();
}

void setTransform(WorldObject* self, mat4d m)
{
  self->transform.setValue(m);
}

// get/set for "pos" property
vec3d getPos(WorldObject* self)
{
  return self->pos.getValue();
}

void setPos(WorldObject* self, vec3d p)
{
  self->pos.setValue(p);
}

// get/set for "rot" property
mat3d getRot(WorldObject* self)
{
  return self->rot.getValue();
}

void setRot(WorldObject* self, mat3d r)
{
  self->rot.setValue(r);
}

// get/set for "scale" property
vec3d getScale(WorldObject* self)
{
  return self->scale.getValue();
}

void setScale(WorldObject* self, vec3d s)
{
  self->scale.setValue(s);
}

// get/set for "pivot" property
vec3d getPivot(WorldObject* self)
{
  vec3d res;
  double dummy;
  const mat4d& offset = self->getOffsetTransform();
  offset.getColumn(3, res.x, res.y, res.z, dummy);
  return res;
}

void setPivot(WorldObject* self, vec3d p)
{
  const mat4d& offset = self->getOffsetTransform();
  mat4d new_offset = offset;
  new_offset.setColumn(3, p.x, p.y, p.z, 1.0);
  self->setOffsetTransform(new_offset);
}

// get/set for "cog" property
vec3d getCog(WorldObject* self)
{
  return self->cog.getValue();
}

// get/set for "inertiatensor" property
mat3d getInertiaTensor(WorldObject* self)
{
  return self->inertiatensor.getValue();
}

// get/set for "mass" property
double getMass(WorldObject* self)
{
  return self->mass.getValue();
}

void setMass(WorldObject* self, double m)
{
  self->mass.setValue(m);
}

// get for "totalmass" property
double getTotalMass(WorldObject* self)
{
  return self->totalmass.getValue();
}

// get for "worldtransform" property
mat4d getWorldTransform(WorldObject* self)
{
  return self->worldtransform.getValue();
}

// get/set for "visible" property
bool getVisible(WorldObject* self)
{
  return self->visible.getValue();
}

void setVisible(WorldObject* self, bool v)
{
  self->visible.setValue(v);
}

// get/set for "linearvel" property
vec3d getLinearVel(WorldObject* self)
{
  return self->linearvel.getValue();
}

void setLinearVel(WorldObject* self, const vec3d& v)
{
  self->linearvel.setValue(v);
}

// get/set for "angularvel" property
vec3d getAngularVel(WorldObject* self)
{
  return self->angularvel.getValue();
}

void setAngularVel(WorldObject* self, const vec3d& v)
{
  self->angularvel.setValue(v);
}

// __eq__
bool WorldObject__eq__(WorldObject* self, const WorldObject* other)
{
  return self==other;
}

bool WorldObject__ne__(WorldObject* self, const WorldObject* other)
{
  return self!=other;
}


//class_<WorldObject, bases<Component> > WorldObject_class("WorldObject", 
//                                            init< optional<std::string> >());


// forward declarations
void class_WorldObject2(class_<WorldObject, bases<Component> >& WorldObject_class);
void class_WorldObjectSlots();


// Class
void class_WorldObject()
{

  // WorldObject
  //  WorldObject_class
  class_<WorldObject, bases<Component> > WorldObject_class("WorldObject", 
					      init< optional<std::string> >());

  WorldObject_class
    .add_property("geom", &WorldObject::getGeom, &WorldObject::setGeom)
    //    .add_property("material", &WorldObject::getMaterial, &WorldObject::setMaterial)
    .add_property("parent", make_getter(&WorldObject::parent, return_internal_reference<>()))
    //    .def("getParent", &WorldObject::getParent, return_internal_reference<>())
    .add_property("transform", &getTransform, &setTransform)
    .add_property("worldtransform", &getWorldTransform)
    .add_property("pos", &getPos, &setPos)
    .add_property("rot", &getRot, &setRot)
    .add_property("scale", &getScale, &setScale)
    .add_property("pivot", &getPivot, &setPivot)
    .add_property("cog", &getCog)
    .add_property("inertiatensor", &getInertiaTensor)
    .add_property("mass", &getMass, &setMass)
    .add_property("totalmass", &getTotalMass)
    .add_property("visible", &getVisible, &setVisible)
    .add_property("linearvel", &getLinearVel, &setLinearVel)
    .add_property("angularvel", &getAngularVel, &setAngularVel)
    .def_readonly("transform_slot", &WorldObject::transform)
    .def_readonly("pos_slot", &WorldObject::pos)
    .def_readonly("rot_slot", &WorldObject::rot)
    .def_readonly("scale_slot", &WorldObject::scale)
    .def_readonly("cog_slot", &WorldObject::cog)
    .def_readonly("inertiatensor_slot", &WorldObject::inertiatensor)
    .def_readonly("mass_slot", &WorldObject::mass)
    .def_readonly("totalmass_slot", &WorldObject::totalmass)
    .def_readonly("worldtransform_slot", &WorldObject::worldtransform)
    .def_readonly("visible_slot", &WorldObject::visible)
    .def_readonly("linearvel_slot", &WorldObject::linearvel)
    .def_readonly("angularvel_slot", &WorldObject::angularvel)

    .def("__eq__", WorldObject__eq__)
    .def("__ne__", WorldObject__ne__)

    .def("localTransform", &WorldObject::localTransform,
	 return_value_policy<copy_const_reference>(),
	 "localTransform() -> mat4\n\n"
	 "Return the local transformation that has to be used for rendering.\n"
	 "The returned transformation L is calculated as follows: L = T*P^-1\n"
	 "where T is the current transform (taken from the transform slot)\n"
	 "and P is the offset transform.")
    ;

  class_WorldObject2(WorldObject_class);
}
