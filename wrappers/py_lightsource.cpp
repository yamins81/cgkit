/*
 LightSource
 */

#include <boost/python.hpp>
#include "lightsource.h"


using namespace boost::python;
using namespace support3d;

// get/set for "enabled" property
bool getEnabled(LightSource* self)
{
  return self->enabled.getValue();
}

void setEnabled(LightSource* self, bool e)
{
  self->enabled.setValue(e);
}

// get/set for "enabled" property
bool getCastShadow(LightSource* self)
{
  return self->cast_shadow.getValue();
}

void setCastShadow(LightSource* self, bool s)
{
  self->cast_shadow.setValue(s);
}

// get/set for "intensity" property
double getIntensity(LightSource* self)
{
  return self->intensity.getValue();
}

void setIntensity(LightSource* self, double v)
{
  self->intensity.setValue(v);
}

void class_LightSource()
{
  class_<LightSource, bases<WorldObject>, boost::noncopyable >("LightSource")
    .def(init<string>())
    .def_readonly("enabled_slot", &LightSource::enabled)
    .add_property("enabled", &getEnabled, &setEnabled)
    .def_readonly("cast_shadow_slot", &LightSource::cast_shadow)
    .add_property("cast_shadow", &getCastShadow, &setCastShadow)
    .def_readonly("intensity_slot", &LightSource::intensity)
    .add_property("intensity", &getIntensity, &setIntensity)
    .def("isShadowCaster", &LightSource::isShadowCaster)
    .def("isShadowReceiver", &LightSource::isShadowReceiver)
  ;
}
