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

#ifndef SPHEREGEOM_H
#define SPHEREGEOM_H

/** \file spheregeom.h
 Contains the sphere geometry class.
 */

#include "geomobject.h"
#include "proceduralslot.h"
#include "sor_triangulator.h"

// Define the CGKIT_SHARED variable
#ifdef DLL_EXPORT_SPHEREGEOM
  #include "shared_export.h"
#else
  #include "shared.h"
#endif

namespace support3d {

/**
  Sphere geometry.
 */
class CGKIT_SHARED SphereGeom : public GeomObject
{
  public:
  /// Sphere radius
  Slot<double> radius;
  /// Number of segments in u
  Slot<int> segmentsu;
  /// Number of segments in v
  Slot<int> segmentsv;

  Slot<vec3d> cog;
  ProceduralSlot<mat3d, SphereGeom> inertiatensor;

  public:
  SphereGeom(double aradius=1.0, int segsu=16, int segsv=8);
  ~SphereGeom();

  virtual BoundingBox boundingBox();
  virtual void drawGL();

  //  virtual int uniformCount() const { return 1; }
  //  virtual int varyingCount() const { return 4; }
  //  virtual int vertexCount() const { return 4; }

  virtual boost::shared_ptr<SizeConstraintBase> slotSizeConstraint(VarStorage storage) const;

  void computeInertiaTensor(mat3d& tensor);

  virtual void convert(GeomObject* target);

  private:
  void createSilhouette(SORTriangulator::SORVertexList& vlist);
};


}  // end of namespace

#endif
