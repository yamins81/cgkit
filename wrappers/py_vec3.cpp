/*
  vec3 wrapper
 */

#include <sstream>
#include <boost/python.hpp>
#include "vec3.h"
#include "py_exceptions.h"

using namespace boost::python;
using namespace support3d;

boost::python::str repr(vec3d* self)
{
  std::ostringstream s;
  s<<*self;
  return boost::python::str(s.str());
}

int __len__(vec3d* self)
{
  return 3;
}

double __getitem__(vec3d* self, int idx)
{
  switch(idx)
  {
  case 0: return self->x; 
  case 1: return self->y; 
  case 2: return self->z;
  default: throw EPyIndexError("index out of range");
  }
}

void __setitem__(vec3d* self, int idx, double val)
{
  switch(idx)
  {
  case 0: self->x = val; break;
  case 1: self->y = val; break;
  case 2: self->z = val; break;
  default: throw EPyIndexError("index out of range");
  }
}

vec3d __pos__(vec3d* self)
{
  return *self;
}

double angle_method(vec3d* self, const vec3d& b)
{
  return angle(*self, b);
}

// pickle & copy support
struct vec3_pickle_suite : boost::python::pickle_suite
{
  // Return the arguments that will be passed to the constructor
  static tuple getinitargs(vec3d const& self)
  {
    return make_tuple(self.x, self.y, self.z);
  }
};

void class_vec3()
{
  vec3d (vec3d::*norm1)() const = &vec3d::normalize;
  vec3d (vec3d::*cross)(const vec3d& b) const = &vec3d::cross;

  class_<vec3d>("vec3")
    .def(init<double>())
    .def(init<double,double>())
    .def(init<double,double,double>())
    .def(init<const vec3d&>())
    .def_readwrite("x", &vec3d::x)
    .def_readwrite("y", &vec3d::y)
    .def_readwrite("z", &vec3d::z)
    .def(self+=self)
    .def(self-=self)
    .def(self*=double())
    .def(self/=double())
    .def(self%=self)
    .def(self%=double())
    .def(self+self)
    .def(self-self)
    .def(-self)
    .def(double()*self)
    .def(self*double())
    .def(self*self)
    .def(self/double())
    .def(self%double())
    .def(self%self)
    .def(self==self)
    .def(self!=self)
    .def(self<self)
    .def(self<=self)
    .def(self>self)
    .def(self>=self)
    // See http://mail.python.org/pipermail/c++-sig/2004-February/006806.html
    // for the "self_ns::" 
    .def(self_ns::str(self))
    .def("__repr__", repr)
    .def("__pos__", __pos__)
    .def("__len__", __len__)
    .def("__getitem__", __getitem__)
    .def("__setitem__", __setitem__)
    .def("__abs__", &vec3d::length)
    .def("length", &vec3d::length)
    .def("normalize", norm1)
    .def("cross", cross)
    .def("ortho", &vec3d::ortho)
    .def("reflect", &vec3d::reflect, arg("N"))
    .def("refract", &vec3d::refract, (arg("N"), arg("eta")))
    .def("angle", angle_method)
    .def("min", &vec3d::min)
    .def("max", &vec3d::max)
    .def("minIndex", &vec3d::minIndex)
    .def("maxIndex", &vec3d::maxIndex)
    .def("minAbs", &vec3d::minAbs)
    .def("maxAbs", &vec3d::maxAbs)
    .def("minAbsIndex", &vec3d::minAbsIndex)
    .def("maxAbsIndex", &vec3d::maxAbsIndex)
    .def_pickle(vec3_pickle_suite())
  ;
}
