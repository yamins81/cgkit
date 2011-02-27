/*
 Mass properties
 */

#include <boost/python.hpp>
#include "massproperties.h"


using namespace boost::python;
using namespace support3d;


tuple getCenterOfMass(MassProperties* self)
{
  return make_tuple(self->r[0], self->r[1], self->r[2]);
}

tuple getInertiaTensor(MassProperties* self)
{
  return make_tuple(self->J[0][0], self->J[1][1], self->J[2][2], 
		    self->J[0][1], self->J[0][2], self->J[1][2]);
}


void class_MassProperties()
{
  class_<FACE>("FACE", init<>())
    .def_readwrite("numVerts", &FACE::numVerts)
    .def("init", &FACE::init)
    .def("setVert", &FACE::setVert)
  ;


  class_<MassProperties>("MassProperties", init<>())
    .def_readonly("density", &MassProperties::density)
    .def_readonly("mass", &MassProperties::mass)
    .def_readonly("volume", &MassProperties::volume)
    .add_property("r", getCenterOfMass)
    .add_property("J", getInertiaTensor)

    .def("setDensity", &MassProperties::setDensity)
    .def("setMass", &MassProperties::setMass)
    .def("meshBegin", &MassProperties::meshBegin)
    .def("face", &MassProperties::face)
    .def("meshEnd", &MassProperties::meshEnd)
  ;
}
