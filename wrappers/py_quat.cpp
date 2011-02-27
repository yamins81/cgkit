/*
  quat wrapper
 */

#include <sstream>
#include <boost/python.hpp>
#include "quat.h"

using namespace boost::python;
using namespace support3d;

boost::python::str __repr__(quatd* self)
{
  std::ostringstream s;
  s<<*self;
  return boost::python::str(s.str());
}

quatd __pos__(quatd* self)
{
  return *self;
}

tuple toAngleAxis(quatd* self)
{
  double angle;
  vec3d axis;

  self->toAngleAxis(angle, axis);
  return make_tuple(angle, axis);
}

// pickle & copy support
struct quat_pickle_suite : boost::python::pickle_suite
{
  // Return the arguments that will be passed to the constructor
  static tuple getinitargs(quatd const& self)
  {
    return make_tuple(self.w, self.x, self.y, self.z);
  }
};


void class_quat()
{
  quatd (quatd::*normalize)() const = &quatd::normalize;
  quatd (quatd::*conjugate)() const = &quatd::conjugate;
  quatd (quatd::*inverse)() const = &quatd::inverse;
  quatd& (quatd::*fromMat3)(const mat3d&) = &quatd::fromMat;
  quatd& (quatd::*fromMat4)(const mat4d&) = &quatd::fromMat;
  mat3d (quatd::*toMat3)() const = &quatd::toMat3;
  mat4d (quatd::*toMat4)() const = &quatd::toMat4;
  quatd (quatd::*log)() const = &quatd::log;
  quatd (quatd::*exp)() const = &quatd::exp;
  vec3d (quatd::*rotateVec)(const vec3d& v) const = &quatd::rotateVec;

  class_<quatd>("quat")
    .def(init<double>())
    .def(init<double,double,double,double>())
    .def(init<const quatd&>())
    .def_readwrite("w", &quatd::w)
    .def_readwrite("x", &quatd::x)
    .def_readwrite("y", &quatd::y)
    .def_readwrite("z", &quatd::z)
    .def(self+=self)
    .def(self-=self)
    .def(self*=double())
    .def(self/=double())
    .def(self+self)
    .def(self-self)
    .def(-self)
    .def(double()*self)
    .def(self*double())
    .def(self*self)
    .def(self/double())
    .def(self==self)
    .def(self!=self)
    .def(self_ns::str(self))
    .def("__repr__", __repr__)
    .def("__pos__", __pos__)
    .def("__abs__", &quatd::abs)
    .def("normalize", normalize)
    .def("conjugate", conjugate)
    .def("inverse", inverse)
    .def("dot", &quatd::dot)
    .def("fromMat", fromMat3, return_self<>(), arg("matrix"))
    .def("fromMat", fromMat4, return_self<>(), arg("matrix"))
    .def("toMat3", toMat3)
    .def("toMat4", toMat4)
    .def("toAngleAxis", toAngleAxis)
    .def("fromAngleAxis", &quatd::fromAngleAxis, return_self<>(), (arg("angle"), arg("axis")))
    .def("log", log)
    .def("exp", exp)
    .def("rotateVec", rotateVec)
    .def_pickle(quat_pickle_suite())
  ;

  quatd (*slerpd)(double t, const quatd& q0, const quatd& q1, bool shortest) = &slerp;
  quatd (*squadd)(double t, const quatd& a, const quatd& b,const quatd& c, const quatd& d) = &squad;
  def("slerp", slerpd, (arg("t"), arg("q0"), arg("q1"), arg("shortest")=true));
  def("squad", squadd, (arg("t"), arg("a"), arg("b"), arg("c"), arg("d")));
}
