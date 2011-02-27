/*
 GLSpotLight
 */

#include <boost/python.hpp>
#include "glspotlight.h"

using namespace boost::python;
using namespace support3d;

// get/set for "ambient" property
vec3d getAmbient(GLSpotLight* self)
{
  return self->ambient.getValue();
}

void setAmbient(GLSpotLight* self, vec3d c)
{
  self->ambient.setValue(c);
}

// get/set for "diffuse" property
vec3d getDiffuse(GLSpotLight* self)
{
  return self->diffuse.getValue();
}

void setDiffuse(GLSpotLight* self, vec3d c)
{
  self->diffuse.setValue(c);
}

// get/set for "specular" property
vec3d getSpecular(GLSpotLight* self)
{
  return self->specular.getValue();
}

void setSpecular(GLSpotLight* self, vec3d c)
{
  self->specular.setValue(c);
}

// get/set for "constant_attenuation" property
double getConstantAttenuation(GLSpotLight* self)
{
  return self->constant_attenuation.getValue();
}

void setConstantAttenuation(GLSpotLight* self, double a)
{
  self->constant_attenuation.setValue(a);
}

// get/set for "linear_attenuation" property
double getLinearAttenuation(GLSpotLight* self)
{
  return self->linear_attenuation.getValue();
}

void setLinearAttenuation(GLSpotLight* self, double a)
{
  self->linear_attenuation.setValue(a);
}

// get/set for "quadratic_attenuation" property
double getQuadraticAttenuation(GLSpotLight* self)
{
  return self->quadratic_attenuation.getValue();
}

void setQuadraticAttenuation(GLSpotLight* self, double a)
{
  self->quadratic_attenuation.setValue(a);
}

// get/set for "exponent" property
double getExponent(GLSpotLight* self)
{
  return self->exponent.getValue();
}

void setExponent(GLSpotLight* self, double e)
{
  self->exponent.setValue(e);
}

// get/set for "cutoff" property
double getCutoff(GLSpotLight* self)
{
  return self->cutoff.getValue();
}

void setCutoff(GLSpotLight* self, double e)
{
  self->cutoff.setValue(e);
}


void class_GLSpotLight()
{
  class_<GLSpotLight, bases<LightSource> >("GLSpotLight")
    .def(init<string>())
    .def_readonly("ambient_slot", &GLSpotLight::ambient)
    .add_property("ambient", &getAmbient, &setAmbient)
    .def_readonly("diffuse_slot", &GLSpotLight::diffuse)
    .add_property("diffuse", &getDiffuse, &setDiffuse)
    .def_readonly("specular_slot", &GLSpotLight::specular)
    .add_property("specular", &getSpecular, &setSpecular)
    .def_readonly("constant_attenuation_slot", &GLSpotLight::constant_attenuation)
    .add_property("constant_attenuation", &getConstantAttenuation, &setConstantAttenuation)
    .def_readonly("linear_attenuation_slot", &GLSpotLight::linear_attenuation)
    .add_property("linear_attenuation", &getLinearAttenuation, &setLinearAttenuation)
    .def_readonly("quadratic_attenuation_slot", &GLSpotLight::quadratic_attenuation)
    .add_property("quadratic_attenuation", &getQuadraticAttenuation, &setQuadraticAttenuation)
    .def_readonly("exponent_slot", &GLSpotLight::exponent)
    .add_property("exponent", &getExponent, &setExponent)
    .def_readonly("cutoff_slot", &GLSpotLight::cutoff)
    .add_property("cutoff", &getCutoff, &setCutoff)
    .def("applyGL", &GLSpotLight::applyGL)
  ;
}
