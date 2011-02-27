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

#ifndef GLRENDERER_H
#define GLRENDERER_H

/** \file glrenderer.h
 OpenGL renderer.
 */

#include "mat4.h"
#include "worldobject.h"
#include "glmaterial.h"

namespace support3d {

class GLRenderInstance
{
  public:
  int viewx;
  int viewy;
  int viewwidth;
  int viewheight;
  mat4d projectionmatrix;
  // View matrix for left eye (and non-stereo view)
  mat4d viewmatrix1;
  // View matrix for right eye
  mat4d viewmatrix2;

  // Background color
  vec4d clearcol;

  /// Shall the display be left handed?
  bool left_handed;

  bool draw_solid;

  /// Bounding boxes?
  bool draw_bboxes;

  bool draw_coordsys;

  // Small coordinate system in the lower left corner?
  bool draw_orientation;

  /// GL_FLAT or GL_SMOOTH shade model?
  bool smooth_model;

  /// Backface culling?
  bool backface_culling;

  /// Apply specular color after texturing?
  bool separate_specular_color;

  /// Point, light, fill mode
  int polygon_mode;

  // 0=No stereo / 1=VSplit
  int stereo_mode;

  /// Default material
  GLMaterial defaultmat;  

  public:
  GLRenderInstance();
  virtual ~GLRenderInstance() {}

  void setProjection(const mat4d& P);
  mat4d getProjection();
  void setViewTransformation(const mat4d& V, int eyeindex=0);
  mat4d getViewTransformation(int eyeindex=0);
  void setViewport(int x, int y, int width, int height);
  void getViewport(int& x, int& y, int& width, int& height) const;
  void paint(WorldObject& root);
  //void pick(int x, int y);   Rückgabe?

  protected:
  void drawScene(WorldObject& root, const mat4d& viewmat);
  bool drawNode(WorldObject& node, bool draw_blends);
  void applyLights(WorldObject& node);
  void drawWireCube(double lx, double ly, double lz);
  void drawCoordSystem();
};


}  // end of namespace

#endif
