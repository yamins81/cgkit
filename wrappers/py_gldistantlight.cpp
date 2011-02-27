/*
 GLDistantLight
 */

#include <boost/python.hpp>
#include "gldistantlight.h"

using namespace boost::python;
using namespace support3d;

// get/set for "ambient" property
vec3d getAmbient(GLDistantLight* self)
{
  return self->ambient.getValue();
}

void setAmbient(GLDistantLight* self, vec3d c)
{
  self->ambient.setValue(c);
}

// get/set for "diffuse" property
vec3d getDiffuse(GLDistantLight* self)
{
  return self->diffuse.getValue();
}

void setDiffuse(GLDistantLight* self, vec3d c)
{
  self->diffuse.setValue(c);
}

// get/set for "specular" property
vec3d getSpecular(GLDistantLight* self)
{
  return self->specular.getValue();
}

void setSpecular(GLDistantLight* self, vec3d c)
{
  self->specular.setValue(c);
}

void class_GLDistantLight()
{
  class_<GLDistantLight, bases<LightSource> >("GLDistantLight")
    .def(init<string>())
    .def_readonly("ambient_slot", &GLDistantLight::ambient)
    .add_property("ambient", &getAmbient, &setAmbient)
    .def_readonly("diffuse_slot", &GLDistantLight::diffuse)
    .add_property("diffuse", &getDiffuse, &setDiffuse)
    .def_readonly("specular_slot", &GLDistantLight::specular)
    .add_property("specular", &getSpecular, &setSpecular)
    .def("applyGL", &GLDistantLight::applyGL)
  ;
}
