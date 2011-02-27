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

#include "gldistantlight.h"

#include "vec3.h"
#include "opengl.h"

namespace support3d {

GLDistantLight::GLDistantLight()
: LightSource(),
  ambient(vec3d(0,0,0), 0),
  diffuse(vec3d(1,1,1), 0),
  specular(vec3d(1,1,1), 0)
{
  addSlot("ambient", ambient);
  addSlot("diffuse", diffuse);
  addSlot("specular", specular);
}

GLDistantLight::GLDistantLight(string aname)
: LightSource(aname),
  ambient(vec3d(0,0,0), 0),
  diffuse(vec3d(1,1,1), 0),
  specular(vec3d(1,1,1), 0)
{
  addSlot("ambient", ambient);
  addSlot("diffuse", diffuse);
  addSlot("specular", specular);
}

/**
  Apply the light using OpenGL commands.

  This method does not enable the light source (you have to call glEnable(GL_LIGHTn)
  yourself).

  \pre The transformation of the light was already applied
 */
void GLDistantLight::applyGL(int idx)
{
  GLfloat c[4] = {0,0,0,1};
  int glidx = GL_LIGHT0 + idx;
  double I = intensity.getValue();

  glLightf(glidx, GL_SPOT_CUTOFF, 180.0);

  c[2] = -1;
  c[3] = 0;
  glLightfv(glidx, GL_POSITION, c);

  const vec3d& ac = ambient.getValue();
  c[0] = GLfloat(I*ac.x);
  c[1] = GLfloat(I*ac.y);
  c[2] = GLfloat(I*ac.z);
  glLightfv(glidx, GL_AMBIENT, c);

  const vec3d& dc = diffuse.getValue();
  c[0] = GLfloat(I*dc.x);
  c[1] = GLfloat(I*dc.y);
  c[2] = GLfloat(I*dc.z);
  glLightfv(glidx, GL_DIFFUSE, c);

  const vec3d& sc = specular.getValue();
  c[0] = GLfloat(sc.x);
  c[1] = GLfloat(sc.y);
  c[2] = GLfloat(sc.z);
  glLightfv(glidx, GL_SPECULAR, c);

  glLightf(glidx, GL_CONSTANT_ATTENUATION, 1.0);
  glLightf(glidx, GL_LINEAR_ATTENUATION, 0.0);
  glLightf(glidx, GL_QUADRATIC_ATTENUATION, 0.0);
}


}  // end of namespace
