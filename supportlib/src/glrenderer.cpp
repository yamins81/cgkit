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

#include "glrenderer.h"
#include "lightsource.h"
#include "glpointlight.h"
#include "glspotlight.h"
#include "gldistantlight.h"

#include "opengl.h"

// Define those two OpenGL 1.2 constants (that don't come with MSVC6)
#ifndef GL_LIGHT_MODEL_COLOR_CONTROL
#define GL_LIGHT_MODEL_COLOR_CONTROL      0x81F8
#define GL_SINGLE_COLOR                   0x81F9
#define GL_SEPARATE_SPECULAR_COLOR        0x81FA
#endif

namespace support3d {

GLRenderInstance::GLRenderInstance()
: viewx(0), viewy(0), viewwidth(0), viewheight(0),
  projectionmatrix(1), viewmatrix1(1), viewmatrix2(2),
  clearcol(0.5,0.5,0.6,0),
  left_handed(false), draw_solid(true), draw_bboxes(false),
  draw_coordsys(true), draw_orientation(true),
  smooth_model(true), backface_culling(false),
  separate_specular_color(false), polygon_mode(2),
  stereo_mode(0), defaultmat()
{
}

/**
  Set the projection matrix.

  The projection usually comes from the camera object and should be
  set before calling the paint() method.

  \param P Projection matrix
 */
void GLRenderInstance::setProjection(const mat4d& P)
{
  projectionmatrix = P;
}

/**
  Return the current projection matrix.

  \return Projection matrix
 */
mat4d GLRenderInstance::getProjection()
{
  return projectionmatrix;
}

/**
  Set the view transformation.

  For stereo rendering you have to set the view transformation for
  the left and the right eye.

  The view transformation usually comes from the camera object and
  should be set before calling the paint() method.

  \param V View transformation
  \param eyeindex 0=Left eye (or non-stereo view) / 1=Right eye
 */
void GLRenderInstance::setViewTransformation(const mat4d& V, int eyeindex)
{
  if (eyeindex==0)
    viewmatrix1 = V;
  else
    viewmatrix2 = V;
}

/**
  Return the current view transformation for the left or right eye.

  \param eyeindex 0=Left eye (or non-stereo view) / 1=Right eye
  \return View transformation
 */
mat4d GLRenderInstance::getViewTransformation(int eyeindex)
{
  if (eyeindex==0)
    return viewmatrix1;
  else
    return viewmatrix2;
}

/**
  Set the viewport area.
 */
void GLRenderInstance::setViewport(int x, int y, int width, int height)
{
  viewx = x;
  viewy = y;
  viewwidth = width;
  viewheight = height;
}

/**
  Get the viewport area.
 */
void GLRenderInstance::getViewport(int& x, int& y, int& width, int& height) const
{
  x = viewx;
  y = viewy;
  width = viewwidth;
  height = viewheight;
}

/**
  Draw the current scene.

  \todo Lichtquellen am Anfang disablen
 */
void GLRenderInstance::paint(WorldObject& root)
{
  double M[16];

  if (stereo_mode==2)
  {
    // Switch to both back buffers for initialization...
    glDrawBuffer(GL_BACK);
  }

  // Viewport
  glViewport(viewx, viewy, viewwidth, viewheight);

  // Initialize
  glClearColor(GLfloat(clearcol.x), GLfloat(clearcol.y), GLfloat(clearcol.z), GLfloat(clearcol.w));
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);
//  glDisable(GL_LIGHTING);
  glEnable(GL_LIGHTING);
  glEnable(GL_NORMALIZE);
  if (smooth_model)
    glShadeModel(GL_SMOOTH);
  else
    glShadeModel(GL_FLAT);

  // Enable backface culling
  if (backface_culling)
  {
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
  }
  else
    glDisable(GL_CULL_FACE);

//  glEnable(GL_COLOR_MATERIAL);
  glColorMaterial(GL_FRONT, GL_DIFFUSE);

  if (separate_specular_color)
    glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR);
  else
    glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SINGLE_COLOR);

  glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
  glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
//  glEnable(GL_LINE_SMOOTH);
//  glEnable(GL_BLEND);
//  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

//  glDisable(GL_LIGHTING)
  switch(polygon_mode)
  {
   case 0: glPolygonMode(GL_FRONT_AND_BACK, GL_POINT); break;
   case 1: glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); break;
   case 2: glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); break;
  }

  // Projection
  glMatrixMode(GL_PROJECTION);
  projectionmatrix.toList(M, false);
  glLoadMatrixd(M);

  // Initialize ModelView matrix
  glMatrixMode(GL_MODELVIEW);

  switch(stereo_mode)
  {
   case 1: glViewport(viewx, viewy, viewwidth/2, viewheight); break;
   case 2: glDrawBuffer(GL_BACK_LEFT); break;
  }

  // Draw the scene
  drawScene(root, viewmatrix1);

  switch(stereo_mode)
  {
   case 1:
     // Draw right image
     glViewport(viewx+viewwidth/2, viewy, viewwidth/2, viewheight);
     drawScene(root, viewmatrix2);
     break;
   case 2:
     glDrawBuffer(GL_BACK_RIGHT);
     // Draw right image
     glClear(GL_DEPTH_BUFFER_BIT);
     drawScene(root, viewmatrix2);
     glDrawBuffer(GL_BACK);
     break;
  }

  // Draw the orientation as a small coordinate system in the lower left corner
  if (draw_orientation)
  {
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glViewport(0, 0, 48, 48);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1,1,-1,1,-1,1);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    if (left_handed)
      glScaled(-1,1,1);

    glScalef(GLfloat(0.9), GLfloat(0.9), GLfloat(0.9));
    glRotated(180,0,1,0);
    mat4d V2(viewmatrix1);
    V2.setColumn(3, 0,0,0,1);
    V2.toList(M, false);
    glMultMatrixd(M);

    drawCoordSystem();

    if (stereo_mode==1)
    {
      glViewport(viewwidth/2, 0, 48, 48);
      drawCoordSystem();
    }
  }

  // HUD-Test
/*  glViewport(viewx, viewy, viewwidth, viewheight);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_LIGHTING);
  glDisable(GL_NORMALIZE);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0,1, 0,1, -1, 1);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glColor3d(0,1,0);
  glBegin(GL_LINES);
  glVertex2d(0,0);
  glVertex2d(1,1);
  glEnd();*/
}

void GLRenderInstance::drawScene(WorldObject& root, const mat4d& viewmat)
{
  double M[16];

  glLoadIdentity();
  if (left_handed)
      glScaled(-1,1,1);

  // Default light source
  // (this is overwritten if there's at least one light in the scene)
  GLfloat pos[4] = {0,0,1,0};
  GLfloat diffuse[4] = {1,1,1,1};
  GLfloat specular[4] = {1,1,1,1};
  glLightfv(GL_LIGHT0, GL_POSITION, pos);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
  glEnable(GL_LIGHT0);

  // View transformation
  glRotated(180,0,1,0);
  viewmat.toList(M, false);
  glMultMatrixd(M);

  // Apply the light sources
  applyLights(root);

  // Draw the scene
  if (draw_coordsys)
    drawCoordSystem();
  defaultmat.applyGL();
  if (drawNode(root, false))
  {
    // There have been objects that use blending, so draw them now...
    glDepthMask(GL_FALSE);
    drawNode(root, true);
    glDepthMask(GL_TRUE);
  }
}

/**
  Draw a tree.

  \param node Node
  \param draw_blends If true, only objects that use blending are drawn. Otherwise objects with
         blending are not drawn.
  \return True if the tree contained one or more objects that use blending
 */
bool GLRenderInstance::drawNode(WorldObject& node, bool draw_blends)
{
  double M[16];
  BoundingBox bb;
  vec3d bmin, bmax, t, d;
  bool render_flag = true;
  bool res = false;

  WorldObject::ChildIterator it;
  for(it=node.childsBegin(); it!=node.childsEnd(); it++)
  {
    glPushMatrix();
    // Set the local transformation
    it->second->localTransform().toList(M);
    glMultMatrixd(M);
    // Draw the geom (if visible)
    boost::shared_ptr<GeomObject> geom = it->second->getGeom();
    if (geom.get()!=0 && (it->second->visible.getValue()))
    {
      // Render by default if draw_blends is false
      render_flag = !draw_blends;

      // Set material
      boost::shared_ptr<Material> mat = it->second->getMaterial();
      Material* bmat = dynamic_cast<Material*>(mat.get());
      // Check if the object should be postponed because blending is used...
      if (bmat!=0)
      {
        if (bmat->usesBlending())
        {
          res = true;
          render_flag = draw_blends;
        }
      }

      if (render_flag)
      {
        // Set material
        glPushAttrib(GL_LIGHTING_BIT | GL_TEXTURE_BIT);
        if (bmat!=0)
        {
          bmat->applyGL();
        }

        if (draw_solid)
        {
          geom->drawGL();
        }
        // Draw bounding box
        if (draw_bboxes)
        {
          bb = geom->boundingBox();
          if (!bb.isEmpty())
          {
            glDisable(GL_LIGHTING);
            bb.getBounds(bmin, bmax);
            t = 0.5*(bmax+bmin);
            d = bmax-bmin;
            glPushMatrix();
            glTranslated(t.x, t.y, t.z);
            drawWireCube(d.x, d.y, d.z);
            glPopMatrix();
            glEnable(GL_LIGHTING);
          }
        }
        glPopAttrib();  // restore material
      }
    }
    // Draw the children
    res |= drawNode(*(it->second), draw_blends);
    glPopMatrix();
  }
  return res;
}

/**
  Walk recursively through a node and switch on all lights.

  \param node World object to search for lights
 */
void GLRenderInstance::applyLights(WorldObject& node)
{
  double M[16];
  int idx=0;

  WorldObject::ChildIterator it;
  for(it=node.childsBegin(); it!=node.childsEnd(); it++)
  {
    glPushMatrix();
    // Set the local transformation
    it->second->localTransform().toList(M);
    glMultMatrixd(M);

    // Check if the object is a light source...
    LightSource* lgt = dynamic_cast<LightSource*>(it->second.get());
    if (lgt!=0)
    {
      // Is it a point light?
      GLPointLight* pntlgt = dynamic_cast<GLPointLight*>(lgt);
      if (pntlgt!=0)
      {
        if (pntlgt->enabled.getValue())
        {
          pntlgt->applyGL(idx);
          glEnable(GL_LIGHT0+idx);
          idx++;
        }
      }
      // Is it a spot light?
      GLSpotLight* spotlgt = dynamic_cast<GLSpotLight*>(lgt);
      if (spotlgt!=0)
      {
        if (spotlgt->enabled.getValue())
        {
          spotlgt->applyGL(idx);
          glEnable(GL_LIGHT0+idx);
          idx++;
        }
      }
      // Is it a distant light?
      GLDistantLight* dstlgt = dynamic_cast<GLDistantLight*>(lgt);
      if (dstlgt!=0)
      {
        if (dstlgt->enabled.getValue())
        {
          dstlgt->applyGL(idx);
          glEnable(GL_LIGHT0+idx);
          idx++;
        }
      }
    }

    // Search the children for lights
    applyLights(*(it->second));
    glPopMatrix();
  }
}

void GLRenderInstance::drawWireCube(double lx, double ly, double lz)
{
  lx /= 2.0;
  ly /= 2.0;
  lz /= 2.0;
  glBegin(GL_LINE_STRIP);
  glVertex3f(GLfloat(-lx), GLfloat(-ly), GLfloat(-lz));
  glVertex3f(GLfloat( lx), GLfloat(-ly), GLfloat(-lz));
  glVertex3f(GLfloat( lx), GLfloat( ly), GLfloat(-lz));
  glVertex3f(GLfloat(-lx), GLfloat( ly), GLfloat(-lz));
  glVertex3f(GLfloat(-lx), GLfloat(-ly), GLfloat(-lz));
  glEnd();
  glBegin(GL_LINE_STRIP);
  glVertex3f(GLfloat(-lx), GLfloat(-ly), GLfloat(lz));
  glVertex3f(GLfloat( lx), GLfloat(-ly), GLfloat(lz));
  glVertex3f(GLfloat( lx), GLfloat( ly), GLfloat(lz));
  glVertex3f(GLfloat(-lx), GLfloat( ly), GLfloat(lz));
  glVertex3f(GLfloat(-lx), GLfloat(-ly), GLfloat(lz));
  glEnd();
  glBegin(GL_LINES);
  glVertex3f(GLfloat(-lx), GLfloat(-ly), GLfloat(-lz));
  glVertex3f(GLfloat(-lx), GLfloat(-ly), GLfloat( lz));
  glVertex3f(GLfloat( lx), GLfloat(-ly), GLfloat(-lz));
  glVertex3f(GLfloat( lx), GLfloat(-ly), GLfloat( lz));
  glVertex3f(GLfloat( lx), GLfloat( ly), GLfloat(-lz));
  glVertex3f(GLfloat( lx), GLfloat( ly), GLfloat( lz));
  glVertex3f(GLfloat(-lx), GLfloat( ly), GLfloat(-lz));
  glVertex3f(GLfloat(-lx), GLfloat( ly), GLfloat( lz));
  glEnd();
}

void GLRenderInstance::drawCoordSystem()
{
  glPushAttrib(GL_ALL_ATTRIB_BITS);
  glDisable(GL_LIGHTING);
  glDisable(GL_DEPTH_TEST);
  glBegin(GL_LINES);
  glColor3f(1,0,0);
  glVertex3f(0,0,0);
  glVertex3f(1,0,0);
  glColor3f(0,1,0);
  glVertex3f(0,0,0);
  glVertex3f(0,1,0);
  glColor3f(0,0,1);
  glVertex3f(0,0,0);
  glVertex3f(0,0,1);
  glEnd();
  glPopAttrib();
}

} // of of namespace
