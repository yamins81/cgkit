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

#define DLL_EXPORT_SPHEREGEOM
#include "torusgeom.h"
#include "trimeshgeom.h"
#include "fixedsizeconstraints.h"
#include <cmath>

#include "opengl.h"

// Obviously some header on Linux creates stray macros "major" and "minor".
// Undo this bad habit so it doesn't interfere with the below variables...
#ifdef major
#undef major
#endif
#ifdef minor
#undef minor
#endif

namespace support3d {

TorusGeom::TorusGeom(double amajor, double aminor, int segsu, int segsv)
 : major(amajor,0), minor(aminor,0),
  segmentsu(segsu,0), segmentsv(segsv,0),
  cog(vec3d(0,0,0), Slot<vec3d>::NO_INPUT_CONNECTIONS),
  inertiatensor()
{
  inertiatensor.setProcedure(this, &TorusGeom::computeInertiaTensor);
  major.addDependent(&inertiatensor);
  minor.addDependent(&inertiatensor);
  addSlot("major", major);
  addSlot("minor", minor);
  addSlot("cog", cog);
  addSlot("inertiatensor", inertiatensor);
  addSlot("segmentsu", segmentsu);
  addSlot("segmentsv", segmentsv);
}

TorusGeom::~TorusGeom()
{
  major.removeDependent(&inertiatensor);
  minor.removeDependent(&inertiatensor);
}

BoundingBox TorusGeom::boundingBox()
{
  double minr = minor.getValue();
  double maj = major.getValue()+minr;
  BoundingBox res(vec3d(-maj,-maj,-minr), vec3d(maj,maj,minr));
  return res;
}

void TorusGeom::drawGL()
{
  SORTriangulator sor;
  SORTriangulator::SORVertexList vlist;
  int segsu = segmentsu.getValue();

  if (segsu<3)
    segsu=3;

  createSilhouette(vlist);
  sor.drawGL(0.0, 360.0, segsu, vlist);
}

// slotSizeConstraint
boost::shared_ptr<SizeConstraintBase> TorusGeom::slotSizeConstraint(VarStorage storage) const
{
  switch(storage)
  {
  case UNIFORM:
    return sizeConstraint_one;
  case VARYING:
  case VERTEX:
    return sizeConstraint_four;
  default:
    return sizeConstraint_zero;
  }
}

/**
  Computes the inertia tensor of the torus.

  The inertia tensor of the torus with respect to its local
  coordinate system L is computed.

  This method is used for the inertiatensor slot.

  \param[out] tensor Receives the result.
  \todo Compute inertia tensor
 */
void TorusGeom::computeInertiaTensor(mat3d& tensor)
{
  tensor = mat3d(0);
}


/**
  Convert to TriMesh
 */
void TorusGeom::convert(GeomObject* target)
{
  TriMeshGeom* tm = dynamic_cast<TriMeshGeom*>(target);

  // Check if the target geom is really a TriMesh
  if (tm==0)
  {
    throw ENotImplementedError("Conversion not supported by the TorusGeom");
  }

  SORTriangulator sor;
  SORTriangulator::SORVertexList vlist;
  int segsu = segmentsu.getValue();

  if (segsu<3)
    segsu=3;

  createSilhouette(vlist);
  sor.convertToTriMesh(0.0, 360.0, segsu, vlist, *tm);
}

/**
  Helper method.

  This method creates the silhouette for the SORTriangulator class.

  \param[out] vlist Empty vertex list which will receive the result
  \todo Is silhouette closed? (in v direction?)
 */
void TorusGeom::createSilhouette(SORTriangulator::SORVertexList& vlist)
{
  double majr = major.getValue();
  double minr = minor.getValue();
  int segsv = segmentsv.getValue();
  int i;

  if (segsv<3)
    segsv=3;

  // Create silhouette...
  for(i=0; i<=segsv; i++)
  {
    double v = double(i)/segsv;
    double t = v*2*3.1415926535897931;
    double nx = cos(t);
    double ny = sin(t);
    double px = minr*nx;
    double py = minr*ny;
    SORTriangulator::SORVertex vert(majr+px, py, nx, ny, v);
    vlist.push_back(vert);
  }
}


}  // end of namespace
