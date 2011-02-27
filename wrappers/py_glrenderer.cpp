/*
  GLRenderer wrapper
 */

#include <boost/python.hpp>
#include "glrenderer.h"

using namespace boost::python;
using namespace support3d;

// getViewport wrapper
tuple getViewport(GLRenderInstance* self)
{
  int x,y,width,height;
  self->getViewport(x,y,width,height);
  return make_tuple(x,y,width,height);
}


void class_GLRenderInstance()
{
  class_<GLRenderInstance>("GLRenderInstance", init<>())

    .def_readwrite("left_handed", &GLRenderInstance::left_handed)
    .def_readwrite("draw_solid", &GLRenderInstance::draw_solid)
    .def_readwrite("draw_bboxes", &GLRenderInstance::draw_bboxes)
    .def_readwrite("draw_coordsys", &GLRenderInstance::draw_coordsys)
    .def_readwrite("draw_orientation", &GLRenderInstance::draw_orientation)
    .def_readwrite("smooth_model", &GLRenderInstance::smooth_model)
    .def_readwrite("backface_culling", &GLRenderInstance::backface_culling)
    .def_readwrite("separate_specular_color", &GLRenderInstance::separate_specular_color)
    .def_readwrite("polygon_mode", &GLRenderInstance::polygon_mode)
    .def_readwrite("stereo_mode", &GLRenderInstance::stereo_mode)
    .def_readwrite("clearcol", &GLRenderInstance::clearcol)

    .def("setProjection", &GLRenderInstance::setProjection, arg("P"),
	 "setProjection(P)\n\n"
	 "Set the projection matrix (given as mat4).")

    .def("getProjection", &GLRenderInstance::getProjection,
	 "getProjection() -> mat4\n\n"
	 "Return the current projection matrix.")

    .def("setViewTransformation", &GLRenderInstance::setViewTransformation, (arg("V"), arg("eyeindex")=0),
	 "setViewTransformation(V, eyeindex=0)\n\n"
	 "Set the view matrix (given as mat4). If eyeindex is 0 the left"
	 "eye is set, otherwise the right eye.")

    .def("getViewTransformation", &GLRenderInstance::getViewTransformation, (arg("eyeindex")=0),
	 "getViewTransformation(eyeindex=0) -> mat4\n\n"
	 "Return the current view transformation for the left or right eye.")

    .def("setViewport", &GLRenderInstance::setViewport, 
	 (arg("x"), arg("y"), arg("width"), arg("height")),
	 "setViewport(x, y, width, height)\n\n"
	 "Set the viewport window.")

    .def("getViewport", getViewport, 
	 "getViewport() -> (x, y, width, height)\n\n"
	 "Return the current viewport window.")

    .def("paint", &GLRenderInstance::paint, arg("root"),
	 "paint(root)\n\n"
	 "Paint the scene starting at root.")
  ;
}
