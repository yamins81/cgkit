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

#ifndef SORTRIANGULATOR_H
#define SORTRIANGULATOR_H

/** \file sor_triangulator.h
 Contains the SORTriangulator class.
 */

#include <iostream>
#include <vector>
#include "vec3.h"
#include "trimeshgeom.h"

namespace support3d {

/**
  A silhouette vertex for a surface of revolution.

  \see SORTriangulator
 */
class _SORTri_vertex
{
  public:
  _SORTri_vertex(double apx, double apy, double anx, double any, double av=0.0)
    : px(apx), py(apy), nx(anx), ny(any), v(av) {}

  void set(double apx, double apy, double anx=0, double any=0, double av=0.0)
    { px=apx; py=apy; nx=anx; ny=any; v=av; }

  // 2D point position
  double px;
  double py;
  // Normal
  double nx;
  double ny;
  // Texture coordinate (v direction)
  double v;
};

/**
  Surface of Revolution triangulator.

 */
class SORTriangulator
{
  public:
  typedef _SORTri_vertex SORVertex;
  typedef std::vector<SORVertex> SORVertexList;

  public:
  SORTriangulator() {}
  ~SORTriangulator() {}

  void drawGL(double startangle, double endangle, int segmentsu, SORVertexList& vlist);
  void convertToTriMesh(double startangle, double endangle, int segmentsu,
			SORVertexList& vlist, TriMeshGeom& tm);
};


}  // end of namespace

#endif
