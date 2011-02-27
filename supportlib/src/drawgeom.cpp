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

#include "drawgeom.h"
#include "sor_triangulator.h"

#include "opengl.h"

namespace support3d {

DrawGeom::DrawGeom()
: markers(0), lines()
{
}

DrawGeom::~DrawGeom()
{
}

// Calculate bounding box
BoundingBox DrawGeom::boundingBox()
{
  BoundingBox bb;

  // Markers...
  std::vector<D_Marker>::iterator mit;
  for(mit=markers.begin(); mit!=markers.end(); mit++)
  {
    bb.addPoint(mit->pos);
  }

  // Lines...
  std::vector<D_Line>::iterator lit;
  for(lit=lines.begin(); lit!=lines.end(); lit++)
  {
    bb.addPoint(lit->pos1);
    bb.addPoint(lit->pos2);
  }

  return bb;
}

// Draw the objects
void DrawGeom::drawGL()
{
  SORTriangulator sor;
  SORTriangulator::SORVertexList vlist;
  SORTriangulator::SORVertex vert(0, -1, 0, 0, 0);
  vlist.push_back(vert);
  vert.set(1, 0, 0, 0, 0);
  vlist.push_back(vert);
  vert.set(0, 1, 0, 0, 0);
  vlist.push_back(vert);

  glPushAttrib(GL_ENABLE_BIT);
  glDisable(GL_LIGHTING);

  // Draw markers (points)...
  std::vector<D_Marker>::iterator mit;
//  glBegin(GL_POINTS);
  for(mit=markers.begin(); mit!=markers.end(); mit++)
  {
    glColor3d(mit->col.x, mit->col.y, mit->col.z);
//    glPointSize(mit->size);
//    glVertex3d(mit->pos.x, mit->pos.y, mit->pos.z);
    glPushMatrix();
    glTranslated(mit->pos.x, mit->pos.y, mit->pos.z);
    glScaled(mit->size, mit->size, mit->size);
    sor.drawGL(0.0, 360.0, 4, vlist);
    glPopMatrix();
  }
//  glEnd();

  // Draw lines...
  std::vector<D_Line>::iterator lit;
  glBegin(GL_LINES);
  for(lit=lines.begin(); lit!=lines.end(); lit++)
  {
    glColor3d(lit->col.x, lit->col.y, lit->col.z);
    // The following generates an error which later lead to an Exception
    // if Python OpenGL calls were made (at least on my machine)
//    glLineWidth(lit->size);
    glVertex3d(lit->pos1.x, lit->pos1.y, lit->pos1.z);
    glVertex3d(lit->pos2.x, lit->pos2.y, lit->pos2.z);
  }
  glEnd();

  glPopAttrib();
}

/**
  Clear all stored objects.
 */
void DrawGeom::clear()
{
  markers.clear();
  lines.clear();
}

/**
  Add a marker object.
 */
void DrawGeom::marker(const vec3d& pos, const vec3d& col, float size)
{
  D_Marker m(pos, col, size);
  markers.push_back(m);
}

/**
  Add a line object.
 */
void DrawGeom::line(const vec3d& pos1, const vec3d& pos2, const vec3d& col, float size)
{
  D_Line l(pos1, pos2, col, size);
  lines.push_back(l);
}


}  // end of namespace

