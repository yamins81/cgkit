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

#include "trimeshgeom.h"
#include "massproperties.h"
#include "primvaraccess.h"

#include "opengl.h"

namespace support3d {

TriMeshGeom::TriMeshGeom()
: _on_verts_event(), _on_faces_event(),
  verts(), faces(3),
  cog(), inertiatensor(),
  _cog(), _inertiatensor(), _volume(),
  bb_cache(),
  mass_props_valid(false), bb_cache_valid(true)

{
  _on_verts_event.init(this, &TriMeshGeom::onVertsChanged, &TriMeshGeom::onVertsResize);
  verts.addDependent(&_on_verts_event);
  _on_faces_event.init(this, &TriMeshGeom::onFacesChanged, &TriMeshGeom::onFacesResize);
  faces.addDependent(&_on_faces_event);

  // Create the constraint objects for the primitive variable slots...
  SizeConstraintBase* sc = new LinearSizeConstraint(faces,1,0);
  uniformSizeConstraint = boost::shared_ptr<SizeConstraintBase>(sc);
  sc = new LinearSizeConstraint(verts,1,0);
  varyingSizeConstraint = boost::shared_ptr<SizeConstraintBase>(sc);
  sc = new LinearSizeConstraint(faces,3,0);
  faceVaryingSizeConstraint = boost::shared_ptr<SizeConstraintBase>(sc);

  cog.setProcedure(this, &TriMeshGeom::computeCog);
  inertiatensor.setProcedure(this, &TriMeshGeom::computeInertiaTensor);
  verts.addDependent(&cog);
  verts.addDependent(&inertiatensor);
  faces.addDependent(&cog);
  faces.addDependent(&inertiatensor);

  addSlot("verts", verts);
  addSlot("faces", faces);
  addSlot("cog", cog);
  addSlot("inertiatensor", inertiatensor);
}


// Return bounding box
BoundingBox TriMeshGeom::boundingBox()
{
  if (!bb_cache_valid)
  {
    bb_cache.clear();
    vec3d* vptr = verts.dataPtr();
    int size = verts.size();

    for(int i=0; i<size; i++)
    {
      bb_cache.addPoint(*vptr);
      vptr++;
    }
    bb_cache_valid = true;
  }
  return bb_cache;
}

/**
  Draw the mesh

  The primitive variables "N", "st" and "Cs" are taken into account
  during rendering. "N" contains the normals, "st" the texture coordinates
  and "Cs" the colors.
  All type variations are supported: constant, uniform, varying, facevarying
  and user + ...faces slot.

  \pre The vertex indices in the face list mustn't be out of range!
  \todo Range checking (Cs, N, Nfaces, ...)
 */
void TriMeshGeom::drawGL()
{
  PrimVarAccess<vec3d> normals(*this, std::string("N"), NORMAL, 1, std::string("Nfaces"), true);
  PrimVarAccess<double> texcoords(*this, std::string("st"), FLOAT, 2, std::string("stfaces"), true);
  PrimVarAccess<vec3d> colors(*this, std::string("Cs"), COLOR, 1, std::string("Csfaces"), true);
  vec3d* N;
  double* st;
  vec3d* Cs;

  vec3d* vertsptr = verts.dataPtr();
  int* faceptr = faces.dataPtr();
  int numfaces = faces.size();
  int ia, ib, ic;
  vec3d* a;    // Vertices
  vec3d* b;
  vec3d* c;
  vec3d Ng;
  GLfloat glcol[4] = {0,0,0,1};


  glBegin(GL_TRIANGLES);
  for(int i=0; i<numfaces; i++)
  {
    // ia/ib/ic: Indices of the three vertices
    ia = *faceptr;
    faceptr++;
    ib = *faceptr;
    faceptr++;
    ic = *faceptr;
    faceptr++;

    // Get the three triangle vertices...
    a = vertsptr+ia;
    b = vertsptr+ib;
    c = vertsptr+ic;

    // No normals? Then a face normal has to be calculated...
    if (normals.mode==0)
    {
      try
      {
	Ng.cross((*b)-(*a), (*c)-(*a));
	Ng.normalize(Ng);
      }
      catch(...)
      {
	Ng.set(0,0,0);
      };
      glNormal3d(Ng.x, Ng.y, Ng.z);
    }

    // Normals per face?
    if (normals.onFace(N))
      glNormal3d(N->x, N->y, N->z);

    // Tex coord per face? (well, why not...)
    if (texcoords.onFace(st))
      glTexCoord2dv(st);

    // Color per face?
    if (colors.onFace(Cs))
    {
      glcol[0] = GLfloat(Cs->x);
      glcol[1] = GLfloat(Cs->y);
      glcol[2] = GLfloat(Cs->z);
      glMaterialfv(GL_FRONT, GL_DIFFUSE, glcol);
    }

    // Pass the triangle to OpenGL...

    // Normals per vertex?
    if (normals.onVertex(ia, N))
      glNormal3d(N->x, N->y, N->z);
    // Color per vertex?
    if (colors.onVertex(ia, Cs))
    {
      glcol[0] = GLfloat(Cs->x);
      glcol[1] = GLfloat(Cs->y);
      glcol[2] = GLfloat(Cs->z);
      glMaterialfv(GL_FRONT, GL_DIFFUSE, glcol);
    }
    // Tex coord per vertex?
    if (texcoords.onVertex(ia, st))
      glTexCoord2dv(st);

    glVertex3d(a->x, a->y, a->z);

    // Normals per vertex?
    if (normals.onVertex(ib, N))
      glNormal3d(N->x, N->y, N->z);
    // Color per vertex?
    if (colors.onVertex(ib, Cs))
    {
      glcol[0] = GLfloat(Cs->x);
      glcol[1] = GLfloat(Cs->y);
      glcol[2] = GLfloat(Cs->z);
      glMaterialfv(GL_FRONT, GL_DIFFUSE, glcol);
    }
    // Tex coord per vertex?
    if (texcoords.onVertex(ib, st))
      glTexCoord2dv(st);

    glVertex3d(b->x, b->y, b->z);

    // Normals per vertex?
    if (normals.onVertex(ic, N))
      glNormal3d(N->x, N->y, N->z);
    // Color per vertex?
    if (colors.onVertex(ic, Cs))
    {
      glcol[0] = GLfloat(Cs->x);
      glcol[1] = GLfloat(Cs->y);
      glcol[2] = GLfloat(Cs->z);
      glMaterialfv(GL_FRONT, GL_DIFFUSE, glcol);
    }
    // Tex coord per vertex?
    if (texcoords.onVertex(ic, st))
      glTexCoord2dv(st);

    glVertex3d(c->x, c->y, c->z);
  }
  glEnd();
}

/**
   Return the appropriate size constraint for a primitive variable.
 */
boost::shared_ptr<SizeConstraintBase> TriMeshGeom::slotSizeConstraint(VarStorage storage) const
{
  switch(storage)
  {
  case UNIFORM:
    return uniformSizeConstraint;
  case VARYING:
  case VERTEX:
    return varyingSizeConstraint;
  case FACEVARYING:
  case FACEVERTEX:
    return faceVaryingSizeConstraint;
  default:
    return boost::shared_ptr<SizeConstraintBase>();
  }
}

/**
  Calculate the mass properties and update the cache.
 */
void TriMeshGeom::calcMassProperties()
{
  MassProperties mp;
  FACE f;
  vec3d* vertsptr = verts.dataPtr();
  int* faceptr = faces.dataPtr();
  int numfaces = faces.size();
  vec3d* v;

  std::cout<<"computing mass properties"<<std::endl;

  mp.setMass(1.0);

  mp.meshBegin();
  f.numVerts = 3;
  for(int i=0; i<numfaces; i++)
  {
    // Get the first vertex...
    v = vertsptr+(*faceptr);
    faceptr++;
    f.setVert(0, v->x, v->y, v->z);

    // Get the second vertex...
    v = vertsptr+(*faceptr);
    faceptr++;
    f.setVert(1, v->x, v->y, v->z);

    // Get the third vertex...
    v = vertsptr+(*faceptr);
    faceptr++;
    f.setVert(2, v->x, v->y, v->z);

    if (f.init())
      mp.face(f);
  }

  mp.meshEnd();


  _cog.set(mp.r[0], mp.r[1], mp.r[2]);
  _inertiatensor.setRow(0, vec3d(mp.J[0][0], mp.J[0][1], mp.J[0][2]));
  _inertiatensor.setRow(1, vec3d(mp.J[1][0], mp.J[1][1], mp.J[1][2]));
  _inertiatensor.setRow(2, vec3d(mp.J[2][0], mp.J[2][1], mp.J[2][2]));
  _volume = mp.volume;
  mass_props_valid = true;
}

/**
  Interscet a ray with the mesh.

  This method tests a ray with all triangles, so it's not efficient if
  you have a lot of rays to test. It's meant for only a few rays where
  the preprocessing cost wouldn't be amortized.

  The ray must be given in the local coordinate system L of the geometry.

  The ray-triangle intersection code (non-culling case) is based on:

  Tomas Möller and Ben Trumbore.<br>
  \em Fast, \em minimum \em storage \em ray-triangle \em intersection.<br>
  Journal of graphics tools, 2(1):21-28, 1997<br>
  http://www.acm.org/jgt/papers/MollerTrumbore97/<br>

  \param origin Ray origin
  \param direction Ray direction
  \param[out] info Infos about the nearest hit
  \param earlyexit If true, the method returns as soon as a triangle was hit
  \return True if there was a hit.
 */
bool TriMeshGeom::intersectRay(const vec3d& origin, const vec3d& direction, IntersectInfo& info, bool earlyexit)
{
  vec3d* vertsptr = verts.dataPtr();
  int* faceptr = faces.dataPtr();
  int numfaces = faces.size();
  vec3d* a;    // Vertices
  vec3d* b;
  vec3d* c;
  vec3d edge1, edge2, tvec, pvec, qvec;
  double det,inv_det;
  double u, v, t;

  info.hit = false;

  for(int i=0; i<numfaces; i++)
  {
    // Get the three triangle vertices...
    a = vertsptr+(*faceptr);
    faceptr++;
    b = vertsptr+(*faceptr);
    faceptr++;
    c = vertsptr+(*faceptr);
    faceptr++;

    // Ray-triangle intersection:

    // find vectors for two edges sharing vert a
    edge1.sub(*b,*a);
    edge2.sub(*c,*a);

    // begin calculating determinant - also used to calculate U parameter
    pvec.cross(direction, edge2);

    // if determinant is near zero, ray lies in plane of triangle
    det = edge1*pvec;

    if (det > -vec3d::epsilon && det < vec3d::epsilon)
      continue;  // no hit
    inv_det = 1.0 / det;

    // calculate distance from vert a to ray origin
    tvec.sub(origin, *a);

    // calculate U parameter and test bounds
    u = (tvec*pvec) * inv_det;
    if (u < 0.0 || u > 1.0)
      continue;  // no hit

    // prepare to test V parameter
    qvec.cross(tvec, edge1);

    // calculate V parameter and test bounds
    v = (direction*qvec) * inv_det;
    if (v < 0.0 || u + v > 1.0)
      continue;  // no hit

    // calculate t, ray intersects triangle */
    t = (edge2*qvec) * inv_det;
    if (t<=vec3d::epsilon)
      continue; // the hit would be behind the origin

    if (!info.hit || t<info.t)
    {
      info.t = t;
      info.u = u;
      info.v = v;
      info.faceindex = i;
      info.hit = true;
      if (earlyexit)
        break;
    }
  }
  return info.hit;
}


void TriMeshGeom::onVertsChanged(int start, int end)
{
  bb_cache_valid = false;
  mass_props_valid = false;
}

void TriMeshGeom::onVertsResize(int size)
{
  bb_cache_valid = false;
  mass_props_valid = false;
}

void TriMeshGeom::onFacesChanged(int start, int end)
{
  mass_props_valid = false;
}

void TriMeshGeom::onFacesResize(int size)
{
  mass_props_valid = false;
}

void TriMeshGeom::computeCog(vec3d& cog)
{
  if (!mass_props_valid)
    calcMassProperties();
  cog = _cog;
}

void TriMeshGeom::computeInertiaTensor(mat3d& tensor)
{
  if (!mass_props_valid)
    calcMassProperties();
  tensor = _inertiatensor;
}


}  // end of namespace
