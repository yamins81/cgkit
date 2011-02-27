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

#include "ccylindergeom.h"
#include "fixedsizeconstraints.h"
#include <cmath>

#include "opengl.h"

namespace support3d {

CCylinderGeom::CCylinderGeom(double aradius, double alength, int segsu, int segsvl, int segsvr)
: radius(aradius,0), length(alength,0),
  segmentsu(segsu,0), segmentsvl(segsvl,0), segmentsvr(segsvr,0),
  cog(vec3d(0,0,0), Slot<vec3d>::NO_INPUT_CONNECTIONS),
  inertiatensor()
{
  inertiatensor.setProcedure(this, &CCylinderGeom::computeInertiaTensor);
  radius.addDependent(&inertiatensor);
  length.addDependent(&inertiatensor);
  addSlot("radius", radius);
  addSlot("length", length);
  addSlot("cog", cog);
  addSlot("inertiatensor", inertiatensor);
  addSlot("segmentsu", segmentsu);
  addSlot("segmentsvl", segmentsvl);
  addSlot("segmentsvr", segmentsvr);
}

CCylinderGeom::~CCylinderGeom()
{
  radius.removeDependent(&inertiatensor);
  length.removeDependent(&inertiatensor);
}

BoundingBox CCylinderGeom::boundingBox()
{
  double r = radius.getValue();
  double l2 = length.getValue()/2.0;
  BoundingBox res(vec3d(-r,-r,-l2-r), vec3d(r,r,l2+r));
  return res;
}

void CCylinderGeom::drawGL()
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
boost::shared_ptr<SizeConstraintBase> CCylinderGeom::slotSizeConstraint(VarStorage storage) const
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
  Computes the inertia tensor of the capped cylinder.

  The inertia tensor of the cylinder with respect to its local
  coordinate system L is computed.

  This method is used for the inertiatensor slot.

  \param[out] tensor Receives the result.
 */
void CCylinderGeom::computeInertiaTensor(mat3d& tensor)
{
  double R = radius.getValue();
  double L = length.getValue();
  double R2 = R*R;
  double L2 = L*L;
  double Acyl = L+(4.0/3.0)*R;
  double Asph;
  double a,b;

  // l==0 and r==0?
  if (xabs(Acyl)<=vec3d::epsilon)
  {
    tensor = mat3d(0);
    return;
  }

  // Cylinder mass (under the assumption that the total mass is 1.0)
  Acyl = L/Acyl;
  // Sphere mass
  Asph = 1.0-Acyl;

  a = Acyl*(0.25*R2 + L2/12.0) + Asph*(0.4*R2 + 0.375*R*L + 0.25*L2);
  b = (0.5*Acyl + 0.4*Asph)*R2;
  tensor = mat3d();
  tensor.setDiag(a,a,b);
}

/**
  Convert to TriMesh
 */
void CCylinderGeom::convert(GeomObject* target)
{
  TriMeshGeom* tm = dynamic_cast<TriMeshGeom*>(target);

  // Check if the target geom is really a TriMesh
  if (tm==0)
  {
    throw ENotImplementedError("Conversion not supported by the CCylinderGeom");
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
 */
void CCylinderGeom::createSilhouette(SORTriangulator::SORVertexList& vlist)
{
  double rad = radius.getValue();
  double len = length.getValue();
  double len2 = len/2.0;
  double silhouette_len = 3.1415926535897931*rad+len;
  double cap_len = 1.5707963267948966*rad;
  double v1 = cap_len/silhouette_len;  // v at the end of the lower cap
  double v2 = v1+len/silhouette_len;   // v at the end of the cylinder
  int segsvl = segmentsvl.getValue();
  int segsvr = segmentsvr.getValue();
  int i;

  if (segsvl<1)
    segsvl=1;
  if (segsvr<1)
    segsvr=1;

  // Create silhouette...

  // lower cap
  for(i=0; i<segsvr; i++)
  {
    double v = double(i)/segsvr;
    double t = v*1.5707963267948966;  // v*pi/2
    double nx = sin(t);
    double ny = -cos(t);
    double px = rad*nx;
    double py = -len2 + rad*ny;
    if (i==0)
    {
      px = 0;
      py = -len2-rad;
      nx = 0;
      ny = -1;
    }
    SORTriangulator::SORVertex vert(px, py, nx, ny, v*v1);
    vlist.push_back(vert);
  }

  // cylinder
  for(i=0; i<=segsvl; i++)
  {
    double v = double(i)/segsvl;
    double px = rad;
    double py = v*len2 - (1.0-v)*len2;
    SORTriangulator::SORVertex vert(px, py, 1.0, 0.0, v1+v*len/silhouette_len);
    vlist.push_back(vert);
  }

  // upper cap
  for(i=1; i<=segsvr; i++)
  {
    double v = double(i)/segsvr;
    double t = v*1.5707963267948966;   // v*pi/2
    double nx = cos(t);
    double ny = sin(t);
    double px = rad*nx;
    double py = len2 + rad*ny;
    if (i==segsvr)
    {
      px = 0;
      py = len2+rad;
      nx = 0;
      ny = 1;
    }
    SORTriangulator::SORVertex vert(px, py, nx, ny, v2+v*v1);
    vlist.push_back(vert);
  }
}


}  // end of namespace
