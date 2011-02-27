/*
 "Draw" geom
 */

#include <boost/python.hpp>
#include "drawgeom.h"

using namespace boost::python;
using namespace support3d;


void class_DrawGeom()
{
  class_<DrawGeom, bases<GeomObject> >("DrawGeom",
				      "The \"Draw\" geometry class.",
				      init<>())
    .def("clear", &DrawGeom::clear,
	 "clear()\n\n"
	 "Clear all stored objects.")

    .def("marker", &DrawGeom::marker, 
	 (arg("pos"), arg("col")=vec3d(1,1,1), arg("size")=1.0),
	 "marker(pos, col=vec3(1,1,1), size=1.0)\n\n"
	 "Add a marker object (a point).")

    .def("line", &DrawGeom::line, 
	 (arg("pos1"), arg("pos2"), arg("col")=vec3d(1,1,1), arg("size")=1.0),
	 "line(pos1, pos2, col=vec3(1,1,1), size=1.0)\n\n"
	 "Add a line object.")

  ;

}
