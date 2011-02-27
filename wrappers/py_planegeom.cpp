/*
 Plane geom
 */

#include <boost/python.hpp>
#include "planegeom.h"

using namespace boost::python;
using namespace support3d;


// get/set for "lx" property
double getLx(PlaneGeom* self)
{
  return self->lx.getValue();
}

void setLx(PlaneGeom* self, double l)
{
  self->lx.setValue(l);
}

// get/set for "ly" property
double getLy(PlaneGeom* self)
{
  return self->ly.getValue();
}

void setLy(PlaneGeom* self, double l)
{
  self->ly.setValue(l);
}

// get/set for "segmentsx" property
int getSegmentsx(PlaneGeom* self)
{
  return self->segmentsx.getValue();
}

void setSegmentsx(PlaneGeom* self, int s)
{
  self->segmentsx.setValue(s);
}

// get/set for "segmentsy" property
int getSegmentsy(PlaneGeom* self)
{
  return self->segmentsy.getValue();
}

void setSegmentsy(PlaneGeom* self, int s)
{
  self->segmentsy.setValue(s);
}


void class_PlaneGeom()
{
  class_<PlaneGeom, bases<GeomObject> >("PlaneGeom", 
					"The plane geometry class.",
					init<optional<double,double,int,int> >())
    .add_property("lx", getLx, setLx)
    .add_property("ly", getLy, setLy)
    .add_property("segmentsx", getSegmentsx, setSegmentsx)
    .add_property("segmentsy", getSegmentsy, setSegmentsy)
    .def_readonly("lx_slot", &PlaneGeom::lx)
    .def_readonly("ly_slot", &PlaneGeom::ly)
    .def_readonly("segmentsx_slot", &PlaneGeom::segmentsx)
    .def_readonly("segmentsy_slot", &PlaneGeom::segmentsy)
  ;

}
