/*
 GLMaterial
 */

#include <boost/python.hpp>
#include "glmaterial.h"


using namespace boost::python;
using namespace support3d;

// Wrapper class for the GLTexture class
class GLTextureWrapper : public GLTexture
{
  protected:
  PyObject* self;

  public:
  GLTextureWrapper(PyObject* self_)
    : GLTexture(), self(self_)
  {
  }

  // this method is called when loadTexData() is called from C++ code
  void loadTexData()
  {
    call_method<void>(self, "loadTexData");
  }
};


//////////////////////////////////////////////////////////////////////
// GLMaterial methods
//////////////////////////////////////////////////////////////////////

// get/set for "ambient" property
vec4d getAmbient(GLMaterial* self)
{
  return self->ambient.getValue();
}

void setAmbient(GLMaterial* self, vec4d c)
{
  self->ambient.setValue(c);
}

// get/set for "diffuse" property
vec4d getDiffuse(GLMaterial* self)
{
  return self->diffuse.getValue();
}

void setDiffuse(GLMaterial* self, vec4d c)
{
  self->diffuse.setValue(c);
}

// get/set for "specular" property
vec4d getSpecular(GLMaterial* self)
{
  return self->specular.getValue();
}

void setSpecular(GLMaterial* self, vec4d c)
{
  self->specular.setValue(c);
}

// get/set for "emission" property
vec4d getEmission(GLMaterial* self)
{
  return self->emission.getValue();
}

void setEmission(GLMaterial* self, vec4d c)
{
  self->emission.setValue(c);
}

// get/set for "shininess" property
double getShininess(GLMaterial* self)
{
  return self->shininess.getValue();
}

void setShininess(GLMaterial* self, double v)
{
  self->shininess.setValue(v);
}


void class_GLShader()
{
  scope scp = class_<GLShader, bases<Component> >("GLShader", init<GLShader::ShaderType, optional<string> >())
    //    .def_readwrite("type", &GLShader::type)
    .def_readwrite("filename", &GLShader::filename)
    .def("getType", &GLShader::getType)
  ;

  enum_<GLShader::ShaderType>("ShaderType")
    .value("VERTEX", GLShader::VERTEX)
    .value("FRAGMENT", GLShader::FRAGMENT)
  ;
}

void class_GLMaterial()
{
  class_<GLTexture, GLTextureWrapper, boost::noncopyable>("GLTexture")
    .add_property("imagename", &GLTexture::getImageName, &GLTexture::setImageName)
    .def_readwrite("environment_map", &GLTexture::environment_map)
    .def_readwrite("mipmap", &GLTexture::mipmap)
    .def_readwrite("mode", &GLTexture::mode)
    .def_readwrite("mag_filter", &GLTexture::mag_filter)
    .def_readwrite("min_filter", &GLTexture::min_filter)
    .def_readwrite("wrap_s", &GLTexture::wrap_s)
    .def_readwrite("wrap_t", &GLTexture::wrap_t)
    .def_readwrite("internalformat", &GLTexture::internalformat)
    .def_readwrite("texenvcolor", &GLTexture::texenvcolor)
    .def_readwrite("transform", &GLTexture::transform)
    .def("applyGL", &GLTextureWrapper::applyGL)
    .def("texData", &GLTextureWrapper::texData)
  ;

  class_GLShader();

  class_<GLMaterial, bases<Material> >("GLMaterial")
    .def(init<string, double>())
    .def_readonly("ambient_slot", &GLMaterial::ambient)
    .add_property("ambient", &getAmbient, &setAmbient)
    .def_readonly("diffuse_slot", &GLMaterial::diffuse)
    .add_property("diffuse", &getDiffuse, &setDiffuse)
    .def_readonly("specular_slot", &GLMaterial::specular)
    .add_property("specular", &getSpecular, &setSpecular)
    .def_readonly("emission_slot", &GLMaterial::emission)
    .add_property("emission", &getEmission, &setEmission)
    .def_readonly("shininess_slot", &GLMaterial::shininess)
    .add_property("shininess", &getShininess, &setShininess)
    //    .def_readwrite("texture", &GLMaterial::texture)
    .def_readwrite("blend_sfactor", &GLMaterial::blend_sfactor)
    .def_readwrite("blend_dfactor", &GLMaterial::blend_dfactor)
    .def("applyGL", &GLMaterial::applyGL)
    .def("usesBlending", &GLMaterial::usesBlending)

    .def("getNumTextures", &GLMaterial::getNumTextures,
	 "getNumTextures() -> int\n\n"
	 "Return the current size of the texture array.")

    .def("setNumTextures", &GLMaterial::setNumTextures, arg("num"),
	 "setNumTextures(num)\n\n"
	 "Set a new size for the texture array.")

    .def("getTexture", &GLMaterial::getTexture, (arg("idx")=0),
	 "getTexture(idx=0) -> GLTexture\n\n"
	 "Get a stored texture. The method returns None if the given index\n"
	 "is out of range or if there is no texture stored at that position.")

    .def("setTexture", &GLMaterial::setTexture, (arg("texture"), arg("idx")=0),
	 "setTexture(tex, idx=0)\n\n"
	 "Set a new texture. An IndexError exception is thrown if the index\n"
	 "is out of range.")

    .def("getNumVertexShaders", &GLMaterial::getNumVertexShaders,
	 "getNumVertexShaders() -> int\n\n"
	 "Return the current size of the vertex shader array.")

    .def("setNumVertexShaders", &GLMaterial::setNumVertexShaders, arg("num"),
	 "setNumVertexShaders(num)\n\n"
	 "Set a new size for the vertex shader array.")

    .def("getVertexShader", &GLMaterial::getVertexShader, (arg("idx")=0),
	 "getVertexShader(idx=0) -> GLShader\n\n"
	 "Get a vertex shader object. The method returns None if the given index\n"
	 "is out of range or if there is no shader object stored at that position."
    	 )

    .def("setVertexShader", &GLMaterial::setVertexShader, (arg("shader"), arg("idx")=0),
	 "setVertexShader(shader, idx=0)\n\n"
	 "Set a new vertex shader object. An IndexError exception is thrown if the\n"
	 "index is out of range. A ValueError exception is thrown if the shader is\n"
	 "not of type VERTEX.")

    .def("getNumFragmentShaders", &GLMaterial::getNumFragmentShaders,
	 "getNumFragmentShaders() -> int\n\n"
	 "Return the current size of the fragment shader array.")

    .def("setNumFragmentShaders", &GLMaterial::setNumFragmentShaders, arg("num"),
	 "setNumFragmentShaders(num)\n\n"
	 "Set a new size for the fragment shader array.")

    .def("getFragmentShader", &GLMaterial::getFragmentShader, //(arg("idx")=0),
	 "getFragmentShader(idx=0) -> GLShader\n\n"
	 "Get a fragment shader object. The method returns None if the given index\n"
	 "is out of range or if there is no shader object stored at that position.")

    .def("setFragmentShader", &GLMaterial::setFragmentShader, //(arg("shader"), arg("idx")=0),
	 "setFragmentShader(shader, idx=0)\n\n"
	 "Set a new fragment shader object. An IndexError exception is thrown if the\n"
	 "index is out of range. A ValueError exception is thrown if the shader is\n"
	 "not of type FRAGMENT.")
  ;

}
