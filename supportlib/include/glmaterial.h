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

#ifndef GLMATERIAL_H
#define GLMATERIAL_H

/** \file glmaterial.h
 Contains the GLMaterial class.
 */

#include <vector>
#include <string>
#include "component.h"
#include "material.h"
#include "vec3.h"
#include "vec4.h"
#include "mat4.h"
#include "slot.h"
#include <boost/shared_ptr.hpp>


namespace support3d {

/**
  OpenGL texture object.

  This class represents an OpenGL texture and its parameters. 
  A %GLTexture object is used in combination with a GLMaterial 
  object where it has to be assigned to the \a texture attribute.

  This class cannot be used directly as it doesn't include a method
  for retrieving the actual texture data. This has to be done in
  a derived class in the loadTexData() method. This method either
  loads an image from disk or creates an image procedurally.

  \see GLMaterial
 */
class GLTexture
{
  public:
  /// Environment map flag
  bool environment_map;

  /// Mip map flag
  bool mipmap;

  /// Texture mode (GL_MODULATE, GL_DECAL, GL_BLEND, GL_REPLACE)
  int mode;

  /// Magnification filter (GL_NEAREST, GL_LINEAR)
  int mag_filter;

  /// Minification filter (GL_NEAREST, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR, ...)
  int min_filter;

  // Texture wrap in s direction (GL_CLAMP, GL_CLAMP_TO_EDGE, GL_REPEAT)
  int wrap_s;

  // Texture wrap in t direction (GL_CLAMP, GL_CLAMP_TO_EDGE, GL_REPEAT)
  int wrap_t;

  /// Internal texture format (GL_RGB, GL_RGBA, ...)
  int internalformat;

  /// Color for the texture blending
  vec4d texenvcolor;

  /// Texture coordinate transformation matrix
  mat4d transform;

  protected:
  /// Image file name
  string imagename;

  /// OpenGL texture name
  int texname;

  /// Update flags
  int flags;

  // Flags
  enum { IMAGE_NAME = 0x01 }; 

  public:
  GLTexture();
  virtual ~GLTexture();

  string getImageName();
  void setImageName(string name);

  void applyGL();

  // Initialization (alloc texture object)
  void allocGL();
  void releaseGL();
  void texData(int w, int h, int format, int type, char* data);

  /**
    Provide the texture data.

    This method has to be implemented in a derived class.
    The method must pass the texture data by calling texData().
   */
  virtual void loadTexData() {}
};

/**
  An OpenGL shader class.

  This class stores the file name of a shader and its uniform parameters
  which are stored in the slots.

  \see GLMaterial
 */
class GLShader : public Component
{
  public:
  enum ShaderType { VERTEX, FRAGMENT };

  /// Shader source file
  std::string filename;

  private:

  /// Shader type.
  ShaderType type;

  public:
  GLShader(ShaderType atype, std::string afilename=std::string()) : filename(afilename), type(atype) {}

  ShaderType getType() const { return type; }
};

/**
  An OpenGL material class.

  This material has the same parameters that the OpenGL material model
  has.

  \see GLTexture, GLShader
 */
class GLMaterial : public Material
{
  public:
  /// Ambient color 
  Slot<vec4d> ambient;
  /// Diffuse color 
  Slot<vec4d> diffuse;
  /// Specular color 
  Slot<vec4d> specular;
  /// Shininess color 
  Slot<double> shininess;
  /// Emission color 
  Slot<vec4d> emission;

  int blend_sfactor;
  int blend_dfactor;

  protected:
  /** A list of GLTexture objects.
   */
  std::vector<boost::shared_ptr<GLTexture> > textures;
  //  boost::shared_ptr<GLTexture> texture;

  /** A list of GLShader objects that make up a vertex shader.
      If this list is empty, no vertex shader is used. The shaders in 
      this list may only be of type VERTEX. An item may also contain an
      empty pointer in which case it should be ignored.
   */
  std::vector<boost::shared_ptr<GLShader> > vertex_shader;
  
  /** A list of GLShader objects that make up a fragment shader.
      If this list is empty, no fragment shader is used. The shaders in
      this list may only be of type FRAGMENT. An item may also contain an
      empty pointer in which case it should be ignored.
  */
  std::vector<boost::shared_ptr<GLShader> > fragment_shader;

  public:
  GLMaterial();
  GLMaterial(string aname, double adensity=1.0);
  virtual ~GLMaterial();
  
  virtual void applyGL();
  bool usesBlending();

  int getNumTextures() const;
  void setNumTextures(int num);
  boost::shared_ptr<GLTexture> getTexture(int idx=0) const;
  void setTexture(boost::shared_ptr<GLTexture> atexture, int idx=0);

  int getNumVertexShaders() const;
  void setNumVertexShaders(int num);
  boost::shared_ptr<GLShader> getVertexShader(int idx=0) const;
  void setVertexShader(boost::shared_ptr<GLShader> ashader, int idx=0);

  int getNumFragmentShaders() const;
  void setNumFragmentShaders(int num);
  boost::shared_ptr<GLShader> getFragmentShader(int idx=0) const;
  void setFragmentShader(boost::shared_ptr<GLShader> ashader, int idx=0);
  
};

}  // end of namespace

#endif
