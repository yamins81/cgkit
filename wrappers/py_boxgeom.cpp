/*
 Box geom
 */

#include <boost/python.hpp>
#include "boxgeom.h"

using namespace boost::python;
using namespace support3d;


// get/set for "lx" property
double getLx(BoxGeom* self)
{
  return self->lx.getValue();
}

void setLx(BoxGeom* self, double l)
{
  self->lx.setValue(l);
}

// get/set for "ly" property
double getLy(BoxGeom* self)
{
  return self->ly.getValue();
}

void setLy(BoxGeom* self, double l)
{
  self->ly.setValue(l);
}

// get/set for "lz" property
double getLz(BoxGeom* self)
{
  return self->lz.getValue();
}

void setLz(BoxGeom* self, double l)
{
  self->lz.setValue(l);
}

// get/set for "segmentsx" property
int getSegmentsx(BoxGeom* self)
{
  return self->segmentsx.getValue();
}

void setSegmentsx(BoxGeom* self, int s)
{
  self->segmentsx.setValue(s);
}

// get/set for "segmentsy" property
int getSegmentsy(BoxGeom* self)
{
  return self->segmentsy.getValue();
}

void setSegmentsy(BoxGeom* self, int s)
{
  self->segmentsy.setValue(s);
}

// get/set for "segmentsz" property
int getSegmentsz(BoxGeom* self)
{
  return self->segmentsz.getValue();
}

void setSegmentsz(BoxGeom* self, int s)
{
  self->segmentsz.setValue(s);
}


// get for "inertiatensor" property
mat3d getInertiaTensor(BoxGeom* self)
{
  return self->inertiatensor.getValue();
}

// get for "cog" property
vec3d getCog(BoxGeom* self)
{
  return self->cog.getValue();
}


void class_BoxGeom()
{
  class_<ProceduralSlot<mat3d, BoxGeom>, bases<Slot<mat3d> > >("BoxGeom_InertiatensorSlot", no_init);

  class_<BoxGeom, bases<GeomObject> >("BoxGeom", 
				      "The box geometry class.",
				      init<optional<double,double,double,int,int,int> >())
    .add_property("lx", getLx, setLx)
    .add_property("ly", getLy, setLy)
    .add_property("lz", getLz, setLz)
    .add_property("segmentsx", getSegmentsx, setSegmentsx)
    .add_property("segmentsy", getSegmentsy, setSegmentsy)
    .add_property("segmentsz", getSegmentsz, setSegmentsz)
    .add_property("cog", getCog)
    .add_property("inertiatensor", getInertiaTensor)
    .def_readonly("lx_slot", &BoxGeom::lx)
    .def_readonly("ly_slot", &BoxGeom::ly)
    .def_readonly("lz_slot", &BoxGeom::lz)
    .def_readonly("segmentsx_slot", &BoxGeom::segmentsx)
    .def_readonly("segmentsy_slot", &BoxGeom::segmentsy)
    .def_readonly("segmentsz_slot", &BoxGeom::segmentsz)
    .def_readonly("cog_slot", &BoxGeom::cog)
    .def_readonly("inertiatensor_slot", &BoxGeom::inertiatensor)
  ;

}
