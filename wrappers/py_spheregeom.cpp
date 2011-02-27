/*
 Sphere geom
 */

#include <boost/python.hpp>
#include "spheregeom.h"

using namespace boost::python;
using namespace support3d;


// get/set for "radius" property
double getRadius(SphereGeom* self)
{
  return self->radius.getValue();
}

void setRadius(SphereGeom* self, double r)
{
  self->radius.setValue(r);
}

// get/set for "segmentsu" property
int getSegmentsu(SphereGeom* self)
{
  return self->segmentsu.getValue();
}

void setSegmentsu(SphereGeom* self, int s)
{
  self->segmentsu.setValue(s);
}

// get/set for "segmentsv" property
int getSegmentsv(SphereGeom* self)
{
  return self->segmentsv.getValue();
}

void setSegmentsv(SphereGeom* self, int s)
{
  self->segmentsv.setValue(s);
}

// get for "inertiatensor" property
mat3d getInertiaTensor(SphereGeom* self)
{
  return self->inertiatensor.getValue();
}

// get for "cog" property
vec3d getCog(SphereGeom* self)
{
  return self->cog.getValue();
}


/*Slot<double>& radiusSlot(SphereGeom* self)
{
  return *(self->radius.get());
  }*/

void class_SphereGeom()
{
  class_<ProceduralSlot<mat3d, SphereGeom>, bases<Slot<mat3d> > >("SphereGeom_InertiatensorSlot", no_init);

  class_<SphereGeom, bases<GeomObject> >("SphereGeom", 
					 "The sphere geometry class.",
					 init<optional<double,int,int> >())
    .add_property("radius", getRadius, setRadius)
    .add_property("segmentsu", getSegmentsu, setSegmentsu)
    .add_property("segmentsv", getSegmentsv, setSegmentsv)
    .add_property("cog", getCog)
    .add_property("inertiatensor", getInertiaTensor)
    .def_readonly("radius_slot", &SphereGeom::radius)
    .def_readonly("segmentsu_slot", &SphereGeom::segmentsu)
    .def_readonly("segmentsv_slot", &SphereGeom::segmentsv)
    .def_readonly("cog_slot", &SphereGeom::cog)
    .def_readonly("inertiatensor_slot", &SphereGeom::inertiatensor)
  ;

}
