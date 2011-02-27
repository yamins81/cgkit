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

/*
  PLY import is implemented using the RPly library which is available
  at http://www.cs.princeton.edu/~diego/professional/rply/
  The entire RPly distribution is included in the subdirectory "rply".

  The RPly functions are not exposed in Python, instead the class PLYReader
  uses RPly internally and provides high level access for reading the file.
  The actual import is mainly done here in C++ with Python as a controling
  instance.
 */

#include <boost/python.hpp>
#include <iostream>
#include <exception>
#include <string>
#include <vector>
#include "common_exceptions.h"
#include "polyhedrongeom.h"
#include "arrayslot.h"
#include "vec3.h"

#include "rply/rply.h"

using namespace boost::python;

/**
   This class describes a user defined variable.

   This class describes one user defined variable as described by the
   declaration tuples that are passed in as argument to the read() method
   of the PLYReader.
   The variable can either be an int or float variable, a list of ints or
   floats or a vec3 type (which consists of 3 individual float variables).
 */
class VariableInfo
{
  public:
  /// Variable name (=slot name)
  std::string varname;
  /// Variable type
  support3d::VarType vartype;
  /// Element name
  std::string element;
  /// Number of properties (1 or 3)
  int numprops;
  /// Property 1 name
  std::string prop1name;
  /// Property 2 name
  std::string prop2name;
  /// Property 3 name
  std::string prop3name;
  /// The number of variable values
  long ninstances;
  /// The property type
  e_ply_type proptype;
  e_ply_type len_type;
  e_ply_type val_type;
  bool valid;

  VariableInfo() : varname(), vartype(support3d::INT),
		   element(), numprops(0), 
		   prop1name(), prop2name(), prop3name(), 
		   ninstances(0), proptype(PLY_INT), len_type(PLY_INT), 
		   val_type(PLY_INT), valid(false) {}

  /// Return the storage class for the corresponding slot
  support3d::VarStorage storageClass()
  {
    if (element=="vertex")
      return support3d::VARYING;
    if (element=="face")
      return support3d::UNIFORM;
    return support3d::USER;
  }

};

//////////////////////////////////////////////////////////////////////

/*
  The following PlyVarXyzInfo classes are used as input for the callback 
  functions that are called by the RPly library for each new value.
  The classes store the data that is required by the callback functions
  (mainly the slot that should receive the value).
 */

/// Base class
class PlyVarInfoBase
{
  public:
  virtual ~PlyVarInfoBase() {}
};

/// Info about an int slot
class PlyVarIntInfo : public PlyVarInfoBase
{
  public:
  /// The slot that should receive the values
  support3d::ArraySlot<int>* slot;
  /// The current index
  long idx;
  /// The data block
  int* data;

  PlyVarIntInfo(support3d::ArraySlot<int>* s) 
    : slot(s), idx(0) { data = s->dataPtr(); }
};

/// Info about a float slot
class PlyVarFloatInfo : public PlyVarInfoBase
{
  public:
  /// The slot that should receive the values
  support3d::ArraySlot<double>* slot;
  /// The current index
  long idx;
  /// The data block
  double* data;

  PlyVarFloatInfo(support3d::ArraySlot<double>* s) 
    : slot(s), idx(0) { data = s->dataPtr(); }
};

/// Info about a vec3 slot
class PlyVarVec3Info : public PlyVarInfoBase
{
  public:
  /// The slot that should receive the values
  support3d::ArraySlot<support3d::vec3d>* slot;
  /// The current index
  long idx;
  /// The data block
  support3d::vec3d* data;


  PlyVarVec3Info(support3d::ArraySlot<support3d::vec3d>* s) 
    : slot(s), idx(0) { data = s->dataPtr(); }
};

/**
  Info about a list variable.

  This class is used as input for the var_list() callback which is
  used for all list properties. This callback builds the entire list
  and calls the final callback that receives the complete list as input.
 */
class PlyVarListInfo : public PlyVarInfoBase
{
  public:
  /// Callback function that is called when the list is complete
  int (*callback)(PlyVarListInfo& vi);
  /// The geom object
  support3d::PolyhedronGeom& geom;
  /// The current list
  std::vector<double> elements;
  /// The current index
  long idx;
  /// Variable info
  VariableInfo varinfo;
  /// The data block
  void* data;
  /// The multiplicity of the array slot
  short mult;
  /// Flag that specifies if the poly orientation should be inverted
  bool invertfaces;

  PlyVarListInfo(support3d::PolyhedronGeom& ageom, int (*acallback)(PlyVarListInfo&), bool invfaces=false) 
    : callback(acallback), geom(ageom), elements(), idx(0), varinfo(), data(0), mult(0), invertfaces(invfaces) {}
  PlyVarListInfo(support3d::PolyhedronGeom& ageom, int (*acallback)(PlyVarListInfo&), VariableInfo vi) 
    : callback(acallback), geom(ageom), elements(), idx(0), varinfo(vi), data(0), mult(0), invertfaces(false) {}
};

// Callbacks:

/// Set an integer value.
int var_int(p_ply_argument arg)
{
  PlyVarIntInfo* vi;
  ply_get_argument_user_data(arg, (void**)(&vi), 0);

  vi->data[vi->idx] = int(ply_get_argument_value(arg));
  vi->idx++;
  return 1;
}

/// Set an float value.
int var_float(p_ply_argument arg)
{
  PlyVarFloatInfo* vi;
  ply_get_argument_user_data(arg, (void**)(&vi), 0);

  vi->data[vi->idx] = ply_get_argument_value(arg);
  vi->idx++;
  return 1;
}

/// Set the x component of a vec3 slot.
int var_vec3_x(p_ply_argument arg)
{
  PlyVarVec3Info* vi;
  ply_get_argument_user_data(arg, (void**)(&vi), 0);

  vi->data[vi->idx].x = ply_get_argument_value(arg);
  vi->idx++;
  return 1;
}

/// Set the y component of a vec3 slot.
int var_vec3_y(p_ply_argument arg)
{
  PlyVarVec3Info* vi;
  ply_get_argument_user_data(arg, (void**)(&vi), 0);

  vi->data[vi->idx].y = ply_get_argument_value(arg);
  vi->idx++;
  return 1;
}

/// Set the z component of a vec3 slot.
int var_vec3_z(p_ply_argument arg)
{
  PlyVarVec3Info* vi;
  ply_get_argument_user_data(arg, (void**)(&vi), 0);

  vi->data[vi->idx].z = ply_get_argument_value(arg);
  vi->idx++;
  return 1;
}

/**
  Build the lists and call a callback method whenever a list is complete.
 */
int var_list(p_ply_argument arg)
{
  PlyVarListInfo* vi;
  ply_get_argument_user_data(arg, (void**)(&vi), 0);
  long len, val_index;

  ply_get_argument_property(arg, 0, &len, &val_index);
  // Is the current value the list length? Then initialize the var info
  if (val_index<0)
  {
    vi->elements.clear();
  }
  else
  {
    vi->elements.push_back(ply_get_argument_value(arg));
    // Is the list complete?
    if (val_index+1>=len)
    {
      int res = vi->callback(*vi);
      vi->idx++;
      return res;
    }
  }
  return 1;
}

/**
  Final list callback for the polygons.
 */
int var_vert_ids(PlyVarListInfo& vi)
{
  std::vector<int> loop;
  unsigned int size = vi.elements.size();
  double v;

  // Convert the list to ints...
  for(unsigned int i=0; i<size; i++)
  {
    if (vi.invertfaces)
      v = vi.elements[size-i-1];
    else
      v = vi.elements[i];

    // +0.1 to make sure the conversion to int will be the true int...
    loop.push_back(int(v+0.1));
  }
  vi.geom.setLoop(vi.idx, 0, loop);
  return 1;
}

int var_float_list(PlyVarListInfo& vi)
{
  // Create a new variable before the first element is set. The size of 
  // the first list determines the multiplicity.
  if (vi.idx==0)
  {
    vi.mult = vi.elements.size();
    vi.geom.newVariable(vi.varinfo.varname, 
			vi.varinfo.storageClass(), 
			vi.varinfo.vartype, 
			vi.mult, 
			vi.varinfo.ninstances);
    support3d::ISlot* slot = &(vi.geom.slot(vi.varinfo.varname));
    vi.data = ((support3d::ArraySlot<double>*)(slot))->dataPtr();
  }
  // Copy the list into the slot...
  for(int i=0; i<(int)(vi.elements.size()) && i<vi.mult; i++)
  {
    ((double*)(vi.data))[i] = vi.elements[i];
  }
  vi.data = ((double*)(vi.data))+vi.mult;
  return 1;
}

int var_int_list(PlyVarListInfo& vi)
{
  // Create a new variable before the first element is set. The size of 
  // the first list determines the multiplicity.
  if (vi.idx==0)
  {
    vi.mult = vi.elements.size();
    vi.geom.newVariable(vi.varinfo.varname, 
			vi.varinfo.storageClass(), 
			vi.varinfo.vartype, 
			vi.mult, 
			vi.varinfo.ninstances);
    support3d::ISlot* slot = &(vi.geom.slot(vi.varinfo.varname));
    vi.data = ((support3d::ArraySlot<int>*)(slot))->dataPtr();
  }
  // Copy the list into the slot...
  for(int i=0; i<(int)(vi.elements.size()) && i<vi.mult; i++)
  {
    ((int*)(vi.data))[i] = int(vi.elements[i]);
  }
  vi.data = ((int*)(vi.data))+vi.mult;
  return 1;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

/**
  Error callback
 */
static void error_callback(const char* message)
{
  throw support3d::ERuntimeError(std::string("RPly error: ")+std::string(message));
}

/**
  PLYReader
 */
class PLYReader
{
  private:
  /// PLY file handle.
  p_ply handle;

  /// Stores the number of vertices...
  long numverts;
  /// Stores the number of faces...
  long numfaces;

  /// Variable infos
  std::vector<PlyVarInfoBase*> varinfos;

  public:
  PLYReader() : handle(0), numverts(0), numfaces(0), varinfos() {}
  ~PLYReader() { close(); }

  /**
     Open a PLY file.
   */
  void open(std::string name)
  {
    close();
    // Open the file
    handle = ply_open(name.c_str(), error_callback);
    if (handle==0)
    {
      throw support3d::EIOError("Could not open file \""+name+"\".");
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
      numverts = 0;
      numfaces = 0;
    }
    deleteVarInfos();
  }

  /**
     Read the file header.

     The return value is a tuple (elements, comment, objinfo).
     comment and objinfo is a string that is a concatenation of all
     individual comment/objinfo lines in the file.
     elements is a list of the elements specified in the header. Each
     element is described by a 3-tuple (name, ninstances, properties).
     name is the element's name (such as "vertex" and "face"), ninstances
     specifies how many items are stored in the file and properties is
     a list of element properties. Each property is again stored as
     a tuple (name, type, len_type, val_type) where name is the name
     of the property, type its type and len_type/val_type is the type
     of the length and the value of list properties. Each type is given
     as a string.

     This method also initializes the class attributes numverts and numfaces.
   */
  object readHeader()
  {
    // Read the header...
    if (!ply_read_header(handle))
    {
      throw support3d::ERuntimeError("Could not read PLY file header.");
    }

    // Iterate over the elements...
    p_ply_element el = ply_get_next_element(handle, 0);
    list elements;
    while(el!=0)
    {
      const char* elname;
      long ninstances;
      ply_get_element_info(el, &elname, &ninstances);
      if (strcmp(elname, "vertex")==0)
	numverts = ninstances;
      else if (strcmp(elname, "face")==0)
	numfaces = ninstances;
      // Iterate over the properties of the element...
      p_ply_property prop = ply_get_next_property(el, 0);
      list props;
      while(prop!=0)
      {
	const char* propname;
	e_ply_type proptype, len_type, val_type;
	ply_get_property_info(prop, &propname, &proptype, &len_type, &val_type);
	props.append(make_tuple(std::string(propname), 
				plyTypeToString(proptype),
				plyTypeToString(len_type),
				plyTypeToString(val_type)));
	prop = ply_get_next_property(el, prop);
      }

      elements.append(make_tuple(std::string(elname), ninstances, props));
      el = ply_get_next_element(handle, el);
    }

    // Iterate over the comments
    const char* s = ply_get_next_comment(handle, 0);
    list comments;
    while(s!=0)
    {
      comments.append(boost::python::str(s));
      s = ply_get_next_comment(handle, s);
    }

    // Iterate over the objinfos
    s = ply_get_next_obj_info(handle, 0);
    list objinfos;
    while(s!=0)
    {
      objinfos.append(boost::python::str(s));
      s = ply_get_next_obj_info(handle, s);
    }

    return make_tuple(elements, 
		      boost::python::str("\n").join(comments),
		      boost::python::str("\n").join(objinfos));
  }

  /**
     Read the data.

     This may only be called after readHeader() was called.

     vardecl is a list of variable declarations. Each declaration
     is a tuple (slot name, slot type, element name, propname) where propname
     may be a 3-tuple if type is a vec3 type. The vertices and faces
     must not be declared as they will be added automatically.

     \param geom An empty polyhedron geom that receives the result
     \param vardecl User variable declarations.
     \param invertfaces Specifies if the poly orientations should be inverted or not
   */
  void read(support3d::PolyhedronGeom& geom, object vardecl, bool invertfaces)
  {
    PlyVarInfoBase* varinfo = 0;
    int i;

    // Delete all existing variable infos (just in case)
    deleteVarInfos();

    // Allocate space for the verts and faces (the number was 
    // obtained in readHeader())
    geom.verts.resize(numverts);
    geom.setNumPolys(numfaces);

    // Register read callback functions for the declared variables...
    for (i=0; i<vardecl.attr("__len__")(); i++)
    {
      // Process one declaration tuple and fill a VariableInfo object
      VariableInfo vi = createVarInfo(vardecl[i]);
      
      if (!vi.valid)
	continue;

      // Scalar float or int?
      if (vi.numprops==1 && vi.proptype!=PLY_LIST)
      {
	int (*varcallback)(p_ply_argument) = 0;
	
	// Create a new variable and obtain the corresponding slot...
	geom.newVariable(vi.varname, vi.storageClass(), vi.vartype, 1, vi.ninstances);
	support3d::ISlot* slot = &(geom.slot(vi.varname));
	if (vi.vartype==support3d::INT)
	{
	  varinfo = new PlyVarIntInfo((support3d::ArraySlot<int>*)(slot));
	  varcallback = var_int;
	}
	else if (vi.vartype==support3d::FLOAT)
	{
	  varinfo = new PlyVarFloatInfo((support3d::ArraySlot<double>*)(slot));
	  varcallback = var_float;
        }
	varinfos.push_back(varinfo);
	// register the read callback
	ply_set_read_cb(handle, vi.element.c_str(), vi.prop1name.c_str(), varcallback, varinfo, 0);
      }
      // Int or float list?
      if (vi.numprops==1 && vi.proptype==PLY_LIST)
      {
	if (vi.vartype==support3d::INT)
	{
	  varinfo = new PlyVarListInfo(geom, var_int_list, vi);
	}
	else if (vi.vartype==support3d::FLOAT)
	{
	  varinfo = new PlyVarListInfo(geom, var_float_list, vi);
	}
	ply_set_read_cb(handle, vi.element.c_str(), vi.prop1name.c_str(), var_list, varinfo, 0);
      }
      // Vector valued?
      if (vi.numprops==3)
      {
	// Create a new variable and obtain the corresponding slot...
	geom.newVariable(vi.varname, vi.storageClass(), vi.vartype, 1, vi.ninstances);
	support3d::ISlot* slot = &(geom.slot(vi.varname));
	// x component
	varinfo = new PlyVarVec3Info((support3d::ArraySlot<support3d::vec3d>*)(slot));
	ply_set_read_cb(handle, vi.element.c_str(), vi.prop1name.c_str(), var_vec3_x, varinfo, 0);
	// y component
	varinfo = new PlyVarVec3Info((support3d::ArraySlot<support3d::vec3d>*)(slot));
	ply_set_read_cb(handle, vi.element.c_str(), vi.prop2name.c_str(), var_vec3_y, varinfo, 0);
	// z component
	varinfo = new PlyVarVec3Info((support3d::ArraySlot<support3d::vec3d>*)(slot));
	ply_set_read_cb(handle, vi.element.c_str(), vi.prop3name.c_str(), var_vec3_z, varinfo, 0);	
      }
    }
    
    // Register read callbacks for the verts and faces...
    PlyVarVec3Info vertinfox(&geom.verts);
    PlyVarVec3Info vertinfoy(&geom.verts);
    PlyVarVec3Info vertinfoz(&geom.verts);
    ply_set_read_cb(handle, "vertex", "x", var_vec3_x, &vertinfox, 0);
    ply_set_read_cb(handle, "vertex", "y", var_vec3_y, &vertinfoy, 0);
    ply_set_read_cb(handle, "vertex", "z", var_vec3_z, &vertinfoz, 0);
    PlyVarListInfo facesinfo(geom, var_vert_ids, invertfaces);
    ply_set_read_cb(handle, "face", "vertex_indices", var_list, &facesinfo, 0);

    // Read the model
    if (!ply_read(handle))
      throw support3d::ERuntimeError("Error while reading the PLY file.");

    deleteVarInfos();
  }

  //////////////////////////////////////////////////////////////////////
  private:

  /**
     Delete all variable infos.
   */
  void deleteVarInfos()
  {
    for(unsigned int i=0; i<varinfos.size(); i++)
    {
      delete varinfos[i];
    }
    varinfos.clear();
  }

  /**
     Create a VariableInfo object from a Python declaration tuple.

     If the tuple contains illegal values, the valid flag in the
     resulting object is false.
   */
  VariableInfo createVarInfo(object decl)
  {
    VariableInfo res;

    // Item 0: Slot name
    res.varname = extract<std::string>(decl[0]);
    // Item 1: Variable type
    res.vartype = extract<support3d::VarType>(decl[1]);
    // Item 2: Element name
    res.element = extract<std::string>(decl[2]);
    // Item 3: propnames (either 1 or 3 strings)
    res.numprops = extract<int>(decl[3].attr("__len__")());
    switch(res.numprops)
    {
     case 1: 
     {
       res.prop1name = extract<std::string>(decl[3][0]);
       res.valid = searchProperty(res.element.c_str(), 
				  res.prop1name.c_str(), 
				  &res.ninstances, &res.proptype, 
				  &res.len_type, &res.val_type);
       // Only int or float is allowed
       if (res.vartype!=support3d::INT && res.vartype!=support3d::FLOAT)
	 res.valid = false;
       break;
     }
     case 3: 
     {
       // Only a vec3 type is allowed
       if (res.vartype!=support3d::COLOR && 
	   res.vartype!=support3d::POINT &&
	   res.vartype!=support3d::VECTOR &&
	   res.vartype!=support3d::NORMAL)
       {
	 res.valid = false;
	 break;
       }
       res.prop1name = extract<std::string>(decl[3][0]);
       res.prop2name = extract<std::string>(decl[3][1]);
       res.prop3name = extract<std::string>(decl[3][2]);
       // Get the number of values (from one of the 3 properties)
       // If none of the properties is available, mark the result as invalid
       res.valid = searchProperty(res.element.c_str(), 
				  res.prop1name.c_str(), 
				  &res.ninstances, &res.proptype, 
				  &res.len_type, &res.val_type);
       if (res.valid)
	 break;
       res.valid = searchProperty(res.element.c_str(), 
				  res.prop2name.c_str(), &res.ninstances,
				  0,0,0);
       if (res.valid)
	 break;
       res.valid = searchProperty(res.element.c_str(), 
				  res.prop3name.c_str(), &res.ninstances,
				  0,0,0);
       break;
     }
     default: /* res remains invalid */ break;
    }
    return res;
  }


  /**
     Search a property and return info about it.

     len_type and val_type are only meaningful if proptype is PLY_LIST.

     \param element The element name
     \param property The property name inside the element
     \param[out] ninstances Receives the number of values (may be NULL)
     \param[out] proptype Receives the type of the property (may be NULL)
     \param[out] len_type Receives the type of the list length (may be NULL)
     \param[out] val_type Receives the type of a list item (may be NULL)
   */
  bool searchProperty(const char* element, const char* property, 
		      long* ninstances, e_ply_type* proptype, 
		      e_ply_type* len_type, e_ply_type* val_type)
  {
    // Iterate over the elements...
    p_ply_element el = ply_get_next_element(handle, 0);
    while(el!=0)
    {
      const char* elname;
      ply_get_element_info(el, &elname, ninstances);
      if (strcmp(elname, element)==0)
	break;
      el = ply_get_next_element(handle, el);
    }

    if (el==0)
      return false;

    // Iterate over the properties of the element...
    p_ply_property prop = ply_get_next_property(el, 0);
    while(prop!=0)
    {
      const char* propname;
      ply_get_property_info(prop, &propname, proptype, len_type, val_type);
      if (strcmp(propname, property)==0)
	return true;
      prop = ply_get_next_property(el, prop);
    }
    return false;
  }

  /**
     Convert a PLY type specification into a string.
   */
  std::string plyTypeToString(e_ply_type t)
  {
    switch(t)
    {
     case PLY_INT8: return "int8";
     case PLY_UINT8: return "uint8";      
     case PLY_INT16: return "int16";
     case PLY_UINT16: return "uint16";      
     case PLY_INT32: return "int32";
     case PLY_UIN32: return "uint32";      
     case PLY_FLOAT32: return "float32";
     case PLY_FLOAT64: return "float64";
     case PLY_CHAR: return "char";
     case PLY_UCHAR: return "uchar";
     case PLY_SHORT: return "short";
     case PLY_USHORT: return "ushort";
     case PLY_INT: return "int";
     case PLY_UINT: return "uint";      
     case PLY_FLOAT: return "float";
     case PLY_DOUBLE: return "double";
     case PLY_LIST: return "list";
     default: return "?";
    }
  }
};

//////////////////////////////////////////////////////////////////////
void rply_read()
{
  class_<PLYReader>("PLYReader", init<>())
    .def("open", &PLYReader::open)
    .def("close", &PLYReader::close)
    .def("readHeader", &PLYReader::readHeader)
    .def("read", &PLYReader::read)
  ;
}
