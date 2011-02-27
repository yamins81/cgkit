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

#include "sor_triangulator.h"
#include <cmath>

#include "opengl.h"

namespace support3d {

/**
  Draw the surface of revolution using OpenGL.
 */
void SORTriangulator::drawGL(double startangle, double endangle,
			     int segmentsu, SORVertexList& vlist)
{
  int segmentsv = vlist.size()-1;
  // True if the first silhouette point lies on the rotation axis
  bool ov_first = fabs(vlist[0].px)<1E-8;
  // True if the last silhouette point lies on the rotation axis
  bool ov_last = fabs(vlist[segmentsv].px)<1E-8;
  int i, j;

  for(i=0; i<segmentsu; i++)
  {
    double u1 = double(i)/segmentsu;
    double u2 = double((i+1)%segmentsu)/segmentsu;
    double angle1 = (1.0-u1)*startangle + u1*endangle;
    double angle2 = (1.0-u2)*startangle + u2*endangle;
    angle1 *= 3.1415926535897931/180.0;
    angle2 *= 3.1415926535897931/180.0;
    double s1 = sin(angle1);
    double c1 = cos(angle1);
    double s2 = sin(angle2);
    double c2 = cos(angle2);

    glBegin(GL_QUAD_STRIP);
    u2 = double(i+1)/segmentsu;   // without modulo for the texture coordinate
    for(j=0; j<=segmentsv; j++)
    {
      // 1st vertex
      double v = vlist[j].v;  // double(j)/segmentsv;
      if ((ov_first && j==0) || (ov_last && j==segmentsv))
	glTexCoord2d(u1 + 0.5/segmentsu, v);
      else
	glTexCoord2d(u1, v);
      glNormal3d(c1*vlist[j].nx, s1*vlist[j].nx, vlist[j].ny);
      glVertex3d(c1*vlist[j].px, s1*vlist[j].px, vlist[j].py);

      // 2nd vertex
      if ((ov_first && j==0) || (ov_last && j==segmentsv))
	glTexCoord2d(u2 - 0.5/segmentsu, v);
      else
	glTexCoord2d(u2, v);
      glNormal3d(c2*vlist[j].nx, s2*vlist[j].nx, vlist[j].ny);
      glVertex3d(c2*vlist[j].px, s2*vlist[j].px, vlist[j].py);
    }
    glEnd();
  }
}

/**
  Initialize a TriMeshGeom.

  The silhouette vertices should be ordered in v (first vertex gets v=0).

  \param startangle Start angle in degrees
  \param endangle End angle in degrees
  \param segmentsu Number of segments in u direction
  \param vlist Vertex list of the silhouette
  \param[out] tm Triangle mesh that receives the result
 */
void SORTriangulator::convertToTriMesh(double startangle, double endangle,
				       int segmentsu,
				       SORVertexList& vlist, TriMeshGeom& tm)
{
  //  std::cout<<"convertToTriMesh()"<<std::endl;
  int segmentsv = vlist.size()-1;
  // ou: 1, if the sor is open in u direction, otherwise 0
  int ou = fabs(endangle-startangle-360.0)<1E-8? 0 : 1;
  // True if the first silhouette point lies on the rotation axis
  bool ov_first = fabs(vlist[0].px)<1E-8;
  // True if the last silhouette point lies on the rotation axis
  bool ov_last = fabs(vlist[segmentsv].px)<1E-8;
  // ov: Number of points on the rotation axis (0,1,2)
  int ov = 0;
  if ((ov_first && !ov_last) || (!ov_first && ov_last))
    ov = 1;
  if (ov_first && ov_last)
    ov = 2;

  int i, j;

  //  std::cout<<"su: "<<segmentsu<<std::endl;
  //  std::cout<<"sv: "<<segmentsv<<std::endl;
  //  std::cout<<"ou: "<<ou<<std::endl;
  //  std::cout<<"ov: "<<ov<<std::endl;

  tm.deleteAllVariables();

  ////////// Set vertices /////////////

  tm.verts.resize((segmentsu+ou)*(segmentsv+1-ov)+ov);
  tm.newVariable("N", VARYING, NORMAL);
  ArraySlot<vec3d>& N_slot = dynamic_cast<ArraySlot<vec3d>&>(tm.slot("N"));

  int offset=0;
  if (ov_first)
  {
    tm.verts.setValue(offset, vec3d(0, 0, vlist[0].py));
    N_slot.setValue(offset, vec3d(0, 0, vlist[0].ny).normalize());
    offset++;
  }
  if (ov_last)
  {
    tm.verts.setValue(offset, vec3d(0,0, vlist[vlist.size()-1].py));
    N_slot.setValue(offset, vec3d(0, 0, vlist[vlist.size()-1].ny).normalize());
    offset++;
  }

  int idx = offset;
  for(i=0; i<segmentsu+ou; i++)
  {
    double t = double(i)/segmentsu;
    double angle = (1.0-t)*startangle + t*endangle;
    angle *= 3.1415926535897931/180.0;
    double s = sin(angle);
    double c = cos(angle);
    for(j=ov_first? 1:0; j<segmentsv+(ov_last? 0:1); j++)
    {
      vec3d v(c*vlist[j].px, s*vlist[j].px, vlist[j].py);
      vec3d n(c*vlist[j].nx, s*vlist[j].nx, vlist[j].ny);
      tm.verts.setValue(idx, v);
      N_slot.setValue(idx, n);
      idx++;
    }
  }

  ////////// Set texture verts //////////////

  tm.newVariable("st", USER, FLOAT, 2, (segmentsu+1)*(segmentsv+1)-ov);
  ArraySlot<double>& st_slot = dynamic_cast<ArraySlot<double>&>(tm.slot("st"));
  double st[2];

  idx = 0;
  for(i=0; i<=segmentsu; i++)
  {
    for(j=0; j<=segmentsv; j++)
    {
      // Leave out the first/last point of the last column if
      // the first/last point lies on the rotation axis
      if ((ov_first || ov_last) && i==segmentsu && (j==0 || j==segmentsv))
	continue;
      if (ov_first && j==0)
	st[0] = double(i)/segmentsu + 0.5/segmentsu;
      else if (ov_last && j==segmentsv)
	st[0] = double(i)/segmentsu + 0.5/segmentsu;
      else
	st[0] = double(i)/segmentsu;
      st[1] = vlist[j].v;
      st_slot.setValues(idx, st);
      idx++;
    }
  }

  ////////// Set faces & tex faces /////////////

  tm.faces.resize(2*(segmentsu*segmentsv) - ov*segmentsu);
  tm.newVariable("stfaces", UNIFORM, INT, 3);
  ArraySlot<int>& stfaces_slot = dynamic_cast<ArraySlot<int>&>(tm.slot("stfaces"));

  int numpercol = segmentsv+1-ov;  // #verts per column
  int f[3];
  int tf[3];
  idx=0;

  // Create the "stripes" along v...

  for(i=0; i<segmentsu;i++)
  {
    int offset1 = offset + i*numpercol;
    int offset2 = offset1 + numpercol;
    // modulo
    if (ou==0 && i==segmentsu-1)
      offset2 = offset;

    for(j=0; j<segmentsv; j++)
    {
      // Special treatment of first segment?
      if (j==0 && ov_first)
      {
	f[0] = offset1;
	f[1] = 0;
	f[2] = offset2;
	tm.faces.setValues(idx, f);
	tf[1] = i*(segmentsv+1);
	tf[0] = tf[1]+1;
	tf[2] = tf[0]+(segmentsv+1);
	if (ov_first && i==segmentsu-1)
	  tf[2] -= 1;
	stfaces_slot.setValues(idx, tf);
	idx++;
	offset1 -= 1;
	offset2 -= 1;
      }
      // Special treatment of last segment?
      else if (j==segmentsv-1 && ov_last)
      {
	f[0] = offset1+j;
	f[1] = offset2+j;
	f[2] = (ov_first)? 1:0;
	tm.faces.setValues(idx, f);
	tf[0] = i*(segmentsv+1)+j;
	tf[1] = tf[0]+(segmentsv+1);
	if (ov_first && i==segmentsu-1)
	  tf[1] -= 1;
	tf[2] = tf[0]+1;
	stfaces_slot.setValues(idx, tf);
	idx++;
      }
      // Inner segment
      else
      {
	f[0] = offset1+j;
	f[1] = offset2+j;
	f[2] = offset2+j+1;
	tm.faces.setValues(idx, f);
	tf[0] = i*(segmentsv+1)+j;
	tf[1] = tf[0]+(segmentsv+1);
	if (ov_first && i==segmentsu-1)
	  tf[1] -= 1;
	tf[2] = tf[1]+1;
	stfaces_slot.setValues(idx, tf);
	idx++;
	f[1] = f[2];
	f[2] = f[0]+1;
	tm.faces.setValues(idx, f);
	tf[1] = tf[2];
	tf[2] = tf[0]+1;
	stfaces_slot.setValues(idx, tf);
	idx++;
      }
    }
  }

}

}  // end of namespace

