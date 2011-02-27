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

#ifndef TRIMESH_H
#define TRIMESH_H

/** \file trimeshgeom.h
 Contains the TriMesh geometry class.
 */

#include "geomobject.h"
#include "slot.h"
#include "arrayslot.h"
#include "proceduralslot.h"
#include "vec3.h"
#include "boundingbox.h"

namespace support3d {

struct IntersectInfo
{
  IntersectInfo() : t(0), u(0), v(0), faceindex(0), hit(false) {}
  double t;
  double u;
  double v;
  int faceindex;
  bool hit;
};

/**
  TriMeshGeom geometry.
 */
class TriMeshGeom : public GeomObject
{
  public:
  NotificationForwarder<TriMeshGeom> _on_verts_event;
  NotificationForwarder<TriMeshGeom> _on_faces_event;

  /// The mesh vertices.
  ArraySlot<vec3d> verts;

  /** The mesh faces.

    Each face is an array of 3 vertex indices.
   */
  ArraySlot<int> faces;

  ProceduralSlot<vec3d, TriMeshGeom> cog;
  ProceduralSlot<mat3d, TriMeshGeom> inertiatensor;

  // Mass properties
  vec3d _cog;
  mat3d _inertiatensor;
  double _volume;

  /// A cache for the bounding box.
  BoundingBox bb_cache;

  /// Size constraint for uniform primitive variables.
  boost::shared_ptr<SizeConstraintBase> uniformSizeConstraint;
  /// Size constraint for varying or vertex primitive variables.
  boost::shared_ptr<SizeConstraintBase> varyingSizeConstraint;
  /// Size constraint for facevarying or facevertex primitive variables.
  boost::shared_ptr<SizeConstraintBase> faceVaryingSizeConstraint;

  /// True if the mass properties are still valid, otherwise they have to be recomputed.
  bool mass_props_valid;
  /// True if bb_cache is still valid, otherwise it has to be recomputed.
  bool bb_cache_valid;

  public:
  TriMeshGeom();

  virtual BoundingBox boundingBox();
  virtual void drawGL();

  /*  virtual int uniformCount() const { return faces.size(); }
  virtual int varyingCount() const { return verts.size(); }
  virtual int vertexCount() const { return verts.size(); }
  virtual int faceVaryingCount() const { return 3*faces.size(); }
  virtual int faceVertexCount() const { return 3*faces.size(); }*/
  virtual boost::shared_ptr<SizeConstraintBase> slotSizeConstraint(VarStorage storage) const;

  void calcMassProperties();
  bool intersectRay(const vec3d& origin, const vec3d& direction, IntersectInfo& info, bool earlyexit=false);

  void onVertsChanged(int start, int end);
  void onVertsResize(int size);
  void onFacesChanged(int start, int end);
  void onFacesResize(int size);

  void computeCog(vec3d& cog);
  void computeInertiaTensor(mat3d& tensor);
};


}  // end of namespace

#endif
