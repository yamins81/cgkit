/*
 Capped cylinder geom
 */

#include <boost/python.hpp>
#include "ccylindergeom.h"

using namespace boost::python;
using namespace support3d;


// get/set for "radius" property
double getRadius(CCylinderGeom* self)
{
  return self->radius.getValue();
}

void setRadius(CCylinderGeom* self, double r)
{
  self->radius.setValue(r);
}

// get/set for "length" property
double getLength(CCylinderGeom* self)
{
  return self->length.getValue();
}

void setLength(CCylinderGeom* self, double l)
{
  self->length.setValue(l);
}

// get/set for "segmentsu" property
int getSegmentsu(CCylinderGeom* self)
{
  return self->segmentsu.getValue();
}

void setSegmentsu(CCylinderGeom* self, int s)
{
  self->segmentsu.setValue(s);
}

// get/set for "segmentsvl" property
int getSegmentsvl(CCylinderGeom* self)
{
  return self->segmentsvl.getValue();
}

void setSegmentsvl(CCylinderGeom* self, int s)
{
  self->segmentsvl.setValue(s);
}

// get/set for "segmentsvr" property
int getSegmentsvr(CCylinderGeom* self)
{
  return self->segmentsvr.getValue();
}

void setSegmentsvr(CCylinderGeom* self, int s)
{
  self->segmentsvr.setValue(s);
}

// get for "inertiatensor" property
mat3d getInertiaTensor(CCylinderGeom* self)
{
  return self->inertiatensor.getValue();
}

// get for "cog" property
vec3d getCog(CCylinderGeom* self)
{
  return self->cog.getValue();
}


/*Slot<double>& radiusSlot(CCylinderGeom* self)
{
  return *(self->radius.get());
  }*/

void class_CCylinderGeom()
{
  class_<ProceduralSlot<mat3d, CCylinderGeom>, bases<Slot<mat3d> > >("CCylinderGeom_InertiatensorSlot", no_init);

  class_<CCylinderGeom, bases<GeomObject> >("CCylinderGeom", 
					 "The capped cylinder geometry class.",
					 init<optional<double, double, int, int, int> >())
    .add_property("radius", getRadius, setRadius)
    .add_property("length", getLength, setLength)
    .add_property("segmentsu", getSegmentsu, setSegmentsu)
    .add_property("segmentsvl", getSegmentsvl, setSegmentsvl)
    .add_property("segmentsvr", getSegmentsvr, setSegmentsvr)
    .add_property("cog", getCog)
    .add_property("inertiatensor", getInertiaTensor)
    .def_readonly("radius_slot", &CCylinderGeom::radius)
    .def_readonly("length_slot", &CCylinderGeom::length)
    .def_readonly("segmentsu_slot", &CCylinderGeom::segmentsu)
    .def_readonly("segmentsvl_slot", &CCylinderGeom::segmentsvl)
    .def_readonly("segmentsvr_slot", &CCylinderGeom::segmentsvr)
    .def_readonly("cog_slot", &CCylinderGeom::cog)
    .def_readonly("inertiatensor_slot", &CCylinderGeom::inertiatensor)
  ;

}
