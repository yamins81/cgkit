/*
  Main wrapper file.
 */

/*
  This Python module is almost a direct wrapper of the lib3ds library
  (http://lib3ds.sourceforge.net). The Lib3dsFile structure is encapsulated
  in the class File3ds. Most other structures have a direct Python 
  correspondence. 
  One exception is the Mesh class that doesn't allow access to the
  vertices and faces directly (because this would be too slow) but
  that can initialize a cgkit TriMeshGeom.
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

/* libds functions:

  - lib3ds_file_load(filename) - Open a file and create the 3ds file struct
  - lib3ds_file_free(f)        - Release the file struct

  - lib3ds_file_eval(f, frame) - Evaluate the scene at a particular time

  - node* = lib3ds_file_node_by_name(f, name, type)
    type: LIB3DS_UNKNOWN_NODE, LIB3DS_AMBIENT_NODE,
          LIB3DS_CAMERA_NODE, LIB3DS_TARGET_NODE,
	  LIB3DS_OBJECT_NODE, LIB3DS_LIGHT_NODE,
	  LIB3DS_SPOT_NODE 

 */

#define GEOM_INIT_NORMALS   0x01
#define GEOM_INIT_MATIDS    0x02
#define GEOM_INIT_TEXELS    0x04
#define GEOM_INIT_FLAGS     0x08
#define GEOM_INIT_SMOOTHING 0x10
#define GEOM_INIT_ALL       0xff

/**
 3DS-File
*/
class File3ds
{
  public:
  Lib3dsFile* file;

  public:
  File3ds() : file(0) {}
  ~File3ds() { free(); }

  /**
     Load a 3ds file.

     Wraps libd3ds_file_load()
   */
  void load(char* filename)
  {
    // Open the 3ds file...
    file = lib3ds_file_load(filename);
    if (file==0)
    {
      std::cout<<"File open error"<<std::endl;
      return;
    }
  }

  /**
     Release the 3ds file structure.

     Wraps libd3ds_file_free()
   */
  void free()
  {
    if (file!=0)
    {
      // Release the 3ds file structure...
      lib3ds_file_free(file);
      file = 0;
    }
  }

  /**
     Evaluate the scene at a given time.

     Wraps libd3ds_file_eval()
   */
  void eval(double t)
  {
    if (file!=0) lib3ds_file_eval(file, t);
  }

  Lib3dsMaterial* materialByName(char* name)
  {
    if (file!=0) 
      return lib3ds_file_material_by_name(file, name);
    else
      return 0;
  }

  Lib3dsMesh* meshByName(char* name)
  {
    if (file!=0) 
      return lib3ds_file_mesh_by_name(file, name);
    else
      return 0;
  }

  Lib3dsCamera* cameraByName(char* name)
  {
    if (file!=0) 
      return lib3ds_file_camera_by_name(file, name);
    else
      return 0;
  }

  Lib3dsLight* lightByName(char* name)
  {
    if (file!=0) 
      return lib3ds_file_light_by_name(file, name);
    else
      return 0;
  }

  Lib3dsNode* nodeByName(char* name, int type)
  {
    if (file!=0) 
      return lib3ds_file_node_by_name(file, name, (Lib3dsNodeTypes)type);
    else
      return 0;
  }

  Lib3dsNode* nodes() { return file->nodes; }
  Lib3dsMesh* meshes() { return file->meshes; }
  Lib3dsMaterial* materials() { return file->materials; }
  Lib3dsCamera* cameras() { return file->cameras; }
  Lib3dsLight* lights() { return file->lights; }

  long mesh_version() { return file->mesh_version; }
  int keyf_revision() { return file->keyf_revision; }
  std::string name() { return std::string(file->name); }
  double master_scale() { return file->master_scale; }
  int frames() { return file->frames; }
  int segment_from() { return file->segment_from; }
  int segment_to() { return file->segment_to; }
  int current_frame() { return file->current_frame; }
};


//////////////////////////////////////////////////////////////////////
// Node methods
//////////////////////////////////////////////////////////////////////

Lib3dsNode* Node_getNext(Lib3dsNode* self) { return self->next; }
Lib3dsNode* Node_getChilds(Lib3dsNode* self) { return self->childs; }
Lib3dsNode* Node_getParent(Lib3dsNode* self) { return self->parent; }
std::string Node_getName(Lib3dsNode* self) { return std::string(self->name); }
int Node_getType(Lib3dsNode* self) { return self->type; }
support3d::mat4d Node_getMatrix(Lib3dsNode* self) { return matrix2mat4(self->matrix); }
Lib3dsAmbientData Node_getAmbientData(Lib3dsNode* self) { return self->data.ambient; }
Lib3dsObjectData Node_getObjectData(Lib3dsNode* self) { return self->data.object; }
Lib3dsCameraData Node_getCameraData(Lib3dsNode* self) { return self->data.camera; }
Lib3dsTargetData Node_getTargetData(Lib3dsNode* self) { return self->data.target; }
Lib3dsLightData Node_getLightData(Lib3dsNode* self) { return self->data.light; }
Lib3dsSpotData Node_getSpotData(Lib3dsNode* self) { return self->data.spot; }


//////////////////////////////////////////////////////////////////////
// Camera methods
//////////////////////////////////////////////////////////////////////

Lib3dsCamera* Cam_getNext(Lib3dsCamera* self) { return self->next; }
std::string Cam_getName(Lib3dsCamera* self) { return std::string(self->name); }
support3d::vec3d Cam_getPos(Lib3dsCamera* self) { return vector2vec3(self->position); }
support3d::vec3d Cam_getTarget(Lib3dsCamera* self) { return vector2vec3(self->target); }


//////////////////////////////////////////////////////////////////////
// TextureMap methods
//////////////////////////////////////////////////////////////////////

std::string TexMap_getName(Lib3dsTextureMap* self) { return std::string(self->name); }
tuple TexMap_getScale(Lib3dsTextureMap* self) { return make_tuple(self->scale[0], self->scale[1]); }
tuple TexMap_getOffset(Lib3dsTextureMap* self) { return make_tuple(self->offset[0], self->offset[1]); }
support3d::vec3d TexMap_getTint1(Lib3dsTextureMap* self) { return color2vec3(self->tint_1); }
support3d::vec3d TexMap_getTint2(Lib3dsTextureMap* self) { return color2vec3(self->tint_2); }
support3d::vec3d TexMap_getTintr(Lib3dsTextureMap* self) { return color2vec3(self->tint_r); }
support3d::vec3d TexMap_getTintg(Lib3dsTextureMap* self) { return color2vec3(self->tint_g); }
support3d::vec3d TexMap_getTintb(Lib3dsTextureMap* self) { return color2vec3(self->tint_b); }



// Testfunktion zur Bug-Suche
void testfunc(support3d::TriMeshGeom& tmin)
{
  support3d::TriMeshGeom& tm = tmin;
  //  support3d::TriMeshGeom tm;

  std::cout<<"START"<<std::endl;
  //  tm.deleteAllVariables();
  //  tm.verts.resize(10);
  //  tm.faces.resize(20);

  std::cout<<"SLOT1"<<std::endl;
  try
  {
    std::cout<<"tm.getName()"<<std::endl;
    std::string n = tm.getName();
    std::cout<<" -> '"<<n<<"'"<<std::endl;
    
    std::cout<<"tm.hasSlot('cog')"<<std::endl;
    bool b = tm.hasSlot("cog");
    std::cout<<" -> "<<b<<std::endl;

    std::cout<<"tm.slot('cog')"<<std::endl;
    support3d::ISlot& s = tm.slot("cog");
  }
  catch(const support3d::EKeyError& e)
  {
    std::cout<<"Exception: "<<e.what()<<std::endl;
  }
  catch(const std::exception& e)
  {
    std::cout<<"STANDARD EXCEPTION: "<<e.what()<<std::endl;
  }
  catch(const char* e)
  {
    std::cout<<"CHAR* EXCEPTION: "<<e<<std::endl;
  }
  catch(const std::string& e)
  {
    std::cout<<"STRING EXCEPTION: "<<e<<std::endl;
  }
  catch(...)
  {
    std::cout<<"UNKNOWN EXCEPTION"<<std::endl;
  };
  std::cout<<"SLOT2"<<std::endl;

  std::cout<<"END"<<std::endl;
}


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

void nodeData();
void class_Lib3dsLight();
void class_Lib3dsMesh();
void class_Lib3dsMaterial();

//BOOST_PYTHON_MODULE(lib3ds)
void lib3ds()
{
  nodeData();

  def("testfunc", testfunc);

  // Libd3dsNode
  class_<Lib3dsNode>("Lib3dsNode")
    .def_readonly("node_id", &Lib3dsNode::node_id)
    .add_property("name", Node_getName)
    .add_property("type", Node_getType)
    .add_property("matrix", Node_getMatrix)
    .add_property("object_data", Node_getObjectData)
    .add_property("camera_data", Node_getCameraData)
    .add_property("target_data", Node_getTargetData)
    .add_property("light_data", Node_getLightData)
    .add_property("spot_data", Node_getSpotData)
    //  .add_property("next", make_getter(Node_getNext, return_internal_reference<>()))
    .def("next", Node_getNext, return_value_policy<reference_existing_object>())
    .def("childs", Node_getChilds, return_value_policy<reference_existing_object>())
    .def("parent", Node_getParent, return_value_policy<reference_existing_object>())
  ;

  // Libd3dsMesh
  class_Lib3dsMesh();

  // Libd3dsCamera
  class_<Lib3dsCamera>("Lib3dsCamera")
    .def_readonly("roll", &Lib3dsCamera::roll)
    .def_readonly("fov", &Lib3dsCamera::fov)
    .def_readonly("see_cone", &Lib3dsCamera::see_cone)
    .def_readonly("near_range", &Lib3dsCamera::near_range)
    .def_readonly("far_range", &Lib3dsCamera::far_range)
    .add_property("name", Cam_getName)
    .add_property("position", Cam_getPos)
    .add_property("target", Cam_getTarget)
    .def("next", Cam_getNext, return_value_policy<reference_existing_object>())
  ;

  // Libd3dsLight
  class_Lib3dsLight();

  // Libd3dsTextureMap
  class_<Lib3dsTextureMap>("Lib3dsTextureMap")
    .add_property("name", TexMap_getName)
    .def_readonly("flags", &Lib3dsTextureMap::flags)
    .def_readonly("percent", &Lib3dsTextureMap::percent)
    .def_readonly("blur", &Lib3dsTextureMap::blur)
    .def_readonly("rotation", &Lib3dsTextureMap::rotation)
    .add_property("scale", TexMap_getScale)
    .add_property("offset", TexMap_getOffset)
    .add_property("tint_1", TexMap_getTint1)
    .add_property("tint_2", TexMap_getTint2)
    .add_property("tint_r", TexMap_getTintr)
    .add_property("tint_g", TexMap_getTintg)
    .add_property("tint_b", TexMap_getTintb)
  ;

  // Libd3dsAutoReflMap
  class_<Lib3dsAutoReflMap>("Lib3dsAutoReflMap")
    .def_readonly("flags", &Lib3dsAutoReflMap::flags)
    .def_readonly("level", &Lib3dsAutoReflMap::level)
    .def_readonly("size", &Lib3dsAutoReflMap::size)
    .def_readonly("frame_step", &Lib3dsAutoReflMap::frame_step)
  ;

  // Libd3dsMaterial
  class_Lib3dsMaterial();

  class_<File3ds>("File3ds")
    .add_property("mesh_version", &File3ds::mesh_version)
    .add_property("keyf_revision", &File3ds::keyf_revision)
    .add_property("name", &File3ds::name)
    .add_property("master_scale", &File3ds::master_scale)
    .add_property("frames", &File3ds::frames)
    .add_property("segment_from", &File3ds::segment_from)
    .add_property("segment_to", &File3ds::segment_to)
    .add_property("current_frame", &File3ds::current_frame)
    .def("load", &File3ds::load)
    .def("free", &File3ds::free)
    .def("eval", &File3ds::eval)
    .def("materialByName", &File3ds::materialByName, return_value_policy<reference_existing_object>())
    .def("meshByName", &File3ds::meshByName, return_value_policy<reference_existing_object>())
    .def("cameraByName", &File3ds::cameraByName, return_value_policy<reference_existing_object>())
    .def("lightByName", &File3ds::lightByName, return_value_policy<reference_existing_object>())
    .def("nodeByName", &File3ds::nodeByName, return_value_policy<reference_existing_object>())
    .def("nodes", &File3ds::nodes, return_value_policy<reference_existing_object>())
    .def("meshes", &File3ds::meshes, return_value_policy<reference_existing_object>())
    .def("cameras", &File3ds::cameras, return_value_policy<reference_existing_object>())
    .def("lights", &File3ds::lights, return_value_policy<reference_existing_object>())
    .def("materials", &File3ds::materials, return_value_policy<reference_existing_object>())
  ;
}
