/*
 Geoms
 */

#include <boost/python.hpp>
#include "geomobject.h"
#include "py_exceptions.h"


using namespace boost::python;
using namespace support3d;

// GeomObject wrapper class
class GeomObjectWrapper : public GeomObject
{
  PyObject* self;

  public:
  GeomObjectWrapper(PyObject* self_) : GeomObject(), self(self_) {}

  // when called from C++
  virtual int uniformCount() const
  {
    return call_method<int>(self, "uniformCount");
  }

  // when called from Python
  virtual int base_uniformCount() const
  {
    return GeomObject::uniformCount();
  }

  // when called from C++
  virtual int varyingCount() const
  {
    return call_method<int>(self, "varyingCount");
  }

  // when called from Python
  virtual int base_varyingCount() const
  {
    return GeomObject::varyingCount();
  }

  // when called from C++
  virtual int vertexCount() const
  {
    return call_method<int>(self, "vertexCount");
  }

  // when called from Python
  virtual int base_vertexCount() const
  {
    return GeomObject::vertexCount();
  }

  // when called from C++
  virtual int faceVaryingCount() const
  {
    return call_method<int>(self, "faceVaryingCount");
  }

  // when called from Python
  virtual int base_faceVaryingCount() const
  {
    return GeomObject::faceVaryingCount();
  }

  // when called from C++
  virtual int faceVertexCount() const
  {
    return call_method<int>(self, "faceVertexCount");
  }

  // when called from Python
  virtual int base_faceVertexCount() const
  {
    return GeomObject::faceVertexCount();
  }

  // this method is called when invoked from C++ code
  virtual boost::shared_ptr<SizeConstraintBase> slotSizeConstraint(VarStorage storage) const
  {
    return call_method<boost::shared_ptr<SizeConstraintBase> >(self, "slotSizeConstraint", storage);
  }

  // this method is called when invoked from Python code
  // (and only if the class didn't override the method)
  virtual boost::shared_ptr<SizeConstraintBase> base_slotSizeConstraint(VarStorage storage) const
  {
    throw ENotImplementedError("The method slotSizeConstraint() is not implemented.");    
  }

  virtual BoundingBox boundingBox()
  {
    return call_method<BoundingBox>(self, "boundingBox");
  }

  virtual void drawGL()
  {
    call_method<void>(self, "drawGL");
  }

  virtual void convert(GeomObject* target)
  {
    call_method<void>(self, "convert", target);
  }

};

////////////////////////////////////////////////////

// Primitive variable iterator
class _VariableIterator
{
  protected:
  GeomObject& geom;
  GeomObject::VariableIterator it;
  GeomObject::VariableIterator itend;

  public:
  _VariableIterator(GeomObject& ageom)
    : geom(ageom)
  {
    it = geom.variablesBegin();
    itend = geom.variablesEnd();
  }

  object __iter__()
  {
    return object(this);
  }

  object next()
  {
    if (it==itend)
    {
      throw StopIteration();
    }
    else
    {
      string name = it->first;
      const PrimVarInfo& info = it->second;
      it++;
      return make_tuple(name, info.storage, info.type, info.multiplicity);
    }
  }
  
};

_VariableIterator* iterVariables(GeomObject* self)
{
  return new _VariableIterator(*self);
}

// findVariable() method wrapper
object findVariable(GeomObject* self, string name)
{
  PrimVarInfo* info = self->findVariable(name);
  if (info==0)
  {
    return object();
  }
  else
  {
    return make_tuple(name, info->storage, info->type, info->multiplicity);
  }    
}

//////////////////////////////////////////////////////////////////////
void class_Geoms()
{
  enum_<VarStorage>("VarStorage")
    .value("CONSTANT", CONSTANT)
    .value("UNIFORM", UNIFORM)
    .value("VARYING", VARYING)
    .value("VERTEX", VERTEX)
    .value("FACEVARYING", FACEVARYING)
    .value("FACEVERTEX", FACEVERTEX)
    .value("USER", USER)
  ;

  enum_<VarType>("VarType")
    .value("INT", INT)
    .value("FLOAT", FLOAT)
    .value("STRING", STRING)
    .value("COLOR", COLOR)
    .value("POINT", POINT)
    .value("VECTOR", VECTOR)
    .value("NORMAL", NORMAL)
    .value("MATRIX", MATRIX)
    .value("HPOINT", HPOINT)
  ;

  class_<GeomObject, GeomObjectWrapper, bases<Component>, boost::noncopyable>("GeomObject", 
     "This is the base class for all geometry classes.\n\n"
     "This base class already handles the creation of user defined\n"
     "primitive variables.\n\n"
     "A derived class has to implement the following methods:\n"
     "boundingBox(), drawGL(), uniformCount(), varyingCount() and vertexCount()")
    .def("boundingBox", &GeomObject::boundingBox, &GeomObjectWrapper::boundingBox, 
	 "boundingBox() -> BoundingBox\n\n"
	 "Return the local axis aligned bounding box. The bounding box is\n"
	 "given with respect to the local transformation L (which is not \n"
	 "what you get from the transform slot of the world object).")

    .def("drawGL", &GeomObject::drawGL, &GeomObjectWrapper::drawGL,
	 "Draw the geometry using OpenGL commands.")

    .def("uniformCount", &GeomObject::uniformCount, &GeomObjectWrapper::base_uniformCount,
	 "uniformCount() -> int\n\n"
	 "Return the multiplicity of uniform variables.")

    .def("varyingCount", &GeomObject::varyingCount, &GeomObjectWrapper::base_varyingCount,
	 "varyingCount() -> int\n\n"
	 "Return the multiplicity of varying variables.")

    .def("vertexCount", &GeomObject::vertexCount, &GeomObjectWrapper::base_vertexCount,
	 "vertexCount() -> int\n\n"
	 "Return the multiplicity of vertex variables.")

    .def("faceVaryingCount", &GeomObject::faceVaryingCount, &GeomObjectWrapper::base_faceVaryingCount,
	 "faceVaryingCount() -> int\n\n"
	 "Return the multiplicity of facevarying variables.")

    .def("faceVertexCount", &GeomObject::faceVertexCount, &GeomObjectWrapper::base_faceVertexCount,
	 "faceVertexCount() -> int\n\n"
	 "Return the multiplicity of facevertex variables.")

    .def("slotSizeConstraint", &GeomObject::slotSizeConstraint, &GeomObjectWrapper::base_slotSizeConstraint,
	 "slotSizeConstraint(storage) -> SizeConstraint\n\n"
	 "Return a constraint object for primitive variable slots.")

    .def("newVariable", &GeomObjectWrapper::newVariable, 
	 (arg("name"), arg("storage"), arg("type"), arg("multiplicity")=1, arg("user_n")=0),
	 "newVariable(name, storage, type, multiplicity=1, user_n=0)\n\n"
         "Create a new primitive variable. storage must be a value from the\n"
	 "VarStorage class (CONSTANT, UNIFORM, VARYING, VERTEX, USER) and\n"
	 "type must be a value from the VarType class (INT, FLOAT, STRING,\n"
	 "COLOR, POINT, VECTOR, NORMAL, MATRIX, HPOINT). If multiplicity\n"
	 "is greater than 1, then an array with that size is created. The\n"
	 "argument user_n specifies the number of elements to create when\n"
	 "you pass USER as storage class.\n\n"
	 "Example:\n\n"
	 ">>> geom.newVariable('foo', VarStorage.UNIFORM, VarType.INT, multiplicity=3)")

    .def("deleteVariable", &GeomObjectWrapper::deleteVariable,
	 "deleteVariable(name)\n\n"
	 "Delete a primitive variable.")

    .def("deleteAllVariables", &GeomObjectWrapper::deleteAllVariables,
	 "deleteAllVariables()\n\n"
	 "Delete all primitive variables.")

    .def("findVariable", findVariable,
	 "findVariable(name) -> (name, storage, type, multiplicity) or None\n\n"
	 "Search for a particular primitive variable and return its descriptor.\n"
	 "None is returned if a variable with the given name cannot be found.")

    .def("iterVariables", iterVariables, return_value_policy<manage_new_object>(),
	 "iterVariables() -> VariableIterator\n\n"
	 "Iterate over all existing primitive variables.")

    .def("convert", &GeomObject::convert, &GeomObjectWrapper::convert,
	 "convert(targetgeom)\n\n"
	 "Convert the geom into another geom.")
  ;

  class_<_VariableIterator>("_VariableIterator", init<GeomObject&>())
    .def("__iter__", &_VariableIterator::__iter__)
    .def("next", &_VariableIterator::next)
  ;

}
