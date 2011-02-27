/*
 Polyhedron geom
 */

#include <boost/python.hpp>
#include "polyhedrongeom.h"
#include "common_exceptions.h"

using namespace boost::python;
using namespace support3d;

/*
  getLoop() wrapper method which returns the loop as list
 */
list getLoop(PolyhedronGeom* self, int poly, int loop)
{
  list res;
  std::vector<int> vloop = self->getLoop(poly, loop);
  for(unsigned int i=0; i<vloop.size(); i++)
  {
    res.append(vloop[i]);
  }
  return res;
}

/*
  setLoop() wrapper method 
 */
void setLoop(PolyhedronGeom* self, int poly, int loop, object vloop)
{
  std::vector<int> vloop2;

  for(int i=0; i<vloop.attr("__len__")(); i++)
  {
    int v = extract<int>(vloop[i]);
    vloop2.push_back(v);
  }

  self->setLoop(poly, loop, vloop2);
}

/*
  Get a polygon.
 */
list getPoly(PolyhedronGeom* self, int poly)
{
  if ((poly<0) || (poly>=self->getNumPolys()))
      throw EIndexError("Poly index out of range.");

  list res;
  int loops = self->getNumLoops(poly);

  // Iterate over the loops...
  for(int i=0; i<loops; i++)
  {
    res.append(getLoop(self, poly, i));
  }
  return res;
}

/*
  Set a polygon.

  polydef must be a sequence of loops. A loop must be a sequence of
  vertex indices.
 */
void setPoly(PolyhedronGeom* self, int poly, object polydef)
{
  if ((poly<0) || (poly>=self->getNumPolys()))
      throw EIndexError("Poly index out of range.");

  int loops = extract<int>(polydef.attr("__len__")());

  self->setNumLoops(poly, loops);

  // Iterate over the loops...
  for(int i=0; i<loops; i++)
  {
    setLoop(self, poly, i, polydef[i]);
  }
}


//////////////////////////////////////////////////////////////////////
void class_PolyhedronGeom()
{
  class_<PolyhedronGeom, bases<GeomObject> >("PolyhedronGeom",
    "A simple polyhedron class.", init<>())

    // The xyz_slot is the same than xyz because they're arrays
    .def_readonly("verts_slot", &PolyhedronGeom::verts)
    .def_readonly("verts", &PolyhedronGeom::verts)
    //    .add_property("cog", getCog)
    //    .add_property("inertiatensor", getInertiaTensor)
    //    .def_readonly("cog_slot", &PolyhedronGeom::cog)
    //    .def_readonly("inertiatensor_slot", &PolyhedronGeom::inertiatensor)

    //    .def("calcMassProperties", &PolyhedronGeom::calcMassProperties)

    .def("hasPolysWithHoles", &PolyhedronGeom::hasPolysWithHoles, 
	 "hasPolysWithHoles() -> bool\n\n"
	 "Return True if there is at least one polygon with a hole.")

    .def("getNumPolys", &PolyhedronGeom::getNumPolys, 
	 "getNumPolys() -> int\n\n"
	 "Return the number of polygons.")

    .def("getNumLoops", &PolyhedronGeom::getNumLoops, (arg("poly")),
	 "getNumLoops(poly) -> int\n\n"
	 "Return the number of vertex loops in the given polygon. poly is the\n\n"
	 "index of the polygon.")

    .def("getNumVerts", &PolyhedronGeom::getNumVerts, (arg("poly"), arg("loop")),
	 "getNumVerts(poly, loop) -> int\n\n"
	 "Return the number of vertex indices in the given loop.")

    .def("setNumPolys", &PolyhedronGeom::setNumPolys, (arg("num")),
	 "setNumPolys(num)\n\n"
	 "Allocate space for num polygons.")

    .def("setNumLoops", &PolyhedronGeom::setNumLoops, (arg("poly"), arg("num")),
	 "setNumLoops(poly, num)\n\n"
	 "Allocate space for num loops in polygon with index poly.")

    .def("getLoop", getLoop, (arg("poly"), arg("loop")),
	 "getLoop(poly, loop) -> Loop\n\n"
	 "Return a loop from a poly.")

    .def("setLoop", setLoop, (arg("poly"), arg("loop"), arg("vloop")),
	 "setLoop(poly, loop, vloop)\n\n"
	 "Set new poly loop.")

    .def("getPoly", getPoly, (arg("poly")),
	 "getPoly(poly) -> polydef\n\n"
	 "Return a polygon.")

    .def("setPoly", setPoly, (arg("poly"), arg("polydef")),
	 "setPoly(poly, polydef)\n\n"
	 "Set a polygon.")
  ;

}
