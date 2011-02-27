/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is the Python Computer Graphics Kit.
 *
 * The Initial Developer of the Original Code is Matthias Baas.
 * Portions created by the Initial Developer are Copyright (C) 2004
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */

#include "glmaterial.h"

#include "opengl.h"

namespace support3d {

//////////////////////////////////////////////////////////////
// Texture
//////////////////////////////////////////////////////////////

/**
  Constructor.
 */
GLTexture::GLTexture()
: environment_map(false),
  mipmap(true),
  mode(GL_DECAL),
  mag_filter(GL_LINEAR),
  min_filter(GL_LINEAR),
  wrap_s(GL_REPEAT),
  wrap_t(GL_REPEAT),
  internalformat(GL_RGB),
  texenvcolor(1),
  transform(1),
  imagename(),
  texname(0),
  flags(0)
{
}

/**
  Destructor.
 */
GLTexture::~GLTexture()
{
}

/**
  Return the name of the texture image.

  \return Image name
 */
string GLTexture::getImageName()
{
  return imagename;
}

/**
  Set a new texture image name.

  The texture will be reloaded in the next call to applyGL().

  \param name New texture image name
 */
void GLTexture::setImageName(string name)
{
  imagename = name;
  flags |= IMAGE_NAME;
}

/**
  Do the OpenGL calls to activate the texture.
 */
void GLTexture::applyGL()
{
  if (texname==0)
  {
    allocGL();
  }

  glBindTexture(GL_TEXTURE_2D, texname);

  // Has the image name changed? Then reload
  if (flags & IMAGE_NAME)
  {
    flags &= ~IMAGE_NAME;
    loadTexData();
  }

  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, mode);
  GLfloat col[4] = {GLfloat(texenvcolor.x), GLfloat(texenvcolor.y),
                    GLfloat(texenvcolor.z), GLfloat(texenvcolor.w)};
  glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, col);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_s);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_t);

  glMatrixMode(GL_TEXTURE);
  double M[16];
  glLoadIdentity();
  transform.toList(M);
  glMultMatrixd(M);
  glMatrixMode(GL_MODELVIEW);

  if (environment_map)
  {
    // Enable environment mapping
    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
    glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_GEN_T);
  }
  else
  {
    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);
  }
}

/**
  Allocate the OpenGL texture object.
 */
void GLTexture::allocGL()
{
  GLuint tn[1];

  // Alloc texture name and create texture object...
  glGenTextures(1, tn);
  texname = tn[0];
  glBindTexture(GL_TEXTURE_2D, texname);

  std::cout<<"Texname: "<<texname<<std::endl;
}

void GLTexture::releaseGL()
{
}

/**
  Pass the texture data to OpenGL.

  This method has to be called by the texData() method.

  \param w Width of the texture image (must be a power of 2)
  \param h Height of the texture image (must be a power of 2)
  \param format Format of the data (see format parameter of the glTexImage2D() function)
  \param type Type of the data (see type parameter of the glTexImage2D() function)
  \param data Image data
 */
void GLTexture::texData(int w, int h, int format, int type, char* data)
{
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  if (mipmap)
    gluBuild2DMipmaps(GL_TEXTURE_2D, internalformat, w, h, format, type, data);
  else
    glTexImage2D(GL_TEXTURE_2D, 0, internalformat, w, h, 0, format, type, data);
}


//////////////////////////////////////////////////////////////
// Material
//////////////////////////////////////////////////////////////

/**
  Constructor.
 */
GLMaterial::GLMaterial()
 : Material(),
   ambient(vec4d(0.2,0.2,0.2,1), 0),
   diffuse(vec4d(0.7,0.7,0.7,1), 0),
   specular(vec4d(0,0,0,1), 0),
   shininess(0.0, 0),
   emission(vec4d(0,0,0,1), 0),
   blend_sfactor(-1),
   blend_dfactor(-1),
   textures(),
   vertex_shader(),
   fragment_shader()
{
  addSlot("ambient", ambient);
  addSlot("diffuse", diffuse);
  addSlot("specular", specular);
  addSlot("shininess", shininess);
  addSlot("emission", emission);
}

/**
  Constructor.
 */
GLMaterial::GLMaterial(string aname, double adensity)
 : Material(aname, adensity),
   ambient(vec4d(0.2,0.2,0.2,1), 0),
   diffuse(vec4d(0.7,0.7,0.7,1), 0),
   specular(vec4d(0,0,0,1), 0),
   shininess(0.0, 0),
   emission(vec4d(0,0,0,1), 0),
   blend_sfactor(-1),
   blend_dfactor(-1),
   textures(),
   vertex_shader(),
   fragment_shader()
{
  addSlot("ambient", ambient);
  addSlot("diffuse", diffuse);
  addSlot("specular", specular);
  addSlot("shininess", shininess);
  addSlot("emission", emission);
}

/** Destructor. */
GLMaterial::~GLMaterial()
{
}

/**
  Apply the material using OpenGL commands.

  The material settings are only applied to the front faces.

  \pre The appropriate OpenGL context has been made current
 */
void GLMaterial::applyGL()
{
  GLfloat c[4] = {0,0,0,1};
  GLfloat f;

  const vec4d& ac = ambient.getValue();
  c[0] = GLfloat(ac.x);
  c[1] = GLfloat(ac.y);
  c[2] = GLfloat(ac.z);
  c[3] = GLfloat(ac.w);
  glMaterialfv(GL_FRONT, GL_AMBIENT, c);

  const vec4d& dc = diffuse.getValue();
  c[0] = GLfloat(dc.x);
  c[1] = GLfloat(dc.y);
  c[2] = GLfloat(dc.z);
  c[3] = GLfloat(dc.w);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, c);

  const vec4d& sc = specular.getValue();
  c[0] = GLfloat(sc.x);
  c[1] = GLfloat(sc.y);
  c[2] = GLfloat(sc.z);
  c[3] = GLfloat(sc.w);
  glMaterialfv(GL_FRONT, GL_SPECULAR, c);

  const vec4d& ec = emission.getValue();
  c[0] = GLfloat(ec.x);
  c[1] = GLfloat(ec.y);
  c[2] = GLfloat(ec.z);
  c[3] = GLfloat(ec.w);
  glMaterialfv(GL_FRONT, GL_EMISSION, c);

  f = GLfloat(shininess.getValue());
  glMaterialf(GL_FRONT, GL_SHININESS, f);

  boost::shared_ptr<GLTexture> texture = getTexture(0);
  if (texture.get()!=0)
  {
    texture->applyGL();
    glEnable(GL_TEXTURE_2D);
  }
  else
  {
    glDisable(GL_TEXTURE_2D);
  }

  // Blending?
  if (usesBlending())
  {
    glEnable(GL_BLEND);
    glBlendFunc(blend_sfactor, blend_dfactor);
  }
  else
  {
    glDisable(GL_BLEND);
  }
}

/**
  Return True if the material activates blending.
 */
bool GLMaterial::usesBlending()
{
  return (blend_sfactor!=-1) && (blend_dfactor!=-1);
}

/**
  Return the current size of the texture array.

  \return Number of textures
 */
int GLMaterial::getNumTextures() const
{
  return textures.size();
}

/**
  Set a new size for the texture array.

  \param num New number of textures
 */
void GLMaterial::setNumTextures(int num)
{
  if (num<1)
    num=1;
  textures.resize(num);
}


/**
  Get a stored texture.

  The method returns an empty pointer if the given index \a idx is out
  of range or if there is no texture stored at that position.

  \param idx The index of the texture (can be negative to count from the end of the list)
  \return The texture object
 */
boost::shared_ptr<GLTexture> GLMaterial::getTexture(int idx) const
{
  if (idx<0)
    idx = textures.size()+idx;
  if ((idx<0) || (idx>=int(textures.size())))
    return boost::shared_ptr<GLTexture>();

  return textures[idx];
}

/**
  Set a new texture.

  An EIndexError exception is thrown if the index is out of range.

  \param atexture The new texture object
  \param idx The index of the texture (can be negative to count from the end of the list)
 */
void GLMaterial::setTexture(boost::shared_ptr<GLTexture> atexture, int idx)
{
  if (idx<0)
    idx = textures.size()+idx;
  if ((idx<0) || (idx>=int(textures.size())))
    throw EIndexError();

  textures[idx] = atexture;
}

/**
  Return the current size of the vertex shader array.

  \return Number of vertex shader objects
 */
int GLMaterial::getNumVertexShaders() const
{
  return vertex_shader.size();
}

/**
  Set a new size for the vertex shader array.

  \param num New number of vertex shader objects
 */
void GLMaterial::setNumVertexShaders(int num)
{
  if (num<1)
    num=1;
  vertex_shader.resize(num);
}


/**
  Get a stored vertex shader object.

  The method returns an empty pointer if the given index \a idx is out
  of range or if there is no shader object stored at that position.

  \param idx The index of the shader object (can be negative to count from the end of the list)
  \return The shader object
 */
boost::shared_ptr<GLShader> GLMaterial::getVertexShader(int idx) const
{
  if (idx<0)
    idx = vertex_shader.size()+idx;
  if ((idx<0) || (idx>=int(vertex_shader.size())))
    return boost::shared_ptr<GLShader>();

  return vertex_shader[idx];
}

/**
  Set a new vertex shader object.

  An EValueError exception is thrown if \a ashader is not of type VERTEX.
  An EIndexError exception is thrown if the index is out of range.

  \param ashader The new shader object
  \param idx The index of the shader object (can be negative to count from the end of the list)
 */
void GLMaterial::setVertexShader(boost::shared_ptr<GLShader> ashader, int idx)
{
  // Check if the shader is a VERTEX shader...
  if (ashader.get()!=0)
  {
    if (ashader->getType()!=GLShader::VERTEX)
      throw EValueError("Shader must be of type VERTEX");
  }

  if (idx<0)
    idx = vertex_shader.size()+idx;
  if ((idx<0) || (idx>=int(vertex_shader.size())))
    throw EIndexError();

  vertex_shader[idx] = ashader;
}

/**
  Return the current size of the fragment shader array.

  \return Number of fragment shader objects
 */
int GLMaterial::getNumFragmentShaders() const
{
  return fragment_shader.size();
}

/**
  Set a new size for the fragment shader array.

  \param num New number of fragment shader objects
 */
void GLMaterial::setNumFragmentShaders(int num)
{
  if (num<1)
    num=1;
  fragment_shader.resize(num);
}


/**
  Get a stored fragment shader object.

  The method returns an empty pointer if the given index \a idx is out
  of range or if there is no shader object stored at that position.

  \param idx The index of the shader object (can be negative to count from the end of the list)
  \return The shader object
 */
boost::shared_ptr<GLShader> GLMaterial::getFragmentShader(int idx) const
{
  if (idx<0)
    idx = fragment_shader.size()+idx;
  if ((idx<0) || (idx>=int(fragment_shader.size())))
    return boost::shared_ptr<GLShader>();

  return fragment_shader[idx];
}

/**
  Set a new fragment shader object.

  An EValueError exception is thrown if \a ashader is not of type FRAGMENT.
  An EIndexError exception is thrown if the index is out of range.

  \param ashader The new shader object
  \param idx The index of the shader object (can be negative to count from the end of the list)
 */
void GLMaterial::setFragmentShader(boost::shared_ptr<GLShader> ashader, int idx)
{
  // Check if the shader is a FRAGMENT shader...
  if (ashader.get()!=0)
  {
    if (ashader->getType()!=GLShader::FRAGMENT)
      throw EValueError("Shader must be of type FRAGMENT");
  }

  if (idx<0)
    idx = fragment_shader.size()+idx;
  if ((idx<0) || (idx>=int(fragment_shader.size())))
    throw EIndexError();

  fragment_shader[idx] = ashader;
}


}  // end of namespace
