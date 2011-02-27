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

#include "boxgeom.h"
#include "trimeshgeom.h"
#include "fixedsizeconstraints.h"

#include "opengl.h"

namespace support3d {

BoxGeom::BoxGeom(double alx, double aly, double alz, int segsx, int segsy, int segsz)
: lx(alx,0), ly(aly,0), lz(alz,0),
  segmentsx(segsx,0), segmentsy(segsy,0), segmentsz(segsz, 0),
  cog(vec3d(0,0,0), Slot<vec3d>::NO_INPUT_CONNECTIONS),
  inertiatensor()
{
  inertiatensor.setProcedure(this, &BoxGeom::computeInertiaTensor);
  lx.addDependent(&inertiatensor);
  ly.addDependent(&inertiatensor);
  lz.addDependent(&inertiatensor);

  addSlot("lx", lx);
  addSlot("ly", ly);
  addSlot("lz", lz);
  addSlot("segmentsx", segmentsx);
  addSlot("segmentsy", segmentsy);
  addSlot("segmentsz", segmentsz);
  addSlot("cog", cog);
  addSlot("inertiatensor", inertiatensor);
}

BoxGeom::~BoxGeom()
{
  lx.removeDependent(&inertiatensor);
  ly.removeDependent(&inertiatensor);
  lz.removeDependent(&inertiatensor);
}

BoundingBox BoxGeom::boundingBox()
{
  double lenx = lx.getValue()/2.0;
  double leny = ly.getValue()/2.0;
  double lenz = lz.getValue()/2.0;
  BoundingBox res(vec3d(-lenx,-leny,-lenz), vec3d(lenx,leny,lenz));
  return res;
}

void BoxGeom::drawGL()
{
  double lenx = lx.getValue();
  double leny = ly.getValue();
  double lenz = lz.getValue();
  double lenx2 = lenx/2.0;
  double leny2 = leny/2.0;
  double lenz2 = lenz/2.0;
  int segsx = segmentsx.getValue();
  int segsy = segmentsy.getValue();
  int segsz = segmentsz.getValue();
  double a,b, x1,y1,y2,z1,z2, u,v1,v2;
  int i,j;

  glPushMatrix();

  // Draw the first 4 sides around Z...
  for(j=0; j<segsz; j++)
  {
    a = double(j)/segsz;
    b = double(j+1)/segsz;
    z1 = (a-1.0)*lenz2 + a*lenz2;
    z2 = (b-1.0)*lenz2 + b*lenz2;
    v1 = a;
    v2 = b;

    // Draw side XZ at -Y
    glBegin(GL_QUAD_STRIP);
    glNormal3f(0,0,-1.0f);
    for(i=0; i<=segsx; i++)
    {
      u = double(i)/segsx;
      x1 = (u-1.0)*lenx2 + u*lenx2;
      glTexCoord2d(u,v2);
      glVertex3d(x1, -leny2, z2);
      glTexCoord2d(u,v1);
      glVertex3d(x1, -leny2, z1);
    }
    glEnd();

    // Draw side YZ at X
    glBegin(GL_QUAD_STRIP);
    glNormal3f(1.0f,0,0);
    for(i=0; i<=segsy; i++)
    {
      u = double(i)/segsy;
      y1 = (u-1.0)*leny2 + u*leny2;
      glTexCoord2d(u,v2);
      glVertex3d(lenx2, y1, z2);
      glTexCoord2d(u,v1);
      glVertex3d(lenx2, y1, z1);
    }
    glEnd();

    // Draw side XZ at Y
    glBegin(GL_QUAD_STRIP);
    glNormal3f(0,1.0f,0);
    for(i=0; i<=segsx; i++)
    {
      u = double(i)/segsx;
      x1 = (1.0-u)*lenx2 - u*lenx2;
      glTexCoord2d(u,v2);
      glVertex3d(x1, leny2, z2);
      glTexCoord2d(u,v1);
      glVertex3d(x1, leny2, z1);
    }
    glEnd();

    // Draw side YZ at -X
    glBegin(GL_QUAD_STRIP);
    glNormal3f(-1.0f,0,0);
    for(i=0; i<=segsy; i++)
    {
      u = double(i)/segsy;
      y1 = (1.0-u)*leny2 - u*leny2;
      glTexCoord2d(u,v2);
      glVertex3d(-lenx2, y1, z2);
      glTexCoord2d(u,v1);
      glVertex3d(-lenx2, y1, z1);
    }
    glEnd();
  }

  // Draw the "top" (XY side at Z)...
  for(j=0; j<segsy; j++)
  {
    a = double(j)/segsy;
    b = double(j+1)/segsy;
    y1 = (a-1.0)*leny2 + a*leny2;
    y2 = (b-1.0)*leny2 + b*leny2;
    v1 = a;
    v2 = b;
    glBegin(GL_QUAD_STRIP);
    glNormal3f(0,0,1.0f);
    for(i=0; i<=segsx; i++)
    {
      u = double(i)/segsx;
      x1 = (u-1.0)*lenx2 + u*lenx2;
      glTexCoord2d(u,v2);
      glVertex3d(x1, y2, lenz2);
      glTexCoord2d(u,v1);
      glVertex3d(x1, y1, lenz2);
    }
    glEnd();
  }

  // Draw the "bottom" (XY side at -Z)...
  for(j=0; j<segsy; j++)
  {
    a = double(j)/segsy;
    b = double(j+1)/segsy;
    y1 = (1.0-a)*leny2 - a*leny2;
    y2 = (1.0-b)*leny2 - b*leny2;
    v1 = a;
    v2 = b;
    glBegin(GL_QUAD_STRIP);
    glNormal3f(0,0,-1.0f);
    for(i=0; i<=segsx; i++)
    {
      u = double(i)/segsx;
      x1 = (u-1.0)*lenx2 + u*lenx2;
      glTexCoord2d(u,v2);
      glVertex3d(x1, y2, -lenz2);
      glTexCoord2d(u,v1);
      glVertex3d(x1, y1, -lenz2);
    }
    glEnd();
  }

  glPopMatrix();
}

// slotSizeConstraint
boost::shared_ptr<SizeConstraintBase> BoxGeom::slotSizeConstraint(VarStorage storage) const
{
  switch(storage)
  {
  case UNIFORM:
    return sizeConstraint_six;
  case VARYING:
  case VERTEX:
    return sizeConstraint_eight;
  default:
    return sizeConstraint_zero;
  }
}

/**
  Computes the inertia tensor of the box.

  The inertia tensor of the sphere with respect to its local
  coordinate system L is computed.

  This method is used for the inertiatensor slot.

  \param[out] tensor Receives the result.
 */
void BoxGeom::computeInertiaTensor(mat3d& tensor)
{
  double lenx = lx.getValue();
  double leny = ly.getValue();
  double lenz = lz.getValue();
  tensor = mat3d(1);
  tensor.setDiag((leny*leny+lenz*lenz)/12.0, (lenx*lenx+lenz*lenz)/12.0, (lenx*lenx+leny*leny)/12.0);
}

/**
  Convert to TriMesh

  \todo Create texture coordinates
 */
void BoxGeom::convert(GeomObject* target)
{
  TriMeshGeom* tm = dynamic_cast<TriMeshGeom*>(target);

  // Check if the target geom is really a TriMesh
  if (tm==0)
  {
    throw ENotImplementedError("Conversion not supported by the BoxGeom");
  }


  double lenx = lx.getValue();
  double leny = ly.getValue();
  double lenz = lz.getValue();
  double lenx2 = lenx/2.0;
  double leny2 = leny/2.0;
  double lenz2 = lenz/2.0;
  int segsx = segmentsx.getValue();
  int segsy = segmentsy.getValue();
  int segsz = segmentsz.getValue();
  int i,j, n;
  int f[3];
  double a, b;
  if (segsx<1)
    segsx=1;
  if (segsy<1)
    segsy=1;
  if (segsz<1)
    segsz=1;

  // Set number of verts and faces
  tm->deleteAllVariables();
  tm->verts.resize(2*(segsx+segsy)*(segsz+1) + 2*(segsx-1)*(segsy-1));
  tm->faces.resize(4*(segsx+segsy)*segsz + 4*segsx*segsy);
  tm->newVariable("st", VARYING, FLOAT, 2);

  ArraySlot<double>* stslot = dynamic_cast<ArraySlot<double>*>(&(tm->slot("st")));

  // Set vertices...
  vec3d v;
  int idx=0;

  for(j=0; j<=segsz; j++)  // Sides...
  {
    a = double(j)/segsz;
    v.z = (a-1.0)*lenz2 + a*lenz2;

    v.y = -leny2;
    for(i=0; i<segsx; i++)
    {
      a = double(i)/segsx;
      v.x = (a-1.0)*lenx2 + a*lenx2;
      tm->verts.setValue(idx, v);
      idx++;
    }

    v.x = lenx2;
    for(i=0; i<segsy; i++)
    {
      a = double(i)/segsy;
      v.y = (a-1.0)*leny2 + a*leny2;
      tm->verts.setValue(idx, v);
      idx++;
    }

    v.y = leny2;
    for(i=0; i<segsx; i++)
    {
      a = double(segsx-i)/segsx;
      v.x = (a-1.0)*lenx2 + a*lenx2;
      tm->verts.setValue(idx, v);
      idx++;
    }

    v.x = -lenx2;
    for(i=0; i<segsy; i++)
    {
      a = double(segsy-i)/segsy;
      v.y = (a-1.0)*leny2 + a*leny2;
      tm->verts.setValue(idx, v);
      idx++;
    }
  }

  n = (segsx-1)*(segsy-1);
  for(j=1; j<segsy; j++)  // Top/bottom
  {
    for(i=1; i<segsx; i++)
    {
      a = double(i)/segsx;
      v.x = (a-1.0)*lenx2 + a*lenx2;
      b = double(j)/segsy;
      v.y = (b-1.0)*leny2 + b*leny2;
      v.z = lenz2;
      tm->verts.setValue(idx, v);
      v.z = -lenz2;
      tm->verts.setValue(idx+n, v);
      idx++;
    }
  }

  // Set faces...
  idx = 0;
  n = 2*(segsx+segsy);
  for(j=0; j<segsz; j++)  // Sides
  {
    for(i=0; i<n; i++)
    {
      f[0] = j*n + i;
      f[1] = j*n + ((i+1)%n);
      f[2] = (j+1)*n + ((i+1)%n);
      tm->faces.setValues(idx, f);
      idx++;

      f[0] = f[2];
      f[1] = (j+1)*n + i;
      f[2] = j*n + i;
      tm->faces.setValues(idx, f);
      idx++;
    }
  }

  int offset = 2*(segsx+segsy)*(segsz+1);
  int topoffset = 2*(segsx+segsy)*segsz;
  for(j=0; j<segsy; j++)  // Top
  {
    for(i=0; i<segsx; i++)
    {
      f[0] = _vertexIndex(i, j, segsx, segsy, offset, topoffset);
      f[1] = _vertexIndex(i+1, j, segsx, segsy, offset, topoffset);
      f[2] = _vertexIndex(i+1, j+1, segsx, segsy, offset, topoffset);
      tm->faces.setValues(idx, f);
      idx++;

      f[0] = _vertexIndex(i+1, j+1, segsx, segsy, offset, topoffset);
      f[1] = _vertexIndex(i, j+1, segsx, segsy, offset, topoffset);
      f[2] = _vertexIndex(i, j, segsx, segsy, offset, topoffset);
      tm->faces.setValues(idx, f);
      idx++;
    }
  }

  offset = 2*(segsx+segsy)*(segsz+1) + (segsx-1)*(segsy-1);
  topoffset = 0;
  for(j=0; j<segsy; j++)  // Bottom
  {
    for(i=0; i<segsx; i++)
    {
      f[0] = _vertexIndex(i, j, segsx, segsy, offset, topoffset);
      f[1] = _vertexIndex(i, j+1, segsx, segsy, offset, topoffset);
      f[2] = _vertexIndex(i+1, j+1, segsx, segsy, offset, topoffset);
      tm->faces.setValues(idx, f);
      idx++;

      f[0] = _vertexIndex(i+1, j+1, segsx, segsy, offset, topoffset);
      f[1] = _vertexIndex(i+1, j, segsx, segsy, offset, topoffset);
      f[2] = _vertexIndex(i, j, segsx, segsy, offset, topoffset);
      tm->faces.setValues(idx, f);
      idx++;
    }
  }
}

int BoxGeom::_vertexIndex(int i, int j, int segsx, int segsy, int offset, int topoffset)
{
  if (j==0)
    return i+topoffset;
  if (i==segsx)
    return segsx+j+topoffset;
  if (j==segsy)
    return 2*segsx+segsy-i+topoffset;
  if (i==0)
    return 2*(segsx+segsy)-j+topoffset;
  return offset+(j-1)*(segsx-1)+(i-1);
}

}  // end of namespace
