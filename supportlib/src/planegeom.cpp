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

#include "planegeom.h"
#include "trimeshgeom.h"
#include "fixedsizeconstraints.h"

#include "opengl.h"

namespace support3d {

/**
  Constructor.

  \param alx Length in x direction
  \param aly Length in y direction
  \param segsx Number of segments in x
  \param segsy Number of segments in y
 */
PlaneGeom::PlaneGeom(double alx, double aly, int segsx, int segsy)
: lx(alx,0), ly(aly,0), segmentsx(segsx,0), segmentsy(segsy,0)
{
  addSlot("lx", lx);
  addSlot("ly", ly);
  addSlot("segmentsx", segmentsx);
  addSlot("segmentsy", segmentsy);
}

BoundingBox PlaneGeom::boundingBox()
{
  double lenx = lx.getValue()/2.0;
  double leny = ly.getValue()/2.0;
  BoundingBox res(vec3d(-lenx,-leny,0), vec3d(lenx,leny,0));
  return res;
}

void PlaneGeom::drawGL()
{
  double lenx = lx.getValue();
  double leny = ly.getValue();
  double lenx2 = lenx/2.0;
  double leny2 = leny/2.0;
  int segsx = segmentsx.getValue();
  int segsy = segmentsy.getValue();
  if (segsx<1)
    segsx=1;
  if (segsy<1)
    segsy=1;

  glNormal3f(0.0f, 0.0f, 1.0f);
  for(int j=0; j<segsy; j++)
  {
    double y1 = j*leny/segsy - leny2;
    double y2 = (j+1)*leny/segsy - leny2;
    double v1 = double(j)/segsy;
    double v2 = double(j+1)/segsy;
    glBegin(GL_QUAD_STRIP);
//    glTexCoord2d(0, v1);
//    glVertex2d(-lenx2, y1);
//    glTexCoord2d(0, v2);
//    glVertex2d(-lenx2, y2);
    for(int i=0; i<=segsx; i++)
    {
      double x = i*lenx/segsx - lenx2;
      double u = double(i)/segsx;
      glTexCoord2d(u, v1);
      glVertex2d(x, y1);
      glTexCoord2d(u, v2);
      glVertex2d(x, y2);
    }
    glEnd();
  }

/*  glBegin(GL_QUADS);
  glNormal3f(0.0f, 0.0f, 1.0f);
  glVertex3d(-lenx, -leny, 0);
  glVertex3d( lenx, -leny, 0);
  glVertex3d( lenx,  leny, 0);
  glVertex3d(-lenx,  leny, 0);
  glEnd();
  */
}

// slotSizeConstraint
boost::shared_ptr<SizeConstraintBase> PlaneGeom::slotSizeConstraint(VarStorage storage) const
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
  Convert the geom object into another geom object.

  Currently the geom can only be converted into a TriMeshGeom.

  \image html plane_geom.png

 \todo Bereits vorhandene Variablen mit übernehmen
*/
void PlaneGeom::convert(GeomObject* target)
{
  TriMeshGeom* tm = dynamic_cast<TriMeshGeom*>(target);

  // Check if the target geom is really a TriMesh
  if (tm==0)
  {
    throw ENotImplementedError("Conversion not supported by the PlaneGeom");
  }

  double lenx = lx.getValue();
  double leny = ly.getValue();
  double lenx2 = lenx/2.0;
  double leny2 = leny/2.0;
  int segsx = segmentsx.getValue();
  int segsy = segmentsy.getValue();
  int i,j;
  if (segsx<1)
    segsx=1;
  if (segsy<1)
    segsy=1;


  // Set number of verts and faces
  tm->deleteAllVariables();
  tm->verts.resize((segsx+1)*(segsy+1));
  tm->faces.resize(2*segsx*segsy);
  tm->newVariable("st", VARYING, FLOAT, 2);

  ArraySlot<double>* stslot = dynamic_cast<ArraySlot<double>*>(&(tm->slot("st")));

  // Set vertices...
  for(j=0; j<=segsy; j++)
  {
    double v = double(j)/segsy;
    double y = v*leny - leny2;
    for(i=0; i<=segsx; i++)
    {
      double u = double(i)/segsx;
      double x = u*lenx - lenx2;
      int idx = j*(segsx+1)+i;
      tm->verts.setValue(idx, vec3d(x,y));
      double st[2] = {u,v};
      stslot->setValues(idx, st);
    }
  }

  // Set faces...
  for(j=0; j<segsy; j++)
  {
    for(i=0; i<segsx; i++)
    {
      int f[3];
      int idx = 2*(j*segsx + i);
      f[0] = j*(segsx+1) + i;
      f[1] = f[0]+1;
      f[2] = f[1]+segsx+1;
      tm->faces.setValues(idx, f);
      f[1] = f[2];
      f[2] = f[1]-1;
      tm->faces.setValues(idx+1, f);
    }
  }

}


}  // end of namespace
