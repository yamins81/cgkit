/*
  vec4 wrapper
 */

#include <sstream>
#include <boost/python.hpp>
#include "vec4.h"
#include "py_exceptions.h"

using namespace boost::python;
using namespace support3d;

boost::python::str repr(vec4d* self)
{
  std::ostringstream s;
  s<<*self;
  return boost::python::str(s.str());
}

vec4d __pos__(vec4d* self)
{
  return *self;
}

int __len__(vec4d* self)
{
  return 4;
}

double __getitem__(vec4d* self, int idx)
{
  switch(idx)
  {
  case 0: return self->x; 
  case 1: return self->y; 
  case 2: return self->z;
  case 3: return self->w;
  default: throw EPyIndexError("index out of range");
  }
}

void __setitem__(vec4d* self, int idx, double val)
{
  switch(idx)
  {
  case 0: self->x = val; break;
  case 1: self->y = val; break;
  case 2: self->z = val; break;
  case 3: self->w = val; break;
  default: throw EPyIndexError("index out of range");
  }
}

// pickle & copy support
struct vec4_pickle_suite : boost::python::pickle_suite
{
  // Return the arguments that will be passed to the constructor
  static tuple getinitargs(vec4d const& self)
  {
    return make_tuple(self.x, self.y, self.z, self.w);
  }
};


void class_vec4()
{
  vec4d (vec4d::*normalize)() const = &vec4d::normalize;

  class_<vec4d>("vec4")
    .def(init<double>())
    .def(init<double,double>())
    .def(init<double,double,double>())
    .def(init<double,double,double,double>())
    .def(init<const vec4d&>())
    .def_readwrite("x", &vec4d::x)
    .def_readwrite("y", &vec4d::y)
    .def_readwrite("z", &vec4d::z)
    .def_readwrite("w", &vec4d::w)
    .def_readwrite("t", &vec4d::w)
    .def(self+=self)
    .def(self-=self)
    .def(self*=double())
    .def(self/=double())
    .def(self%=double())
    .def(self%=self)
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
    .def("__repr__", &repr)
    .def("__pos__", __pos__)
    .def("__len__", __len__)
    .def("__getitem__", __getitem__)
    .def("__setitem__", __setitem__)
    .def("__abs__", &vec4d::length)
    .def("length", &vec4d::length)
    .def("normalize", normalize)
    .def("min", &vec4d::min)
    .def("max", &vec4d::max)
    .def("minIndex", &vec4d::minIndex)
    .def("maxIndex", &vec4d::maxIndex)
    .def("minAbs", &vec4d::minAbs)
    .def("maxAbs", &vec4d::maxAbs)
    .def("minAbsIndex", &vec4d::minAbsIndex)
    .def("maxAbsIndex", &vec4d::maxAbsIndex)
    .def_pickle(vec4_pickle_suite())
  ;
}
