/*
  Main wrapper file. 
 */

#include <boost/python.hpp>
#include <iostream>

#include "vec3.h"
#include "slot.h"
#include "py_slot.h"
#include "boost/shared_ptr.hpp"
#include "worldobject.h"
#include "py_exceptions.h"
#include "debuginfo.h"

#include "component.h"
/*namespace {
#include <windows.h>
#include <GL/gl.h>
}*/

#ifdef WIN32
// for FPU state control
#include <float.h>
#endif

using namespace boost::python;
using namespace support3d;

// noises
void def_noises();

// py_vec3
void class_vec3();
// py_vec4
void class_vec4();
// py_mat3
void class_mat3();
// py_mat4
void class_mat4();
// py_quat
void class_quat();

// py_slots
void class_Slots();
void class_Slots2();
void class_Slots3();
void class_Slots4();
void class_ArraySlots();
void class_ArraySlots2();
void class_Mat3ArraySlot();
void class_Mat4ArraySlot();
void class_ArraySlots4();

// py_boundingbox
void class_BoundingBox();

// py_component
void class_Component();

// py_worldobject
void class_WorldObject();

// py_worldobject3
void class_WorldObjectSlots();

// py_material
void class_Material();

// py_glmaterial
void class_GLMaterial();

// py_geoms
void class_Geoms();

// py_spheregeom
void class_SphereGeom();

// py_ccylindergeom
void class_CCylinderGeom();

// py_torusgeom
void class_TorusGeom();

// py_boxgeom
void class_BoxGeom();

// py_planegeom
void class_PlaneGeom();

// py_trimeshgeom
void class_TriMeshGeom();

// py_polyhedrongeom
void class_PolyhedronGeom();

// py_drawgeom
void class_DrawGeom();

// py_lightsource
void class_LightSource();

// py_glpointlight
void class_GLPointLight();

// py_glspotlight
void class_GLSpotLight();

// py_gldistantlight
void class_GLDistantLight();

// py_glrenderer
void class_GLRenderInstance();


// rply
void rply_read();
void rply_write();

// py_massproperties
void class_MassProperties();


// lib3ds
#ifdef LIB3DS_AVAILABLE
void lib3ds();
#endif

// cyberx3d
#ifdef CYBERX3D_AVAILABLE
void cyberx3d();
#endif

// ogre
#ifdef OGRE_AVAILABLE
void class_OgreCore();
#endif

// irrlicht
#ifdef IRR_AVAILABLE
void class_IrrCore();
#endif

#ifdef OSG_AVAILABLE
void class_OsgCore();
#endif

/**
 */
void StopIterationTranslator(const StopIteration& exc) 
{ 
  PyErr_SetString(PyExc_StopIteration, ""); 
} 

/**
 */
void MemoryErrorTranslator(const EMemoryError& exc) 
{ 
  PyErr_SetString(PyExc_MemoryError, exc.what()); 
} 

/**
 */
void IOErrorTranslator(const EIOError& exc) 
{ 
  PyErr_SetString(PyExc_IOError, exc.what()); 
} 

/**
 */
void RuntimeErrorTranslator(const ERuntimeError& exc) 
{ 
  PyErr_SetString(PyExc_RuntimeError, exc.what()); 
} 

/**
 */
void PyIndexErrorTranslator(const EPyIndexError& exc) 
{ 
  PyErr_SetString(PyExc_IndexError, exc.what()); 
} 

/**
 */
void IndexErrorTranslator(const EIndexError& exc) 
{ 
  PyErr_SetString(PyExc_IndexError, exc.what()); 
} 

/**
 */
void ZeroDivisionErrorTranslator(const EZeroDivisionError& exc) 
{ 
  PyErr_SetString(PyExc_ZeroDivisionError, exc.what()); 
} 

/**
 */
void ValueErrorTranslator(const EValueError& exc) 
{ 
  PyErr_SetString(PyExc_ValueError, exc.what()); 
} 

/**
 */
void KeyErrorTranslator(const EKeyError& exc) 
{ 
  PyErr_SetString(PyExc_KeyError, exc.what()); 
} 

/**
 */
void NotImplementedErrorTranslator(const ENotImplementedError& exc) 
{ 
  PyErr_SetString(PyExc_NotImplementedError, exc.what()); 
} 


/**
  Translator function for the EIncompatibleSlotTypes exception.

  The exception is translated into a ValueError exception.
 */
void IncompatibleSlotTypesTranslator(const EIncompatibleSlotTypes& exc) 
{ 
  //  PyErr_SetString(PyExc_ValueError, "Incompatible slot types"); 
  PyErr_SetString(PyExc_ValueError, exc.what()); 
} 

/**
  Translator function for the ENoInputConnectionsAllowed exception.

  The exception is translated into a ValueError exception.
 */
void NoInputConnectionsAllowedTypesTranslator(const ENoInputConnectionsAllowed& exc) 
{ 
  //  PyErr_SetString(PyExc_ValueError, "Incompatible slot types"); 
  PyErr_SetString(PyExc_ValueError, exc.what()); 
} 

//////////////////////////////////////////////////////////////////////

// Convert a Python sequence to a vec3
struct vec3_from_sequence
{
  vec3_from_sequence()
  {
    boost::python::converter::registry::push_back(
      &convertible,
      &construct,
      boost::python::type_id<vec3d>());
  }

  static void* convertible(PyObject* obj_ptr)
  {
    if (!PySequence_Check(obj_ptr))
      return 0;
    int size = PySequence_Size(obj_ptr);
    if (size==-1)
    {
      PyErr_Clear();
      return 0;
    }
    if (size!=3)
      return 0;
    // Check that the first item is a float (it is assumed that the other
    // items are also floats then)
    PyObject* item = PySequence_ITEM(obj_ptr, 0);
    if (item==0)
    {
      PyErr_Clear();
      return 0;
    }
    int is_num = PyNumber_Check(item);
    Py_DECREF(item);
    if (!is_num)
      return 0;
    
    return obj_ptr;
  }

  static void construct(
      PyObject* obj_ptr,
      boost::python::converter::rvalue_from_python_stage1_data* data)
  {
    double values[3] = {0.0, 0.0, 0.0};
    for(int i=0; i<PySequence_Size(obj_ptr); i++)
    {
      PyObject* item = PySequence_ITEM(obj_ptr, i);
      if (item==0)
	boost::python::throw_error_already_set();
      values[i] = PyFloat_AsDouble(item);
      Py_DECREF(item);
      if (PyErr_Occurred()!=0)
      {
	boost::python::throw_error_already_set();
      }
    }
      
    void* storage = (
        (boost::python::converter::rvalue_from_python_storage<vec3d>*)
         data)->storage.bytes;
    new (storage) vec3d(values[0], values[1], values[2]);
    data->convertible = storage;
  }
};

// Convert a Python sequence to a vec4
struct vec4_from_sequence
{
  vec4_from_sequence()
  {
    boost::python::converter::registry::push_back(
      &convertible,
      &construct,
      boost::python::type_id<vec4d>());
  }

  static void* convertible(PyObject* obj_ptr)
  {
    if (!PySequence_Check(obj_ptr))
      return 0;
    int size = PySequence_Size(obj_ptr);
    if (size==-1)
    {
      PyErr_Clear();
      return 0;
    }
    if (size!=4)
      return 0;
    // Check that the first item is a float (it is assumed that the other
    // items are also floats then)
    PyObject* item = PySequence_ITEM(obj_ptr, 0);
    if (item==0)
    {
      PyErr_Clear();
      return 0;
    }
    int is_num = PyNumber_Check(item);
    Py_DECREF(item);
    if (!is_num)
      return 0;

    return obj_ptr;
  }

  static void construct(
      PyObject* obj_ptr,
      boost::python::converter::rvalue_from_python_stage1_data* data)
  {
    double values[4] = {0.0, 0.0, 0.0, 0.0};
    for(int i=0; i<PySequence_Size(obj_ptr); i++)
    {
      PyObject* item = PySequence_ITEM(obj_ptr, i);
      if (item==0)
         boost::python::throw_error_already_set();
      values[i] = PyFloat_AsDouble(item);
      Py_DECREF(item);
      if (PyErr_Occurred()!=0)
      {
         boost::python::throw_error_already_set();
      }
    }
      
    void* storage = (
        (boost::python::converter::rvalue_from_python_storage<vec4d>*)
         data)->storage.bytes;
    new (storage) vec4d(values[0], values[1], values[2], values[3]);
    data->convertible = storage;
  }
};

//////////////////////////////////////////////////////////////////////

double _setEpsilon(double eps)
{
  double res = vec3d::epsilon;
  vec3d::epsilon = eps;
  return res;
}

double _getEpsilon()
{
  return vec3d::epsilon;
}

#ifdef WIN32
int getFPUControlState()
{
  return _controlfp(0,0);
}

void setFPUPrecision(int n)
{
  switch(n)
  {
   case 0: _controlfp(_PC_24, _MCW_PC); break;
   case 1: _controlfp(_PC_53, _MCW_PC); break;
   case 2: _controlfp(_PC_64, _MCW_PC); break;
  }
}
#endif

//////////////////////////////////////////////////////////////////////

void foo()
{
}

//////////////////////////////////////////////////////////////////////

BOOST_PYTHON_MODULE(_core)
{
  register_exception_translator<StopIteration>(&StopIterationTranslator);
  register_exception_translator<EValueError>(&ValueErrorTranslator);
  register_exception_translator<EKeyError>(&KeyErrorTranslator);
  register_exception_translator<EPyIndexError>(&PyIndexErrorTranslator);
  register_exception_translator<EMemoryError>(&MemoryErrorTranslator);
  register_exception_translator<EIndexError>(&IndexErrorTranslator);
  register_exception_translator<EIOError>(&IOErrorTranslator);
  register_exception_translator<ERuntimeError>(&RuntimeErrorTranslator);
  register_exception_translator<EIncompatibleSlotTypes>(&IncompatibleSlotTypesTranslator);
  register_exception_translator<ENoInputConnectionsAllowed>(&NoInputConnectionsAllowedTypesTranslator);
  register_exception_translator<EZeroDivisionError>(&ZeroDivisionErrorTranslator);
  register_exception_translator<ENotImplementedError>(&NotImplementedErrorTranslator);
  
  // noise
  def_noises();
  
  // cgtypes
  class_vec3();
  class_vec4();
  class_mat3();
  class_mat4();
  class_quat();
  
  // slot
  class_Slots();
  class_Slots2();
  class_Slots3();
  class_Slots4();
  class_ArraySlots();
  class_ArraySlots2();
  class_Mat3ArraySlot();
  class_Mat4ArraySlot();
  class_ArraySlots4();

  // boundingbox
  class_BoundingBox();

  // component
  class_Component();

  // worldobject
  class_WorldObjectSlots();
  class_WorldObject();

  // material
  class_Material();

  // glmaterial
  class_GLMaterial();

  // geoms
  class_Geoms();

  // SphereGeom
  class_SphereGeom();

  // CCylinderGeom
  class_CCylinderGeom();

  // TorusGeom
  class_TorusGeom();

  // BoxGeom
  class_BoxGeom();

  // PlaneGeom
  class_PlaneGeom();

  // TriMeshGeom
  class_TriMeshGeom();

  // PolyhedronGeom
  class_PolyhedronGeom();

  // DrawGeom
  class_DrawGeom();

  // LightSource
  class_LightSource();

  // GLPointLight
  class_GLPointLight();

  // GLSpotLight
  class_GLSpotLight();

  // GLDistantLight
  class_GLDistantLight();

  // GLRenderInstance
  class_GLRenderInstance();

  // MassProperties
  class_MassProperties();


  //  implicitly_convertible<ArraySlot<int>,ArraySlotWrapper<int> >();
  //  implicitly_convertible<ArraySlotWrapper<int>,ArraySlot<int> >();

  // rply
  rply_read();
  rply_write();

  #ifdef LIB3DS_AVAILABLE
  lib3ds();
  #endif

  #ifdef CYBERX3D_AVAILABLE
  cyberx3d();
  #endif
  
  #ifdef OGRE_AVAILABLE
  class_OgreCore();
  #endif

  #ifdef IRR_AVAILABLE
  class_IrrCore();
  #endif

  #ifdef OSG_AVAILABLE
  class_OsgCore();
  #endif

  // Type conversion
  vec3_from_sequence();
  vec4_from_sequence();

  // Test
  def("_set_debug_flag", set_debug_flag);
  def("_get_debug_flag", get_debug_flag);
  def("_setEpsilon", _setEpsilon);
  def("_getEpsilon", _getEpsilon);
  def("foo", &foo);
  //  def("drawGL_test", &drawGL_test);

  #ifdef WIN32
  def("getFPUControlState", getFPUControlState);
  def("setFPUPrecision", setFPUPrecision);
  #endif
}
