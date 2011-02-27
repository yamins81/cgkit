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

#ifndef DRAWGEOM_H
#define DRAWGEOM_H

/** \file drawgeom.h
 Contains the "draw" geometry class (which just contains markers, lines, etc.)
 */

#include <vector>
#include "geomobject.h"
#include "vec3.h"

namespace support3d {

/// Holds the parameters of a "Draw" marker.
struct D_Marker
{
  vec3d pos;
  vec3d col;
  float size;

  D_Marker() {}
  D_Marker(const vec3d& apos, const vec3d& acol, float asize)
    : pos(apos), col(acol), size(asize) {}
};

/// Holds the parameters of a "Draw" line.
struct D_Line
{
  vec3d pos1;
  vec3d pos2;
  vec3d col;
  float size;

  D_Line() {}
  D_Line(const vec3d& apos1, const vec3d& apos2, const vec3d& acol, float asize)
    : pos1(apos1), pos2(apos2), col(acol), size(asize) {}
};

/**
  Draw geometry.

  This class represents a collection of markers and lines.
 */
class DrawGeom : public GeomObject
{
  public:
  std::vector<D_Marker> markers;
  std::vector<D_Line> lines;

  public:
  DrawGeom();
  virtual ~DrawGeom();

  virtual BoundingBox boundingBox();
  virtual void drawGL();

  void clear();
  void marker(const vec3d& pos, const vec3d& col=vec3d(1,1,1), float size=1.0f);
  void line(const vec3d& pos1, const vec3d& pos2, const vec3d& col=vec3d(1,1,1), float size=1.0f);
};


}  // end of namespace

#endif
