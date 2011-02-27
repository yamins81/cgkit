/*
 Trimesh geom
 */

#include <boost/python.hpp>
#include "trimeshgeom.h"

using namespace boost::python;
using namespace support3d;

// get/set for "radius" property
/*double getRadius(SphereGeom* self)
{
  return self->radius.getValue();
}

void setRadius(SphereGeom* self, double r)
{
  self->radius.setValue(r);
}*/

tuple intersectRay(TriMeshGeom* self, const vec3d& origin, const vec3d& direction, bool earlyexit)
{
  IntersectInfo info;
  self->intersectRay(origin, direction, info, earlyexit);
  return make_tuple(info.hit, info.t, info.faceindex, info.u, info.v);
}

// get for "inertiatensor" property
mat3d getInertiaTensor(TriMeshGeom* self)
{
  return self->inertiatensor.getValue();
}

// get for "cog" property
vec3d getCog(TriMeshGeom* self)
{
  return self->cog.getValue();
}


void class_TriMeshGeom()
{
  class_<TriMeshGeom, bases<GeomObject> >("TriMeshGeom", 
    "A simple triangle mesh class.", init<>())

    // The xyz_slot is the same than xyz because they're arrays
    .def_readonly("verts_slot", &TriMeshGeom::verts)
    .def_readonly("verts", &TriMeshGeom::verts)
    .def_readonly("faces_slot", &TriMeshGeom::faces)
    .def_readonly("faces", &TriMeshGeom::faces)
    .add_property("cog", getCog)
    .add_property("inertiatensor", getInertiaTensor)
    .def_readonly("cog_slot", &TriMeshGeom::cog)
    .def_readonly("inertiatensor_slot", &TriMeshGeom::inertiatensor)

    .def("calcMassProperties", &TriMeshGeom::calcMassProperties)

    .def("intersectRay", intersectRay, (arg("origin"), arg("direction"), arg("earlyexit")=false),
	 "intersectRay(origin, direction, earlyexit=false) -> (hit, t, faceindex, u, v))\n\n"
	 "Intersect a ray with the mesh. This method tests a ray with all\n"
	 "triangles, so it's not efficient if you have a lot of rays to test.\n"
         "It's meant for only a few rays where the preprocessing cost wouldn't\n"
         "be amortized.\n\n"
         "The ray must be given in the local coordinate system L of the geometry.\n\n"
         "The ray-triangle intersection code (non-culling case) is based on:\n\n"
	 "Tomas Möller and Ben Trumbore.\n"
	 "Fast, minimum storage ray-triangle intersection.\n"
         "Journal of graphics tools, 2(1):21-28, 1997\n"
         "http://www.acm.org/jgt/papers/MollerTrumbore97/")
  ;

}
