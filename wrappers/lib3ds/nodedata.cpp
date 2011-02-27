/*
  Lib3dsNodeData objects
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
//using namespace support3d;


//////////////////////////////////////////////////////////////////////
// ObjectData methods
//////////////////////////////////////////////////////////////////////

support3d::vec3d ObjData_getPivot(Lib3dsObjectData* self) 
{ return vector2vec3(self->pivot); }

support3d::vec3d ObjData_getBBoxMin(Lib3dsObjectData* self) 
{ return vector2vec3(self->bbox_min); }

support3d::vec3d ObjData_getBBoxMax(Lib3dsObjectData* self) 
{ return vector2vec3(self->bbox_max); }

support3d::vec3d ObjData_getPos(Lib3dsObjectData* self) 
{ return vector2vec3(self->pos); }

support3d::quatd ObjData_getRot(Lib3dsObjectData* self) 
{ return quat2quat(self->rot); }

support3d::vec3d ObjData_getScale(Lib3dsObjectData* self) 
{ return vector2vec3(self->scl); }

std::string ObjData_getInstance(Lib3dsObjectData* self)
{ return std::string(self->instance); }

std::string ObjData_getMorph(Lib3dsObjectData* self)
{ return std::string(self->morph); }

//////////////////////////////////////////////////////////////////////
// CameraData methods
//////////////////////////////////////////////////////////////////////

support3d::vec3d CamData_getPos(Lib3dsCameraData* self) 
{ return vector2vec3(self->pos); }

//////////////////////////////////////////////////////////////////////
// TargetData methods
//////////////////////////////////////////////////////////////////////

support3d::vec3d TargetData_getPos(Lib3dsTargetData* self) 
{ return vector2vec3(self->pos); }

//////////////////////////////////////////////////////////////////////
// LightData methods
//////////////////////////////////////////////////////////////////////

support3d::vec3d LightData_getPos(Lib3dsLightData* self) 
{ return vector2vec3(self->pos); }

support3d::vec3d LightData_getCol(Lib3dsLightData* self) 
{ return color2vec3(self->col); }

//////////////////////////////////////////////////////////////////////
// SpotData methods
//////////////////////////////////////////////////////////////////////

support3d::vec3d SpotData_getPos(Lib3dsSpotData* self) 
{ return vector2vec3(self->pos); }


//////////////////////////////////////////////////////////////////////

void nodeData()
{
  // Libd3dsObjectData
  class_<Lib3dsObjectData>("Lib3dsObjectData")
    .add_property("pivot", ObjData_getPivot)
    .add_property("instance", ObjData_getInstance)
    .add_property("bbox_min", ObjData_getBBoxMin)
    .add_property("bbox_max", ObjData_getBBoxMax)
    .add_property("pos", ObjData_getPos)
    .add_property("rot", ObjData_getRot)
    .add_property("scl", ObjData_getScale)
    .def_readonly("morph_smooth", &Lib3dsObjectData::morph_smooth)
    .add_property("morph", ObjData_getMorph)
    .def_readonly("hide", &Lib3dsObjectData::hide)
  ;

  // Libd3dsCameraData
  class_<Lib3dsCameraData>("Lib3dsCameraData")
    .add_property("pos", CamData_getPos)
    .def_readonly("fov", &Lib3dsCameraData::fov)
    .def_readonly("roll", &Lib3dsCameraData::roll)
  ;

  // Libd3dsTargetData
  class_<Lib3dsTargetData>("Lib3dsTargetData")
    .add_property("pos", TargetData_getPos)
  ;

  // Libd3dsLightData
  class_<Lib3dsLightData>("Lib3dsLightData")
    .add_property("pos", LightData_getPos)
    .add_property("col", LightData_getCol)
    .def_readonly("hotspot", &Lib3dsLightData::hotspot)
    .def_readonly("falloff", &Lib3dsLightData::falloff)
    .def_readonly("roll", &Lib3dsLightData::roll)
  ;

  // Libd3dsSpotData
  class_<Lib3dsSpotData>("Lib3dsSpotData")
    .add_property("pos", SpotData_getPos)
  ;
}
