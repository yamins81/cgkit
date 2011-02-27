/*
  mat3 wrapper
 */

#include <sstream>
#include <boost/python.hpp>
#include "mat3.h"
#include "py_exceptions.h"

//#include <iostream.h>

using namespace boost::python;
using namespace support3d;

//typedef vec3<double> vec3d;
//typedef mat3<double> mat3d;

//////////////////////////////////////////////////////////////////////
// The following functions are used as class methods for the Python
// version of mat3.
// The C++ mat3 either doesn't have those methods or they have to
// be modified for Python.
//////////////////////////////////////////////////////////////////////

// toList() method (with no argument)
object toList0(const mat3d* self)
{
  list res;
  double array[9];
  self->toList(array);
  for(int i=0; i<9; i++)
    res.append(array[i]);
  return res;
}

// toList() method (with 1 argument)
object toList1(const mat3d* self, bool rowmajor)
{
  list res;
  double array[9];
  self->toList(array, rowmajor);
  for(int i=0; i<9; i++)
    res.append(array[i]);
  return res;
}

// identity() method (static)
mat3d identity()
{
  return mat3d(1);
}

// scaling() method (static)
mat3d scaling(const vec3d& s)
{
  mat3d res;
  res.setScaling(s);
  return res;
}

// rotation() method (static)
mat3d rotation(double angle, const vec3d& axis)
{
  mat3d res;
  res.setRotation(angle, axis);
  return res;
}

// fromToRotation() method (static)
mat3d fromToRotation(const vec3d& from, const vec3d& to)
{
  mat3d res;
  res.fromToRotation(from, to);
  return res;
}


// decompose() method
// Returns a tuple (rot, scale)
object decompose(mat3d* self)
{
  mat3d rot;
  vec3d scale;
  self->decompose(rot, scale);
  return make_tuple(rot, scale);
}

// fromEulerXYZ() method (static)
mat3d fromEulerXYZ(double x, double y, double z)
{
  mat3d res;
  res.setRotationXYZ(x, y, z);
  return res;
}

// fromEulerYZX() method (static)
mat3d fromEulerYZX(double x, double y, double z)
{
  mat3d res;
  res.setRotationYZX(x, y, z);
  return res;
}

// fromEulerZXY() method (static)
mat3d fromEulerZXY(double x, double y, double z)
{
  mat3d res;
  res.setRotationZXY(x, y, z);
  return res;
}

// fromEulerXZY() method (static)
mat3d fromEulerXZY(double x, double y, double z)
{
  mat3d res;
  res.setRotationXZY(x, y, z);
  return res;
}

// fromEulerYXZ() method (static)
mat3d fromEulerYXZ(double x, double y, double z)
{
  mat3d res;
  res.setRotationYXZ(x, y, z);
  return res;
}

// fromEulerZYX() method (static)
mat3d fromEulerZYX(double x, double y, double z)
{
  mat3d res;
  res.setRotationZYX(x, y, z);
  return res;
}

// toEulerXYZ() method
// Returns a tuple (x,y,z)
object toEulerXYZ(mat3d* self)
{
  double x,y,z;
  self->getRotationXYZ(x,y,z);
  return make_tuple(x,y,z);
}

// toEulerYZX() method
// Returns a tuple (x,y,z)
object toEulerYZX(mat3d* self)
{
  double x,y,z;
  self->getRotationYZX(x,y,z);
  return make_tuple(x,y,z);
}

// toEulerZXYY() method
// Returns a tuple (x,y,z)
object toEulerZXY(mat3d* self)
{
  double x,y,z;
  self->getRotationZXY(x,y,z);
  return make_tuple(x,y,z);
}

// toEulerXZY() method
// Returns a tuple (x,y,z)
object toEulerXZY(mat3d* self)
{
  double x,y,z;
  self->getRotationXZY(x,y,z);
  return make_tuple(x,y,z);
}

// toEulerYXZ() method
// Returns a tuple (x,y,z)
object toEulerYXZ(mat3d* self)
{
  double x,y,z;
  self->getRotationYXZ(x,y,z);
  return make_tuple(x,y,z);
}

// toEulerZYX() method
// Returns a tuple (x,y,z)
object toEulerZYX(mat3d* self)
{
  double x,y,z;
  self->getRotationZYX(x,y,z);
  return make_tuple(x,y,z);
}

boost::python::str repr(mat3d* self)
{
  std::ostringstream s;
  s<<*self;
  return boost::python::str(s.str());
}

mat3d __pos__(mat3d* self)
{
  return *self;
}

int __len__(mat3d* self)
{
  return 3;
}

vec3d __getitem__(mat3d* self, int idx)
{
  if (idx<0 || idx>2)
    throw EPyIndexError("index out of range");

  return self->getColumn(idx);
}

double __getitem_tup__(mat3d* self, tuple tup)
{
  int len = extract<int>(tup.attr("__len__")());
  if (len!=2)
  {
    throw EValueError("index tuple must be a 2-tuple");
  }
  int i = extract<int>(tup[0]);
  int j = extract<int>(tup[1]);
  if (i<0 || i>2 || j<0 || j>2)
      throw EIndexError();

  return self->at(i,j);
}

void __setitem__(mat3d* self, int idx, const vec3d& val)
{
  if (idx<0 || idx>2)
    throw EPyIndexError("index out of range");

  self->setColumn(idx, val);
}

void __setitem_tup__(mat3d* self, tuple tup, double val)
{
  int len = extract<int>(tup.attr("__len__")());
  if (len!=2)
  {
    throw EValueError("index tuple must be a 2-tuple");
  }
  int i = extract<int>(tup[0]);
  int j = extract<int>(tup[1]);
  if (i<0 || i>2 || j<0 || j>2)
      throw EIndexError();

  self->at(i,j) = val;
}

// pickle & copy support
struct mat3_pickle_suite : boost::python::pickle_suite
{
  // Return the arguments that will be passed to the constructor
  // (i.e. return a flattened matrix in row-major order)
  static tuple getinitargs(mat3d const& self)
  {
    return tuple(toList1(&self, true));
  }
};

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

void class_mat3()
{
  vec3d (mat3d::*getRow)(short i) const = &mat3d::getRow;
  mat3d& (mat3d::*setRow)(short i, const vec3d& r) = &mat3d::setRow;
  vec3d (mat3d::*getColumn)(short i) const = &mat3d::getColumn;
  mat3d& (mat3d::*setColumn)(short i, const vec3d& c) = &mat3d::setColumn;
  vec3d (mat3d::*getDiag)() const = &mat3d::getDiag;
  mat3d& (mat3d::*setDiag)(const vec3d& d) = &mat3d::setDiag;
  mat3d (mat3d::*transpose)() const = &mat3d::transpose;
  mat3d (mat3d::*inverse)() const = &mat3d::inverse;
  mat3d (mat3d::*ortho)() const = &mat3d::ortho;

  class_<mat3d>("mat3")
    .def(init<double>())
    .def(init<double,double,double,double,double,double,double,double,double>())
    .def(init<const mat3d&>())
    .def(self_ns::str(self))
    .def("__repr__", &repr)
    .def("__pos__", __pos__)
    .def("__len__", __len__)
    .def("__getitem__", __getitem__)
    .def("__getitem__", __getitem_tup__)
    .def("__setitem__", __setitem__)
    .def("__setitem__", __setitem_tup__)
    /*.def_readwrite("x", &vec3d::x)
    .def_readwrite("y", &vec3d::y)
    .def_readwrite("z", &vec3d::z)*/
    .def(self+=self)
    .def(self-=self)
    .def(self*=double())
    .def(self*=self)
    .def(self/=double())
    .def(self%=double())
    .def(self%=self)
    .def(self+self)
    .def(self-self)
    .def(-self)
    .def(self*other<vec3d>())
    .def(other<vec3d>()*self)
    .def(double()*self)
    .def(self*double())
    .def(self*self)
    .def(self/double())
    .def(self%double())
    .def(self%self)
    .def(self==self)
    .def(self!=self)
    .def("getColumn", getColumn, arg("index"))
    .def("setColumn", setColumn, return_self<>(), (arg("index"), arg("value")))
    .def("getRow", getRow, arg("index"))
    .def("setRow", setRow, return_self<>(), (arg("index"), arg("value")))
    .def("getDiag", getDiag)
    .def("setDiag", setDiag, return_self<>(), arg("value"))
    .def("toList", &toList0)
    .def("toList", &toList1, arg("rowmajor"))
    .def("identity", &identity)
    .staticmethod("identity")
    .def("scaling", &scaling, arg("s"))
    .staticmethod("scaling")
    .def("rotation", &rotation, (arg("angle"), arg("axis")))
    .staticmethod("rotation")
    .def("transpose", transpose)
    .def("determinant", &mat3d::determinant)
    .def("inverse", inverse)
    .def("scale", &mat3d::scale, return_internal_reference<>(), arg("s"))
    .def("rotate", &mat3d::rotate, return_internal_reference<>(), (arg("angle"), arg("axis")))
    .def("ortho", ortho)
    .def("decompose", &decompose)
    .def("fromEulerXYZ", fromEulerXYZ, (arg("x"), arg("y"), arg("z")))
    .staticmethod("fromEulerXYZ")
    .def("fromEulerYZX", fromEulerYZX, (arg("x"), arg("y"), arg("z")))
    .staticmethod("fromEulerYZX")
    .def("fromEulerZXY", fromEulerZXY, (arg("x"), arg("y"), arg("z")))
    .staticmethod("fromEulerZXY")
    .def("fromEulerXZY", fromEulerXZY, (arg("x"), arg("y"), arg("z")))
    .staticmethod("fromEulerXZY")
    .def("fromEulerYXZ", fromEulerYXZ, (arg("x"), arg("y"), arg("z")))
    .staticmethod("fromEulerYXZ")
    .def("fromEulerZYX", fromEulerZYX, (arg("x"), arg("y"), arg("z")))
    .staticmethod("fromEulerZYX")
    .def("toEulerXYZ", toEulerXYZ)
    .def("toEulerYZX", toEulerYZX)
    .def("toEulerZXY", toEulerZXY)
    .def("toEulerXZY", toEulerXZY)
    .def("toEulerYXZ", toEulerYXZ)
    .def("toEulerZYX", toEulerZYX)
    .def("fromToRotation", fromToRotation, (arg("_from"), arg("to")))
    .staticmethod("fromToRotation")
    .def_pickle(mat3_pickle_suite())
  ;
}

