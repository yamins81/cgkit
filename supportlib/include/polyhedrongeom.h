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

#ifndef POLYHEDRON_H
#define POLYHEDRON_H

/** \file polyhedrongeom.h
 Contains the Polyhedron geometry class.
 */

#include <vector>
#include <list>
#include "geomobject.h"
#include "slot.h"
#include "arrayslot.h"
#include "proceduralslot.h"
#include "vec3.h"
#include "boundingbox.h"

#include "opengl.h"

namespace support3d {

/**
  This class can be used to allocate small data items of equal size.

  This class is for internal use in conjunction withe the OpenGL
  polygon tesselator. The class is used to allocate small data buffers
  that will store vertex information.

  The class allocates larger memory blocks that will be "sliced" into smaller
  buffers. The allocated memory is not freed until the object is destroyed,
  this means once enough memory was allocated, providing small data buffers
  should be fast.

  Usage: Before memory buffers are requested, the size of a buffer has to
  be set via setDataSize(). Then new buffers can be requested using
  newDataPtr(). If those pointers aren't required anymore, reset() has
  to be called so that the allocated memory can be reused.
 */
class DataMemoryManager
{
  private:
  int blocksize;
  int datasize;

  std::vector<void*> memptrs;
  std::vector<int> used;
  unsigned int currentblock;

  public:
  DataMemoryManager(int ablocksize);
  ~DataMemoryManager();

  void setDataSize(int size);
  void reset();
  void* newDataPtr();

  protected:
  void* alloc();
  void free();
};



/**
  PolyhedronGeom geometry.

  This is a simple polyhedron class that can just store and display
  polyhedrons made of general planar concave polygons.

 */
class PolyhedronGeom : public GeomObject
{
  public:
  typedef std::vector<int> VertexLoop;
  typedef std::vector<VertexLoop*> Poly;
  typedef std::vector<int>::iterator LoopIterator;

  public:
  NotificationForwarder<PolyhedronGeom> _on_verts_event;

  /// The polyhedron vertices.
  ArraySlot<vec3d> verts;

  /// The polys
  std::vector<Poly*> polys;


  //  ProceduralSlot<vec3d, PolyhedronGeom> cog;
  //  ProceduralSlot<mat3d, PolyhedronGeom> inertiatensor;

  // Mass properties
  //  vec3d _cog;
  //  mat3d _inertiatensor;
  //  double _volume;

  /// A cache for the bounding box.
  BoundingBox bb_cache;

  /// Size constraint for uniform primitive variables.
  boost::shared_ptr<SizeConstraintBase> uniformSizeConstraint;
  /// Size constraint for varying or vertex primitive variables.
  boost::shared_ptr<SizeConstraintBase> varyingSizeConstraint;
  /// Size constraint for facevarying or facevertex primitive variables.
  boost::shared_ptr<SizeConstraintBase> faceVaryingSizeConstraint;

  /// True if the mass properties are still valid, otherwise they have to be recomputed.
    //  bool mass_props_valid;
  /// True if bb_cache is still valid, otherwise it has to be recomputed.
  bool bb_cache_valid;

  private:
  /// Tesselator object (used by \em every PolyhedronGeom).
  static GLUtesselator* tess;
  static DataMemoryManager dataMemManager;

  public:
  PolyhedronGeom();
  virtual ~PolyhedronGeom();

  virtual BoundingBox boundingBox();
  virtual void drawGL();

  /*  virtual int uniformCount() const { return polys.size(); }
  virtual int varyingCount() const { return verts.size(); }
  virtual int vertexCount() const { return verts.size(); }*/
  int faceVaryingCount() const;
  /*  virtual int faceVertexCount() const { return faceVaryingCount(); }*/
  virtual boost::shared_ptr<SizeConstraintBase> slotSizeConstraint(VarStorage storage) const;

  //  void calcMassProperties();

  bool hasPolysWithHoles() const;

  int getNumPolys() const { return polys.size(); }
  int getNumLoops(int poly) const;
  int getNumVerts(int poly, int loop) const;
  void setNumPolys(int num);
  void setNumLoops(int poly, int num);
  // Get a copy of a loop
  std::vector<int> getLoop(int poly, int loop);
  // Set a copy of a loop
  void setLoop(int poly, int loop, const std::vector<int>& vloop);
  // Iterate over the vertex indices of one particular loop
  LoopIterator loopBegin(int poly, int loop);
  LoopIterator loopEnd(int poly, int loop);


  void onVertsChanged(int start, int end);
  void onVertsResize(int size);

  //  void computeCog(vec3d& cog);
  //  void computeInertiaTensor(mat3d& tensor);

  virtual void convert(GeomObject* target);

  private:
  void deletePoly(int poly);
  void computeNormal(int poly, vec3d& N);
};


}  // end of namespace

#endif
