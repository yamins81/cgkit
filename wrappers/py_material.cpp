/*
 Material
 */

#include <boost/python.hpp>
#include "material.h"


using namespace boost::python;
using namespace support3d;

// Wrapper class for the Material class
class MaterialWrapper : public Material
{
  protected:
  PyObject* self;

  public:
  MaterialWrapper(PyObject* self_)
    : Material(), self(self_)
  {
  }

  MaterialWrapper(PyObject* self_, string aname, double adensity=1.0)
    : Material(aname, adensity), self(self_)
  {
  }

  //////////////////////////////////////////////////////////////////////
  // this method is called when applyGL() is called from C++ code
  void applyGL()
  {
    call_method<void>(self, "applyGL");
  }

  // this method is called when applyGL() is called from Python code
  void base_applyGL()
  {
    Material::applyGL();
  }

  //////////////////////////////////////////////////////////////////////
  // this method is called when usesBlending() is called from C++ code
  bool usesBlending()
  {
    return call_method<bool>(self, "usesBlending");
  }

  // this method is called when usesBlending() is called from Python code
  bool base_usesBlending()
  {
    return Material::usesBlending();
  }
};

//////////////////////////////////////////////////////////////////////

// get/set for "density" property
double getDensity(Material* self)
{
  return self->density.getValue();
}

void setDensity(Material* self, double d)
{
  self->density.setValue(d);
}


void class_Material()
{
  /*  class_<Material, bases<Component>, boost::noncopyable>("Material")
    .def(init<string, double>())
    .def_readonly("density_slot", &Material::density)
    .add_property("density", &getDensity, &setDensity)
    ;*/

  class_<Material, MaterialWrapper, bases<Component>, boost::noncopyable>("Material")
    .def(init<string, double>())
    .def_readonly("density_slot", &Material::density)
    .add_property("density", &getDensity, &setDensity)
    .def("applyGL", &MaterialWrapper::base_applyGL)
    .def("usesBlending", &MaterialWrapper::base_usesBlending)
  ;
}
