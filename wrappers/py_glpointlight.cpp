/*
 GLPointLight
 */

#include <boost/python.hpp>
#include "glpointlight.h"

using namespace boost::python;
using namespace support3d;

// get/set for "ambient" property
vec3d getAmbient(GLPointLight* self)
{
  return self->ambient.getValue();
}

void setAmbient(GLPointLight* self, vec3d c)
{
  self->ambient.setValue(c);
}

// get/set for "diffuse" property
vec3d getDiffuse(GLPointLight* self)
{
  return self->diffuse.getValue();
}

void setDiffuse(GLPointLight* self, vec3d c)
{
  self->diffuse.setValue(c);
}

// get/set for "specular" property
vec3d getSpecular(GLPointLight* self)
{
  return self->specular.getValue();
}

void setSpecular(GLPointLight* self, vec3d c)
{
  self->specular.setValue(c);
}

// get/set for "constant_attenuation" property
double getConstantAttenuation(GLPointLight* self)
{
  return self->constant_attenuation.getValue();
}

void setConstantAttenuation(GLPointLight* self, double a)
{
  self->constant_attenuation.setValue(a);
}

// get/set for "linear_attenuation" property
double getLinearAttenuation(GLPointLight* self)
{
  return self->linear_attenuation.getValue();
}

void setLinearAttenuation(GLPointLight* self, double a)
{
  self->linear_attenuation.setValue(a);
}

// get/set for "quadratic_attenuation" property
double getQuadraticAttenuation(GLPointLight* self)
{
  return self->quadratic_attenuation.getValue();
}

void setQuadraticAttenuation(GLPointLight* self, double a)
{
  self->quadratic_attenuation.setValue(a);
}


void class_GLPointLight()
{
  class_<GLPointLight, bases<LightSource> >("GLPointLight")
    .def(init<string>())
    .def_readonly("ambient_slot", &GLPointLight::ambient)
    .add_property("ambient", &getAmbient, &setAmbient)
    .def_readonly("diffuse_slot", &GLPointLight::diffuse)
    .add_property("diffuse", &getDiffuse, &setDiffuse)
    .def_readonly("specular_slot", &GLPointLight::specular)
    .add_property("specular", &getSpecular, &setSpecular)
    .def_readonly("constant_attenuation_slot", &GLPointLight::constant_attenuation)
    .add_property("constant_attenuation", &getConstantAttenuation, &setConstantAttenuation)
    .def_readonly("linear_attenuation_slot", &GLPointLight::linear_attenuation)
    .add_property("linear_attenuation", &getLinearAttenuation, &setLinearAttenuation)
    .def_readonly("quadratic_attenuation_slot", &GLPointLight::quadratic_attenuation)
    .add_property("quadratic_attenuation", &getQuadraticAttenuation, &setQuadraticAttenuation)
    .def("applyGL", &GLPointLight::applyGL)
  ;
}
