/*
  Lib3dsMesh
 */

#include <boost/python.hpp>
#include <iostream>
#include <string>
#include <map>
#include <functional>
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

#define GEOM_INIT_NORMALS   0x01
#define GEOM_INIT_MATIDS    0x02
#define GEOM_INIT_TEXELS    0x04
#define GEOM_INIT_FLAGS     0x08
#define GEOM_INIT_SMOOTHING 0x10
#define GEOM_INIT_ALL       0xff

//////////////////////////////////////////////////////////////////////
// Mesh methods
//////////////////////////////////////////////////////////////////////

Lib3dsMesh* Mesh_getNext(Lib3dsMesh* self) { return self->next; }
std::string Mesh_getName(Lib3dsMesh* self) { return std::string(self->name); }
support3d::mat4d Mesh_getMatrix(Lib3dsMesh* self) { return matrix2mat4(self->matrix); }

// Get a tuple with the material names used in the mesh
tuple Mesh_materialNames(Lib3dsMesh* self)
{
  list res;
  std::map<std::string, int> matid_lut;
  for(unsigned int i=0; i<self->faces; i++)
  {
    std::string matname = std::string(self->faceL[i].material);
    if (matid_lut.find(matname)==matid_lut.end())
    {
      matid_lut[matname] = 1;
      res.append(matname);
    }
  }
  return tuple(res);
}

// Compare functor for the map "normal_lut" in Mesh_initGeom()
// It implements a special version of the less function for two vec3d objects
struct cmp_vec3d : public std::binary_function<support3d::vec3d, support3d::vec3d, bool> 
{
  bool operator()(const support3d::vec3d& a, const support3d::vec3d& b) const
  { 
    if (a.x==b.x)
    {
      if (a.y==b.y)
      {
	return (a.z<b.z);
      }
      return (a.y<b.y);
    }
    return (a.x<b.x);
  }
};

/**
  Initialize a cgkit TriMeshGeom .

  The following variables will be added (depending on the flags parameter):

  - N/Nfaces: N contains the raw normals  and Nfaces the normal indices 
    per face.
  - matid: If there is more than one material, a variable "matid" will be 
    added to the trimesh that contains the material IDs per face.
  - 3ds_flags: Contains the flags that are set per face. Bit 0-2 of the
    flags specify the visibility of the corresponding triangle edges.
    Bit 0: Edge 3-1 / Bit 1: Edge 2-3 / Bit 2: Edge 1-2
  - 3ds_smoothing: The smoothing group of the corresponding face. Each bit
    represents one group.  

  \param geom Target trimesh geom
  \param flags Specify what variables to create
*/
void Mesh_initGeom(Lib3dsMesh* self, support3d::TriMeshGeom& geom, int flags=GEOM_INIT_ALL)
{ 
  unsigned int i;
  support3d::vec3d v;
  support3d::ArraySlot<int>* matid_slot=0;
  support3d::ArraySlot<double>* st_slot=0;
  support3d::ArraySlot<int>* Nfaces_slot=0;
  support3d::ArraySlot<support3d::vec3d>* N_slot=0;
  support3d::ArraySlot<int>* flags_slot=0;
  support3d::ArraySlot<int>* smoothing_slot=0;

  geom.deleteAllVariables();
  geom.verts.resize(self->points);
  geom.faces.resize(self->faces);
  
  /////////////////// Set vertices ///////////////////////
  for(i=0; i<self->points; i++)
  {
    v.set(self->pointL[i].pos[0], self->pointL[i].pos[1], self->pointL[i].pos[2]);
    geom.verts.setValue(i, v);
  }

  /////////////////// Set faces ///////////////////////

  std::map<std::string, int> matid_lut;

  if (flags & GEOM_INIT_MATIDS)
  {
    // Build material id lookup table...
    int idcount = 0;
    for(i=0; i<self->faces; i++)
    {
      std::string matname = std::string(self->faceL[i].material);
      if (matid_lut.find(matname)==matid_lut.end())
      {
	matid_lut[matname] = idcount;
	idcount+=1;      
      }
    }
    // Only create a "matid" primitive variable if there is indeed more
    // than just one material...
    if (idcount>1)
    {
      geom.newVariable("matid", support3d::UNIFORM, support3d::INT);
      matid_slot = dynamic_cast<support3d::ArraySlot<int>*>(&geom.slot("matid"));
    }
  }

  // Set faces...
  for(i=0; i<self->faces; i++)
  {
    int face[3] = {self->faceL[i].points[0], self->faceL[i].points[1], self->faceL[i].points[2]};
    geom.faces.setValues(i, face);

    // Set material id
    if (matid_slot!=0)
    {
      std::string matname = std::string(self->faceL[i].material);
      int matid = matid_lut[matname];
      matid_slot->setValue(i, matid);
    }
  }

  /////////////////// Set normals ///////////////////////

  if (flags & GEOM_INIT_NORMALS)
  {
    // Create the "Nfaces" slot that holds the normal indices of each face
    geom.newVariable("Nfaces", support3d::UNIFORM, support3d::INT, 3);
    Nfaces_slot = dynamic_cast<support3d::ArraySlot<int>*>(&geom.slot("Nfaces"));

    // Calculate the normals (according to smoothing groups)
    Lib3dsVector *normalL = new Lib3dsVector[3*self->faces];
   
    lib3ds_mesh_calculate_normals(self, normalL);

    // Lookup table for the normals
    // Key: Normal  Value: Index
    std::map<support3d::vec3d, int, cmp_vec3d> normal_lut;

    // Fill the lookup table and set the normal indices...
    support3d::vec3d n1, n2, n3;
    for(i=0; i<self->faces; i++)
    {
      int v[3];
      int fi = 3*i;
      n1.set(normalL[fi  ][0], normalL[fi  ][1], normalL[fi  ][2]);
      n2.set(normalL[fi+1][0], normalL[fi+1][1], normalL[fi+1][2]);
      n3.set(normalL[fi+2][0], normalL[fi+2][1], normalL[fi+2][2]);
      //    std::cout<<"NORMALS: "<<n1<<" "<<n2<<" "<<n3<<std::endl;

      if (normal_lut.find(n1)==normal_lut.end())
      {
	int idx = normal_lut.size();
	normal_lut[n1] = idx;
      }
      if (normal_lut.find(n2)==normal_lut.end())
      {
	int idx = normal_lut.size();
	normal_lut[n2] = idx;
      }
      if (normal_lut.find(n3)==normal_lut.end())
      {
	int idx = normal_lut.size();
	normal_lut[n3] = idx;
      }
      v[0] = normal_lut[n1];
      v[1] = normal_lut[n2];
      v[2] = normal_lut[n3];
      Nfaces_slot->setValues(i, v);
    }

    delete [] normalL;

    // Create the "N" slot that holds the actual normals
    geom.newVariable("N", support3d::USER, support3d::NORMAL, 1, normal_lut.size());
    N_slot = dynamic_cast<support3d::ArraySlot<support3d::vec3d>*>(&geom.slot("N"));

    // Copy the normals
    for(std::map<support3d::vec3d, int, cmp_vec3d>::iterator it=normal_lut.begin(); it!=normal_lut.end(); it++)
    {
      N_slot->setValue(it->second, it->first);
    }
  }

  /////////////////// Set texture coordinates ///////////////////////

  if (flags & GEOM_INIT_TEXELS)
  {
    // Only create the "st" slot if there are texture coordinates present
    // and if the number of texels is identical with the number of points
    // (this should always be the case in 3ds, shouldn't it?)
    if ((self->texels>0) && (self->texels==self->points))
    {
      // Create the "st" slot that holds the texture coordinates
      geom.newVariable("st", support3d::VARYING, support3d::FLOAT, 2);
      st_slot = dynamic_cast<support3d::ArraySlot<double>*>(&geom.slot("st"));

      for(i=0; i<self->texels; i++)
      {
	double st[2] = {self->texelL[i][0], self->texelL[i][1]};
	st_slot->setValues(i, st);
      }
    }
  }

  /////////////////// Set flags ///////////////////////

  if (flags & GEOM_INIT_FLAGS)
  {
    // Create the "3ds_flags" slot that holds the flags
    geom.newVariable("3ds_flags", support3d::UNIFORM, support3d::INT);
    flags_slot = dynamic_cast<support3d::ArraySlot<int>*>(&geom.slot("3ds_flags"));

    for(i=0; i<self->faces; i++)
    {
      flags_slot->setValue(i, self->faceL[i].flags);
    }
  }

  /////////////////// Set smoothing groups ///////////////////////

  if (flags & GEOM_INIT_SMOOTHING)
  {
    // Create the "3ds_smoothing" slot that holds the smoothing group
    geom.newVariable("3ds_smoothing", support3d::UNIFORM, support3d::INT);
    smoothing_slot = dynamic_cast<support3d::ArraySlot<int>*>(&geom.slot("3ds_smoothing"));

    for(i=0; i<self->faces; i++)
    {
      smoothing_slot->setValue(i, self->faceL[i].smoothing);
    }
  }

}

//////////////////////////////////////////////////////////////////////

void class_Lib3dsMesh()
{

  // Libd3dsMesh
  class_<Lib3dsMesh>("Lib3dsMesh")
    .def_readonly("points", &Lib3dsMesh::points)
    .def_readonly("faces", &Lib3dsMesh::faces)
    .def_readonly("flags", &Lib3dsMesh::flags)
    .def_readonly("texels", &Lib3dsMesh::texels)
    .def_readonly("color", &Lib3dsMesh::color)
    .add_property("name", Mesh_getName)
    .add_property("matrix", Mesh_getMatrix)
    .def("next", Mesh_getNext, return_value_policy<reference_existing_object>())
    .def("initGeom", Mesh_initGeom, (arg("geom"), arg("flags")=GEOM_INIT_ALL))
    .def("materialNames", Mesh_materialNames)
  ;

}
