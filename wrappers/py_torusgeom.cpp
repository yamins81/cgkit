/*
 Torus geom
 */

#include <boost/python.hpp>
#include "torusgeom.h"

using namespace boost::python;
using namespace support3d;


// get/set for "major" property
double getMajor(TorusGeom* self)
{
  return self->major.getValue();
}

void setMajor(TorusGeom* self, double r)
{
  self->major.setValue(r);
}

// get/set for "minor" property
double getMinor(TorusGeom* self)
{
  return self->minor.getValue();
}

void setMinor(TorusGeom* self, double r)
{
  self->minor.setValue(r);
}

// get/set for "segmentsu" property
int getSegmentsu(TorusGeom* self)
{
  return self->segmentsu.getValue();
}

void setSegmentsu(TorusGeom* self, int s)
{
  self->segmentsu.setValue(s);
}

// get/set for "segmentsv" property
int getSegmentsv(TorusGeom* self)
{
  return self->segmentsv.getValue();
}

void setSegmentsv(TorusGeom* self, int s)
{
  self->segmentsv.setValue(s);
}

// get for "inertiatensor" property
mat3d getInertiaTensor(TorusGeom* self)
{
  return self->inertiatensor.getValue();
}

// get for "cog" property
vec3d getCog(TorusGeom* self)
{
  return self->cog.getValue();
}


void class_TorusGeom()
{
  class_<ProceduralSlot<mat3d, TorusGeom>, bases<Slot<mat3d> > >("TorusGeom_InertiatensorSlot", no_init);

  class_<TorusGeom, bases<GeomObject> >("TorusGeom", 
				        "The torus geometry class.",
				        init<optional<double,double,int,int> >())
    .add_property("major", getMajor, setMajor)
    .add_property("minor", getMinor, setMinor)
    .add_property("segmentsu", getSegmentsu, setSegmentsu)
    .add_property("segmentsv", getSegmentsv, setSegmentsv)
    .add_property("cog", getCog)
    .add_property("inertiatensor", getInertiaTensor)
    .def_readonly("major_slot", &TorusGeom::major)
    .def_readonly("minor_slot", &TorusGeom::minor)
    .def_readonly("segmentsu_slot", &TorusGeom::segmentsu)
    .def_readonly("segmentsv_slot", &TorusGeom::segmentsv)
    .def_readonly("cog_slot", &TorusGeom::cog)
    .def_readonly("inertiatensor_slot", &TorusGeom::inertiatensor)
  ;

}
