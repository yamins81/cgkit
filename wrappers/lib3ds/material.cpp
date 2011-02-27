/*
  Material
 */

#include <boost/python.hpp>
#include <iostream>
#include <string>
#include <map>
#include <functional>
#include <exception>
#include <lib3ds/file.h>
#include <lib3ds/camera.h>
#include <lib3ds/light.h>
#include <lib3ds/material.h>
#include <lib3ds/mesh.h>
#include <lib3ds/node.h>

#include <trimeshgeom.h>
#include <slot.h>
#include "common_funcs.h"

using namespace boost::python;


//////////////////////////////////////////////////////////////////////
// Material methods
//////////////////////////////////////////////////////////////////////

Lib3dsMaterial* Mat_getNext(Lib3dsMaterial* self) { return self->next; }
std::string Mat_getName(Lib3dsMaterial* self) { return std::string(self->name); }
support3d::vec4d Mat_getAmbient(Lib3dsMaterial* self) { return color2vec4(self->ambient); }
support3d::vec4d Mat_getDiffuse(Lib3dsMaterial* self) { return color2vec4(self->diffuse); }
support3d::vec4d Mat_getSpecular(Lib3dsMaterial* self) { return color2vec4(self->specular); }


//////////////////////////////////////////////////////////////////////

void class_Lib3dsMaterial()
{
  // Libd3dsMaterial
  class_<Lib3dsMaterial>("Lib3dsMaterial")
    .def_readonly("shininess", &Lib3dsMaterial::shininess)
    .def_readonly("shin_strength", &Lib3dsMaterial::shin_strength)
    .def_readonly("use_blur", &Lib3dsMaterial::use_blur)
    .def_readonly("blur", &Lib3dsMaterial::blur)
    .def_readonly("transparency", &Lib3dsMaterial::transparency)
    .def_readonly("falloff", &Lib3dsMaterial::falloff)
    .def_readonly("additive", &Lib3dsMaterial::additive)
    .def_readonly("use_falloff", &Lib3dsMaterial::use_falloff)
    .def_readonly("self_illum", &Lib3dsMaterial::self_illum)
    #ifdef LIB3DS_VERSION  // if it's defined the version is already greater than 1.2.0
    .def_readonly("self_ilpct", &Lib3dsMaterial::self_ilpct)
    #endif
    .def_readonly("shading", &Lib3dsMaterial::shading)
    .def_readonly("soften", &Lib3dsMaterial::soften)
    .def_readonly("face_map", &Lib3dsMaterial::face_map)
    .def_readonly("two_sided", &Lib3dsMaterial::two_sided)
    .def_readonly("map_decal", &Lib3dsMaterial::map_decal)
    .def_readonly("use_wire", &Lib3dsMaterial::use_wire)
    .def_readonly("use_wire_abs", &Lib3dsMaterial::use_wire_abs)
    .def_readonly("wire_size", &Lib3dsMaterial::wire_size)
    .def_readonly("texture1_map", &Lib3dsMaterial::texture1_map)
    .def_readonly("texture1_mask", &Lib3dsMaterial::texture1_mask)
    .def_readonly("texture2_map", &Lib3dsMaterial::texture2_map)
    .def_readonly("texture2_mask", &Lib3dsMaterial::texture2_mask)
    .def_readonly("opacity_map", &Lib3dsMaterial::opacity_map)
    .def_readonly("opacity_mask", &Lib3dsMaterial::opacity_mask)
    .def_readonly("bump_map", &Lib3dsMaterial::bump_map)
    .def_readonly("bump_mask", &Lib3dsMaterial::bump_mask)
    .def_readonly("specular_map", &Lib3dsMaterial::specular_map)
    .def_readonly("specular_mask", &Lib3dsMaterial::specular_mask)
    .def_readonly("shininess_map", &Lib3dsMaterial::shininess_map)
    .def_readonly("shininess_mask", &Lib3dsMaterial::shininess_mask)
    .def_readonly("self_illum_map", &Lib3dsMaterial::self_illum_map)
    .def_readonly("self_illum_mask", &Lib3dsMaterial::self_illum_mask)
    .def_readonly("reflection_map", &Lib3dsMaterial::reflection_map)
    .def_readonly("reflection_mask", &Lib3dsMaterial::reflection_mask)
    .def_readonly("autorefl_map", &Lib3dsMaterial::autorefl_map)
    .add_property("name", Mat_getName)
    .add_property("ambient", Mat_getAmbient)
    .add_property("diffuse", Mat_getDiffuse)
    .add_property("specular", Mat_getSpecular)
    .def("next", Mat_getNext, return_value_policy<reference_existing_object>())
  ;
}
