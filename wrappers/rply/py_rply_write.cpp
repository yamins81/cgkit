/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is the Python Computer Graphics Kit.
 *
 * The Initial Developer of the Original Code is Matthias Baas.
 * Portions created by the Initial Developer are Copyright (C) 2004
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */

#include <boost/python.hpp>
#include <iostream>
#include <exception>
#include <string>
#include <vector>
#include "common_exceptions.h"
#include "polyhedrongeom.h"
#include "trimeshgeom.h"
#include "arrayslot.h"
#include "vec3.h"
#include "mat4.h"

#include "rply/rply.h"

using namespace boost::python;

/**
   Base class for all 'writer' objects.

   A writer object is responsible for writing the next data item into
   the PLY file. This is done in the write() method.
 */
class Writer
{
  public:
  virtual ~Writer() {}

  /**
     Write the data item at position \a idx into the file.
   */
  virtual void write(p_ply handle, int idx) = 0;
  /**
     Return the total number of elements.
   */
  virtual int numElements() const = 0;
};

/// Integer writer (mult=1).
class IntWriter : public Writer
{
  public:
  support3d::ArraySlot<int>* slot;
  int* data;

  IntWriter(support3d::ArraySlot<int>* s) 
    : slot(s) { data = s->dataPtr(); }

  void write(p_ply handle, int idx)
  {
    ply_write(handle, data[idx]);
  }

  int numElements() const { return slot->size(); }
};

/// Float writer (mult=1).
class FloatWriter : public Writer
{
  public:
  support3d::ArraySlot<double>* slot;
  double* data;

  FloatWriter(support3d::ArraySlot<double>* s) 
    : slot(s) { data = s->dataPtr(); }

  void write(p_ply handle, int idx)
  {
    ply_write(handle, data[idx]);
  }

  int numElements() const { return slot->size(); }
};

/// Vec3 writer (mult=1).
class Vec3Writer : public Writer
{
  public:
  support3d::ArraySlot<support3d::vec3d>* slot;
  support3d::vec3d* data;
  support3d::mat4d WT;
  bool transform;

  Vec3Writer(support3d::ArraySlot<support3d::vec3d>* s) 
    : slot(s), WT(), transform(false){ data = s->dataPtr(); }
  Vec3Writer(support3d::ArraySlot<support3d::vec3d>* s, support3d::mat4d aWT) 
    : slot(s), WT(aWT), transform(true) { data = s->dataPtr(); }

  void write(p_ply handle, int idx)
  {
    support3d::vec3d* v = data+idx;
    if (transform)
    {
      support3d::vec3d w = WT*(*v);
      ply_write(handle, w.x);
      ply_write(handle, w.y);
      ply_write(handle, w.z);
    }
    else
    {
      ply_write(handle, v->x);
      ply_write(handle, v->y);
      ply_write(handle, v->z);
    }
  }

  int numElements() const { return slot->size(); }
};

/// Int writer (mult>1)
class IntListWriter : public Writer
{
  public:
  support3d::ArraySlot<int>* slot;
  int* data;

  IntListWriter(support3d::ArraySlot<int>* s) 
    : slot(s) { data = s->dataPtr(); }

  void write(p_ply handle, int idx)
  {
    int mult = slot->multiplicity();
    ply_write(handle, mult);
    for(int i=0; i<mult; i++)
    {
      ply_write(handle, data[mult*idx+i]);
    }
  }

  int numElements() const { return slot->size(); }
};

/// Float writer (mult>1)
class FloatListWriter : public Writer
{
  public:
  support3d::ArraySlot<double>* slot;
  double* data;

  FloatListWriter(support3d::ArraySlot<double>* s)
    : slot(s) { data = s->dataPtr(); }

  void write(p_ply handle, int idx)
  {
    int mult = slot->multiplicity();
    ply_write(handle, mult);
    for(int i=0; i<mult; i++)
    {
      ply_write(handle, data[mult*idx+i]);
    }
  }

  int numElements() const { return slot->size(); }
};

/// Vec3 writer (mult>1)
class Vec3ListWriter : public Writer
{
  public:
  support3d::ArraySlot<support3d::vec3d>* slot;
  support3d::vec3d* data;

  Vec3ListWriter(support3d::ArraySlot<support3d::vec3d>* s)
    : slot(s) { data = s->dataPtr(); }

  void write(p_ply handle, int idx)
  {
    int mult = slot->multiplicity();
    int i;

    // x component
    ply_write(handle, mult);
    for(i=0; i<mult; i++)
    {
      support3d::vec3d* v = data+mult*idx+i;
      ply_write(handle, v->x);
    }
    // y component
    ply_write(handle, mult);
    for(i=0; i<mult; i++)
    {
      support3d::vec3d* v = data+mult*idx+i;
      ply_write(handle, v->y);
    }
    // z component
    ply_write(handle, mult);
    for(i=0; i<mult; i++)
    {
      support3d::vec3d* v = data+mult*idx+i;
      ply_write(handle, v->z);
    }
  }

  int numElements() const { return slot->size(); }
};

/// Polygon face writer
class PolyFaceWriter : public Writer
{
  public:
  support3d::PolyhedronGeom* geom;

  PolyFaceWriter(support3d::PolyhedronGeom* g) 
    : geom(g) {}

  void write(p_ply handle, int idx)
  {
    ply_write(handle, geom->getNumVerts(idx, 0));
    support3d::PolyhedronGeom::LoopIterator it;
    support3d::PolyhedronGeom::LoopIterator it_end = geom->loopEnd(idx, 0);
    for(it=geom->loopBegin(idx, 0); it!=it_end; it++)
    {
      ply_write(handle, *it);
    }
  }

  int numElements() const { return geom->getNumPolys(); }
};


//////////////////////////////////////////////////////////////////////

/**
  Error callback
 */
static void error_callback(const char* message)
{
  throw support3d::ERuntimeError(std::string("RPly error: ")+std::string(message));
}

/**
  PLYWriter.

  This class can be used to export PLY files.
 */
class PLYWriter
{
  private:
  /// PLY file handle.
  p_ply handle;

  /// Writer objects
  std::vector<Writer*> varyingwriters;
  std::vector<Writer*> uniformwriters;
  std::vector<Writer*> userwriters;

  public:
  PLYWriter() : handle(0), varyingwriters(), uniformwriters(), userwriters() {}
  ~PLYWriter() { close(); }

  /**
     Create a PLY file.
   */
  void create(std::string name, e_ply_storage_mode mode)
  {
    close();
    // Open the file
    handle = ply_create(name.c_str(), mode, error_callback);
    if (handle==0)
    {
      throw support3d::EIOError("Could not create file \""+name+"\".");
    }
  }

  /**
     Close the file (if it is open).
   */
  void close()
  {
    if (handle!=0)
    {
      ply_close(handle);
      handle = 0;
    }
    deleteWriters();
  }

  /**
     Add a comment line.

     This method has to be called after create() and before write().
     s must not contain a newline character.
   */
  void addComment(std::string s)
  {
    if (!ply_add_comment(handle, s.c_str()))
      throw support3d::ERuntimeError("Failed to add a comment line to the PLY file.");
  }

  /**
     Add an obj_info line.

     This method has to be called after create() and before write().
     s must not contain a newline character.
   */
  void addObjInfo(std::string s)
  {
    if (!ply_add_obj_info(handle, s.c_str()))
      throw support3d::ERuntimeError("Failed to add an obj_info line to the PLY file.");
  }

  /**
     Write the file.

     \param geom The geometry to be exported (TriMeshGeom or PolyhedronGeom)
   */
  void write(support3d::GeomObject& geom, support3d::mat4d WT)
  {
    support3d::TriMeshGeom* tm = dynamic_cast<support3d::TriMeshGeom*>(&geom);
    support3d::PolyhedronGeom* pg = dynamic_cast<support3d::PolyhedronGeom*>(&geom);
    support3d::ArraySlot<support3d::vec3d>* verts=0;
    int numfaces=0;

    // Check if the target geom is really a TriMesh or Polyhedron
    if (tm==0 && pg==0)
    {
	throw support3d::EValueError("Invalid geom: Only TriMesh and Polyhedron geometry can be exported.");
    }
    if (pg!=0)
    {
      verts = &pg->verts;
      numfaces = pg->getNumPolys();
    }
    else
    {
      verts = &tm->verts;
      numfaces = tm->faces.size();
    }

    // Delete all existing writer objects (just in case)...
    deleteWriters();

    support3d::GeomObject::VariableIterator vit;
    Writer* writer;
    int i;
    unsigned int j;

    // Declare verts and varying variables
    plyAddElement("vertex", verts->size());
    plyAddScalarProperty("x", PLY_FLOAT64);
    plyAddScalarProperty("y", PLY_FLOAT64);
    plyAddScalarProperty("z", PLY_FLOAT64);
    varyingwriters.push_back(new Vec3Writer(verts, WT));
    for(vit=geom.variablesBegin(); vit!=geom.variablesEnd(); vit++)
    {
      if (vit->second.storage==support3d::VARYING)
      {
	writer = addVariable(vit);
	if (writer!=0)
	  varyingwriters.push_back(writer);
      }
    }

    // Declare faces and uniform variables
    plyAddElement("face", numfaces);
    plyAddListProperty("vertex_indices", PLY_UIN32, PLY_UIN32);
    if (pg!=0)
      uniformwriters.push_back(new PolyFaceWriter(pg));
    else
      uniformwriters.push_back(new IntListWriter(&tm->faces));
    for(vit=geom.variablesBegin(); vit!=geom.variablesEnd(); vit++)
    {
      if (vit->second.storage==support3d::UNIFORM)
      {
	writer = addVariable(vit);
	if (writer!=0)
	  uniformwriters.push_back(writer);
      }
    }

    // Declare other variables
    for(vit=geom.variablesBegin(); vit!=geom.variablesEnd(); vit++)
    {
      if ((vit->second.storage==support3d::VARYING) ||
          (vit->second.storage==support3d::UNIFORM))
	continue;
      {
	writer = addVariable(vit, true);
	if (writer!=0)
	  userwriters.push_back(writer);
      }
    }

    // Write the header
    if (!ply_write_header(handle))
      throw support3d::ERuntimeError("Failed to write the PLY header.");

    // Write verts + varying variables
    for(i=0; i<verts->size(); i++)
    {
      for(j=0; j<varyingwriters.size(); j++)
	varyingwriters[j]->write(handle, i);
    }

    // Write faces + uniform variables
    for(i=0; i<numfaces; i++)
    {
      for(j=0; j<uniformwriters.size(); j++)
	uniformwriters[j]->write(handle, i);
    }

    // Write user (and other) variables
    for(j=0; j<userwriters.size(); j++)
    {
      writer = userwriters[j];
      for(i=0; i<writer->numElements(); i++)
	writer->write(handle, i);
    }

    deleteWriters();
  }

  private:

  /**
     Deallocate all writer objects.
   */
  void deleteWriters()
  {
    unsigned int j;

    // Delete all writer objects...
    for(j=0; j<varyingwriters.size(); j++)
      delete varyingwriters[j];

    for(j=0; j<uniformwriters.size(); j++)
      delete uniformwriters[j];

    for(j=0; j<userwriters.size(); j++)
      delete userwriters[j];

    varyingwriters.clear();
    uniformwriters.clear();
    userwriters.clear();
  }

  /**
     Add a variable declaration to the header and return a Writer object.

     The storage class has to be checked *before* calling the method!
     The method returns a new instance of a Writer object or 0 if the
     varibale can't be written. The object has to be deleted by the caller.
   */
  Writer* addVariable(support3d::GeomObject::VariableIterator& vit, bool is_user=false)
  {
    support3d::VarType type = vit->second.type;
    int mult = vit->second.multiplicity;
    std::string basename;

    if (mult==1)
    {
      switch(type)
      {
      // int
      case support3d::INT:
	addElement(vit, is_user);
	plyAddScalarProperty(vit->first.c_str(), PLY_INT32);
	return new IntWriter((support3d::ArraySlot<int>*)(vit->second.slot));
      // float
      case support3d::FLOAT:
	addElement(vit, is_user);
	plyAddScalarProperty(vit->first.c_str(), PLY_FLOAT64);
	return new FloatWriter((support3d::ArraySlot<double>*)(vit->second.slot));
      // vec3
      case support3d::VECTOR:
      case support3d::POINT:
      case support3d::NORMAL:
	if (type==support3d::NORMAL && vit->first=="N")
	  basename = "n";
	else
	  basename = vit->first;
	addElement(vit, is_user);
	plyAddScalarProperty((basename+"x").c_str(), PLY_FLOAT64);
	plyAddScalarProperty((basename+"y").c_str(), PLY_FLOAT64);
	plyAddScalarProperty((basename+"z").c_str(), PLY_FLOAT64);
	return new Vec3Writer((support3d::ArraySlot<support3d::vec3d>*)(vit->second.slot));
      default: break; /* do nothing */
      }
    }
    else
    {
      switch(type)
      {
      // int
      case support3d::INT:
	addElement(vit, is_user);
	plyAddListProperty(vit->first.c_str(), PLY_UIN32, PLY_INT32);
	return new IntListWriter((support3d::ArraySlot<int>*)(vit->second.slot));
      // float
      case support3d::FLOAT:
	addElement(vit, is_user);
	plyAddListProperty(vit->first.c_str(), PLY_UIN32, PLY_FLOAT64);
	return new FloatListWriter((support3d::ArraySlot<double>*)(vit->second.slot));
      // vec3
      case support3d::VECTOR:
      case support3d::POINT:
      case support3d::NORMAL:
	basename = vit->first;
	addElement(vit, is_user);
	plyAddListProperty((basename+"x").c_str(), PLY_UIN32, PLY_FLOAT64);
	plyAddListProperty((basename+"y").c_str(), PLY_UIN32, PLY_FLOAT64);
	plyAddListProperty((basename+"z").c_str(), PLY_UIN32, PLY_FLOAT64);
	return new Vec3ListWriter((support3d::ArraySlot<support3d::vec3d>*)(vit->second.slot));
      default: break; /* do nothing */
      }
    }
    return 0;
  }

  /**
     Adds an element declaration if is_user is true.

     This method is a helper method for the addVariable() method.
   */
  void addElement(support3d::GeomObject::VariableIterator& vit, bool is_user=false)
  {
    if (is_user)
      plyAddElement(vit->first.c_str(), vit->second.slot->size());
  }

  /**
     Wrapper around ply_add_element().
   */
  void plyAddElement(const char *name, long ninstances)
  {
    if (!ply_add_element(handle, name, ninstances))
      throw support3d::ERuntimeError("Failed to add a new element to the PLY file.");
  }

  /**
     Wrapper around ply_add_scalar_property().
   */
  void plyAddScalarProperty(const char *name, e_ply_type type)
  {
    if (!ply_add_scalar_property(handle, name, type))
      throw support3d::ERuntimeError("Failed to add a new property to the PLY file.");
      
  }

  /**
     Wrapper around ply_add_list_property().
   */
  void plyAddListProperty(const char *name, e_ply_type length_type, e_ply_type value_type)
  {
    if (!ply_add_list_property(handle, name, length_type, value_type))
      throw support3d::ERuntimeError("Failed to add a new property to the PLY file.");
  }

};

//////////////////////////////////////////////////////////////////////
void rply_write()
{
  enum_<e_ply_storage_mode>("PlyStorageMode")
    .value("BIG_ENDIAN", PLY_BIG_ENDIAN)
    .value("LITTLE_ENDIAN", PLY_LITTLE_ENDIAN)
    .value("ASCII", PLY_ASCII)
  ;

  class_<PLYWriter>("PLYWriter", init<>())
    .def("create", &PLYWriter::create)
    .def("close", &PLYWriter::close)
    .def("addComment", &PLYWriter::addComment)
    .def("addObjInfo", &PLYWriter::addObjInfo)
    .def("write", &PLYWriter::write)
  ;
}
