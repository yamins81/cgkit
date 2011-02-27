/*
  Lib3dsLight wrapper.
 */

#include <boost/python.hpp>
#include <iostream>
#include <string>
#include <lib3ds/file.h>
#include <lib3ds/camera.h>
#include <lib3ds/light.h>
#include <lib3ds/material.h>
#include <lib3ds/mesh.h>
#include <lib3ds/node.h>

#include "common_funcs.h"

using namespace boost::python;

//////////////////////////////////////////////////////////////////////
// Light methods
//////////////////////////////////////////////////////////////////////

Lib3dsLight* Light_getNext(Lib3dsLight* self) { return self->next; }
std::string Light_getName(Lib3dsLight* self) { return std::string(self->name); }
support3d::vec3d Light_getPos(Lib3dsLight* self) { return vector2vec3(self->position); }
support3d::vec3d Light_getSpot(Lib3dsLight* self) { return vector2vec3(self->spot); }
support3d::vec3d Light_getColor(Lib3dsLight* self) { return color2vec3(self->color); }
std::string Light_getProjector(Lib3dsLight* self) { return std::string(self->projector); }



//////////////////////////////////////////////////////////////////////

void class_Lib3dsLight()
{
  // Libd3dsLight
  class_<Lib3dsLight>("Lib3dsLight")
    .def_readonly("spot_light", &Lib3dsLight::spot_light)
    .def_readonly("see_cone", &Lib3dsLight::see_cone)
    .def_readonly("roll", &Lib3dsLight::roll)
    .def_readonly("off", &Lib3dsLight::off)
    .def_readonly("outer_range", &Lib3dsLight::outer_range)
    .def_readonly("inner_range", &Lib3dsLight::inner_range)
    .def_readonly("multiplier", &Lib3dsLight::multiplier)
    .def_readonly("attenuation", &Lib3dsLight::attenuation)
    .def_readonly("rectangular_spot", &Lib3dsLight::rectangular_spot)
    .def_readonly("shadowed", &Lib3dsLight::shadowed)
    .def_readonly("shadow_bias", &Lib3dsLight::shadow_bias)
    .def_readonly("shadow_filter", &Lib3dsLight::shadow_filter)
    .def_readonly("shadow_size", &Lib3dsLight::shadow_size)
    .def_readonly("spot_aspect", &Lib3dsLight::spot_aspect)
    .def_readonly("use_projector", &Lib3dsLight::use_projector)
    .add_property("projector", Light_getProjector)
    .def_readonly("spot_overshoot", &Lib3dsLight::spot_overshoot)
    .def_readonly("ray_shadows", &Lib3dsLight::ray_shadows)
    .def_readonly("ray_bias", &Lib3dsLight::ray_bias)
    .def_readonly("hotspot", &Lib3dsLight::hot_spot)
    .def_readonly("falloff", &Lib3dsLight::fall_off)
    .add_property("name", Light_getName)
    .add_property("position", Light_getPos)
    .add_property("spot", Light_getSpot)
    .add_property("color", Light_getColor)
    .def("next", Light_getNext, return_value_policy<reference_existing_object>())
  ;

}
