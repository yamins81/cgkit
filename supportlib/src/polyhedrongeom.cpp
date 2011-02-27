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

#include "polyhedrongeom.h"
#include "common_exceptions.h"
#include "primvaraccess.h"
#include "trimeshgeom.h"
//#include "massproperties.h"

namespace support3d {

//////////////////////////////////////////////////////////////////////
// DataMemoryManager
//////////////////////////////////////////////////////////////////////

/**
   Constructor.
 
   \param ablocksize The size in bytes of the memory blocks that will be allocated
 */
DataMemoryManager::DataMemoryManager(int ablocksize)
  : blocksize(ablocksize),
    datasize(0),
    memptrs(),
    used(),
    currentblock(0)
{
}

DataMemoryManager::~DataMemoryManager()
{
  free();
}

/**
   Set the required item size in bytes.

   The size must not be bigger than the block size passed to the constructor!

   \param size Size of the required data item
 */
void DataMemoryManager::setDataSize(int size)
{
  datasize = size;
}

/**
   Reset the memory usage.

   This method has to be called when the previously returned data items
   aren't required anymore. Requesting new data items will then reuse
   the previously allocated memory.
 */
void DataMemoryManager::reset()
{
  // Reset the number of allocated bytes for each block
  std::vector<int>::iterator it = used.begin();
  std::vector<int>::iterator it_end = used.end();
  for( ; it!=it_end; it++)
  {
    (*it) = 0;
  }

  currentblock = 0;
}

/**
   Request a pointer to a data item.

   This method returns a memory buffer that can store as many bytes as
   was previously set by calling setDataSize().
 */
void* DataMemoryManager::newDataPtr()
{
  // No more free blocks available? then allocate a new one
  if (currentblock==memptrs.size())
  {
    alloc();
  }

  // No more free space in the current block? then allocate and use a new one
  if (blocksize-used[currentblock]<datasize)
  {
    alloc();
    currentblock++;
  }

  void* res = (char*)memptrs[currentblock]+used[currentblock];
  used[currentblock] += datasize;
  return res;
}

/**
   Allocate a new block of memory.

   The memory block is added the the memptrs vector. The corresponding
   "used" value is set to 0. A pointer to the newly created memory block
   is returned.

   \return A pointer to the allocated memory block.
 */
void* DataMemoryManager::alloc()
{
  void* ptr = malloc(blocksize);
  if (ptr==0)
    throw EMemoryError();
  memptrs.push_back(ptr);
  used.push_back(0);
  return ptr;
}

/**
   Free all allocated memory blocks.
 */
void DataMemoryManager::free()
{
  for(unsigned int i=0; i<memptrs.size(); i++)
  {
    ::free(memptrs[i]);
  }
  memptrs.resize(0);
  used.resize(0);
  currentblock = 0;
}

//////////////////////////////////////////////////////////////////////
// PolyTriangulation
//////////////////////////////////////////////////////////////////////

/**
  Stores the three vertex indices of a triangle.

  \see PolyTriangulation
 */
struct _PT_Face
{
  _PT_Face(int a, int b, int c) { verts[0]=a; verts[1]=b; verts[2]=c; }
  int verts[3];
};

/** 
  PolyTriangulation helper class.

  This class is used during GLU tesselation to create a TriMeshGeom out of
  a PolyhedronGeom. The class contains tesselation callbacks which build up
  all necessary information to create a TriMeshGeom at the end. All primitive
  variables are preserved.

  \see PolyhedronGeom
 */
class PolyTriangulation
{
  /// The polyhedron geom that is being triangulated
  PolyhedronGeom& polygeom;

  /// Total number of generated triangles.
  int tricount;

  /// Stores the faces (this will later be used to initialize the faces attribute).
  std::vector<_PT_Face> faces;
  /// Stores the number of triangles per polygon.
  std::vector<int> facecounts;
  /// Triangle counter (per poly).
  int currentfacecount;

  /// Stores the facevarying variable indices (each entry is one face).
  std::vector<_PT_Face> facevarindices;

  GLenum geomtype;
  int* v0;
  int* v1;


  public:
  PolyTriangulation(PolyhedronGeom& apolygeom) : polygeom(apolygeom), tricount(0), faces(), facecounts(), currentfacecount(0), facevarindices(), geomtype(0), v0(0), v1(0) {}

  /**
     Inform the class about the end of a polygon.

     This method has to be called after a polygon was processed so that
     the number of generated triangles can be stored.
   */
  void polyFinished()
  {
    // Store the number of triangles of the previously processed poly
    facecounts.push_back(currentfacecount);    
    currentfacecount = 0;
  }

  /**
     Tesselation "begin" callback.
   */
  void onTessBegin(GLenum type)
  {
    geomtype = type;
    v0 = 0;
    v1 = 0;
  }

  /**
     Tesselation "end" callback.
   */
  void onTessEnd()
  {
  }

  /**
     Tesselation "vertex" callback.

     data points to a buffer that contains two ints:
     - Vertex index
     - facevarying variable index
   */
  void onTessVertex(int* data)
  {
    // Store the first two vertices...
    if (v0==0)
    {
      v0 = data;
      return;
    }
    if (v1==0)
    {
      v1 = data;
      return;
    }

    // Here we have three vertices and can create a triangle...
    switch(geomtype)
    {
     // Individual triangles?
     case GL_TRIANGLES: 
       createTriangle(v0, v1, data);
       v0 = v1;
       v1 = data;
       return;

     // Triangle strip?
     case GL_TRIANGLE_STRIP: 
       if (tricount%2==0)
	 createTriangle(v0, v1, data);
       else
	 createTriangle(v1, v0, data);
       v0 = v1;
       v1 = data;
       return;

     // Triangle fan?
     case GL_TRIANGLE_FAN: 
       createTriangle(v0, v1, data);
       v1 = data;
       return;

     default: throw EValueError("Unknown triangle mode");
    };
  }

  /**
     Create a new triangle.

     The arguments are the data blocks of the three vertices. Each block
     looks as follows:

     - int: Vertex id
     - int: Facevarying variable id

     \param a Data block of the first vertex.
     \param b Data block of the second vertex.
     \param c Data block of the third vertex.
   */
  void createTriangle(int* a, int* b, int* c)
  {
    // Store the triangle (i.e. the three vertex indices)
    faces.push_back(_PT_Face(a[0], b[0], c[0]));
    facevarindices.push_back(_PT_Face(a[1], b[1], c[1]));
    tricount++;
    currentfacecount++;
  }

  /**
     Initialize a TriMesh geom object.

     This method has to be called after tesselation.

     \param tm A newly created TriMeshGeom that will receive the result of the triangulation.
   */
  void initTriMesh(TriMeshGeom& tm)
  {
    unsigned int i, j, k;

    // Copy the vertices from the polyhedron...
    tm.verts.resize(polygeom.verts.size());
    polygeom.verts.copyValues(0, polygeom.verts.size(), tm.verts, 0);

    // Create the faces...
    tm.faces.resize(faces.size());
    for(i=0; i<faces.size(); i++)
    {
      tm.faces.setValues(i, faces[i].verts);
    }
    
    // Copy primitive variables...
    GeomObject::VariableIterator it;
    for(it=polygeom.variablesBegin(); it!=polygeom.variablesEnd(); it++)
    {
      // Create the variable...
      tm.newVariable(it->first, 
		     it->second.storage, 
		     it->second.type, 
		     it->second.multiplicity,
		     it->second.slot->size());
      IArraySlot* tmslot = dynamic_cast<IArraySlot*>(&(tm.slot(it->first)));
      switch(it->second.storage)
      {
      case CONSTANT:
      case USER:
	// Copy the entire slot (both slots have the same length)
	it->second.slot->copyValues(0, tmslot->size(), *tmslot, 0);
	break;
      case UNIFORM:
	k = 0;
	for(i=0; i<facecounts.size(); i++)
	{
	  for(j=0; j<(unsigned int)(facecounts[i]); j++)
	  {
	    it->second.slot->copyValues(i, i+1, *tmslot, k);
	    k++;
	  }
        }
	break;
      case VARYING:
      case VERTEX:
	// Copy the entire slot
	it->second.slot->copyValues(0, tmslot->size(), *tmslot, 0);
	break;
      case FACEVARYING:
      case FACEVERTEX:
	k = 0;
	for(i=0; i<facevarindices.size(); i++)
	{
	  int* idxs = facevarindices[i].verts;
	  int idx = idxs[0];
	  it->second.slot->copyValues(idx, idx+1, *tmslot, k);
	  idx = idxs[1];
	  it->second.slot->copyValues(idx, idx+1, *tmslot, k+1);
	  idx = idxs[2];
	  it->second.slot->copyValues(idx, idx+1, *tmslot, k+2);
	  k+=3;
        }
	break;
      }
    }
  }

};


//////////////////////////////////////////////////////////////////////

#ifdef WIN32
#define CALLBACK __stdcall
#else
#define CALLBACK
#endif

// Tesselation callbacks...

#if (defined(__APPLE__) || defined(MACOSX)) && defined(__GNUC__)
typedef GLvoid (CALLBACK *TessCallback)();
#else
typedef GLvoid (CALLBACK *TessCallback)();
#endif

// Callbacks for triangulating...
// (the callbacks receive a pointer to a PolyTriangulation object on
// which they call the appropriate callback method)
GLvoid CALLBACK onTessBegin_triangulation(GLenum type, PolyTriangulation* polytri)
{
  polytri->onTessBegin(type);
}

GLvoid CALLBACK onTessEnd_triangulation(PolyTriangulation* polytri)
{
  polytri->onTessEnd();
}

GLvoid CALLBACK onTessVertex_triangulation(int* data, PolyTriangulation* polytri)
{
  polytri->onTessVertex(data);
}

// Callbacks for drawing...
int tess_data_flag = 0;

GLvoid CALLBACK onTessBegin(GLenum type)
{
  glBegin(type);
}

GLvoid CALLBACK onTessEnd()
{
  glEnd();
}

GLvoid CALLBACK onTessVertex(GLdouble* data)
{
  GLdouble* p = data+3;
  GLfloat glcol[4] = {0,0,0,1};
  // Is a normal available?
  if (tess_data_flag & 0x01)
  {
    glNormal3dv(p);
    p+=3;
  }
  // Is a texture coordinate available?
  if (tess_data_flag & 0x02)
  {
    glTexCoord2d(p[0], p[1]);
    p+=2;
  }
  // Is a color available?
  if (tess_data_flag & 0x04)
  {
    glcol[0] = GLfloat(p[0]);
    glcol[1] = GLfloat(p[1]);
    glcol[2] = GLfloat(p[2]);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, glcol);
  }
  // Pass the vertex
  glVertex3dv(data);
}



//////////////////////////////////////////////////////////////////////
// PolyhedronGeom
//////////////////////////////////////////////////////////////////////

GLUtesselator* PolyhedronGeom::tess = 0;
// Blocksize: One data item must be able to store:
// - A vertex position (3 floats)
// - A normal (3 floats)
// - A color (3 floats)
// - A texture coordinate (2 floats)
// -> 11 doubles / The block size is set to 10 items
DataMemoryManager PolyhedronGeom::dataMemManager(10*11*sizeof(GLdouble));

PolyhedronGeom::PolyhedronGeom()
: _on_verts_event(),
  verts(), polys(),
  //  cog(), inertiatensor(),
  //  _cog(), _inertiatensor(), _volume(),
  bb_cache(), 
  bb_cache_valid(true)
  
{
  _on_verts_event.init(this, &PolyhedronGeom::onVertsChanged, &PolyhedronGeom::onVertsResize);
  verts.addDependent(&_on_verts_event);

  // Create the constraint objects for the primitive variable slots...
  UserSizeConstraint* usc = new UserSizeConstraint();
  uniformSizeConstraint = boost::shared_ptr<SizeConstraintBase>(usc);

  LinearSizeConstraint* lsc = new LinearSizeConstraint(verts,1,0);
  varyingSizeConstraint = boost::shared_ptr<SizeConstraintBase>(lsc);

  usc = new UserSizeConstraint();
  faceVaryingSizeConstraint = boost::shared_ptr<SizeConstraintBase>(usc);

  //  cog.setProcedure(this, &PolyhedronGeom::computeCog);
  //  inertiatensor.setProcedure(this, &PolyhedronGeom::computeInertiaTensor);
  //  verts.addDependent(&cog);
  //  verts.addDependent(&inertiatensor);

  addSlot("verts", verts);
  //  addSlot("cog", cog);
  //  addSlot("inertiatensor", inertiatensor);
}

PolyhedronGeom::~PolyhedronGeom()
{
  // Deallocate polys
  setNumPolys(0);
}

/**
   Return the appropriate size constraint for a primitive variable.
 */
boost::shared_ptr<SizeConstraintBase> PolyhedronGeom::slotSizeConstraint(VarStorage storage) const
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


// Return bounding box
BoundingBox PolyhedronGeom::boundingBox()
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
  Draw the polyhedron.
 */
void PolyhedronGeom::drawGL()
{
  // No tesselation object allocated yet? Then do so once and for all...
  if (tess==0)
  {
    tess = gluNewTess();
    if (tess==0)
      return;
  }

  // Set flag to 0 (i.e. no variables are present so far)
  tess_data_flag = 0;
  PrimVarAccess<vec3d> normals(*this, std::string("N"), NORMAL, 1, std::string("Nfaces"));
  PrimVarAccess<double> texcoords(*this, std::string("st"), FLOAT, 2, std::string("stfaces"));
  PrimVarAccess<vec3d> colors(*this, std::string("Cs"), COLOR, 1, std::string("Csfaces"));
  vec3d* N;
  vec3d* Cs;
  GLfloat glcol[4] = {0,0,0,1};
  double* st;
  vec3d* vertsptr = verts.dataPtr();
  int i,j;
  int nfloats=3;

  // Check which variables has to be passed to the vertex callback
  // (this is the case when mode is > 2)
  if (normals.mode>2)
  {
    tess_data_flag |= 0x01;
    nfloats += 3;
  }
  if (texcoords.mode>2)
  {
    tess_data_flag |= 0x02;
    nfloats += 2;
  }
  if (colors.mode>2)
  {
    tess_data_flag |= 0x04;
    nfloats += 3;
  }

  dataMemManager.setDataSize(nfloats*sizeof(GLdouble));

  gluTessCallback(tess, GLU_TESS_BEGIN, (TessCallback)(&onTessBegin));
  gluTessCallback(tess, GLU_TESS_END, (TessCallback)(&onTessEnd));
  gluTessCallback(tess, GLU_TESS_VERTEX, (TessCallback)(&onTessVertex));
    
  gluTessProperty(tess, GLU_TESS_BOUNDARY_ONLY, GL_FALSE);
  gluTessProperty(tess, GLU_TESS_TOLERANCE, 0);
  gluTessProperty(tess, GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_ODD);

  // Iterate over all polygons...
  for(i=0; i<getNumPolys(); i++)
  {
    dataMemManager.reset();

    // No normals? Then a face normal has to be calculated...
    if (normals.mode==0)
    {
      vec3d Ng;
      computeNormal(i, Ng);
      glNormal3d(Ng.x, Ng.y, Ng.z);      
    }

    // Process uniform variables...
    if (normals.onFace(N))
      glNormal3d(N->x, N->y, N->z);
    if (texcoords.onFace(st))
      glTexCoord2dv(st);
    if (colors.onFace(Cs))
    {
      glcol[0] = GLfloat(Cs->x);
      glcol[1] = GLfloat(Cs->y);
      glcol[2] = GLfloat(Cs->z);
      glMaterialfv(GL_FRONT, GL_DIFFUSE, glcol);
    }

    gluTessBeginPolygon(tess, 0);
    // Iterate over all loops of polygon i...
    for(j=0; j<getNumLoops(i); j++)
    {
      gluTessBeginContour(tess);
      LoopIterator it = loopBegin(i, j);
      LoopIterator itend = loopEnd(i, j);
      vec3d* v;
      for( ; it!=itend; it++)
      {
	int vidx = (*it);
	v = vertsptr + vidx;
	GLdouble* data = (GLdouble*)dataMemManager.newDataPtr();
	GLdouble* p = data+3;
	data[0] = v->x;
	data[1] = v->y;
	data[2] = v->z;
	if (normals.onVertex(vidx, N))
	{
	  p[0] = N->x;
	  p[1] = N->y;
	  p[2] = N->z;
	  p += 3;
	}
	if (texcoords.onVertex(vidx, st))
	{
	  p[0] = st[0];
	  p[1] = st[1];
	  p += 2;
	}
	if (colors.onVertex(vidx, Cs))
	{
	  p[0] = Cs->x;
	  p[1] = Cs->y;
	  p[2] = Cs->z;
	}
	gluTessVertex(tess, data, data);
      }
      gluTessEndContour(tess);
    }
    gluTessEndPolygon(tess);
  }

  /*  for(i=0; i<getNumPolys(); i++)
  {
    for(j=0; j<getNumLoops(i); j++)
    {
      LoopIterator it = loopBegin(i, j);
      LoopIterator itend = loopEnd(i, j);
      vec3d* v;
      glBegin(GL_LINE_LOOP);
      for( ; it!=itend; it++)
      {
	v = vertsptr + (*it);
	glVertex3d(v->x, v->y, v->z);
      }
      glEnd();
    }
    }*/
}

/**
  Return the number of facvarying values.
 */
int PolyhedronGeom::faceVaryingCount() const
{
  int res = 0;
  unsigned int i,j;
  Poly* poly=0;

  // Iterate over the polygons
  for(i=0; i<polys.size(); i++)
  {
    poly = polys[i];
    // Iterate over the loops of polygon i
    for(j=0; j<poly->size(); j++)
    {
      // Add the number of vertices in loop j to the result
      res += (*poly)[j]->size();
    }
  }
  return res;
}

/**
  Return true if there is at least one polygon that has a hole.

  \return True if there is a polygon with a hole
 */
bool PolyhedronGeom::hasPolysWithHoles() const
{
  for(unsigned int i=0; i<polys.size(); i++)
  {
    if (polys[i]->size()>1)
      return true;
  }
  return false;
}


/**
  Return the number of loops of one particular polygon.

  An \c EIndexError exception is thrown if \a poly is out of range.
 */
int PolyhedronGeom::getNumLoops(int poly) const 
{ 
  if ((poly<0) || (poly>=getNumPolys()))
      throw EIndexError("Poly index out of range.");

  return polys[poly]->size(); 
}

/**
  Return the number of vertex ids in one particular loop.

  An \c EIndexError exception is thrown if \a poly or \a loop is out of range. 
 */
int PolyhedronGeom::getNumVerts(int poly, int loop) const
{
  if ((loop<0) || (loop>=getNumLoops(poly)))
      throw EIndexError("Loop index out of range.");

  return (*polys[poly])[loop]->size();
}

/**
  Set the number of polygons.

  Allocate new polygons or delete polygons (if the number is reduced).
  Setting the number to 0 deletes all polys.

  \param num New number of polys
 */
void PolyhedronGeom::setNumPolys(int num)
{ 
  if (num<0)
    num=0;

  int prevsize = polys.size();
  int i;

  // Delete polygons if the number of polys was decreased
  if (num<prevsize)
  {
    for(i=num; i<prevsize; i++)
    {
      deletePoly(i);
    }
  }

  // Resize poly list
  polys.resize(num);

  // Allocate new polys (which have 1 loop by default)...
  for(i=prevsize; i<num; i++)
  {
    polys[i] = new Poly();
    setNumLoops(i, 1);
  }

  // Update the size constraint for uniform and facevarying variables
  UserSizeConstraint* usc = dynamic_cast<UserSizeConstraint*>(uniformSizeConstraint.get());
  if (usc!=0)
    usc->setSize(num);
  usc = dynamic_cast<UserSizeConstraint*>(faceVaryingSizeConstraint.get());
  if (usc!=0)
    usc->setSize(faceVaryingCount());
}

/**
  Set the number of loops for one particular poly.

  An \c EIndexError exception is thrown if \a poly is out of range.
 */
void PolyhedronGeom::setNumLoops(int poly, int num) 
{ 
  if ((poly<0) || (poly>=getNumPolys()))
      throw EIndexError("Poly index out of range.");
  if (num<0)
    num=0;

  Poly* polygon = polys[poly];
  // Number of loops before the modification
  int prevsize = polygon->size();
  // Number of vertices removed (this number is used to adjust the constraint
  // size for facevarying variables)
  int lostverts = 0;
  int i;

  // Delete loops if the number of loops was decreased
  if (num<prevsize)
  {
    for(i=num; i<prevsize; i++)
    {
      // Add the number of verts in the loop
      lostverts += (*polygon)[i]->size();
      // Delete the loop
      delete (*polygon)[i];
    }
  }

  // Resize loop list
  polygon->resize(num);

  // Allocate new loops...
  // (the new loops have no verts, so lostverts doesn't have to be modified)
  for(i=prevsize; i<num; i++)
  {
    (*polygon)[i] = new VertexLoop();
  }

  // Update the size constraint for facevarying variables...
  // Todo: This probably shouldn't be done for every single modification
  // again and again...
  UserSizeConstraint* usc = dynamic_cast<UserSizeConstraint*>(faceVaryingSizeConstraint.get());
  //  std::cout<<"LOSTVERTS: "<<lostverts<<std::endl;
  if (usc!=0)
    usc->setSize(usc->getSize()-lostverts);
}

/**
   Return a copy of a vertex loop.

   An \c EIndexError exception is thrown if \a poly or \a loop is out 
   of range.
 */
std::vector<int> PolyhedronGeom::getLoop(int poly, int loop)
{
  if ((loop<0) || (loop>=getNumLoops(poly)))
      throw EIndexError("Loop index out of range.");

  return (*(*polys[poly])[loop]);
}

/**
   Set the vertex loop of a poly.

   An \c EIndexError exception is thrown if \a poly or \a loop is out 
   of range.
 */
void PolyhedronGeom::setLoop(int poly, int loop, const std::vector<int>& vloop)
{
  if ((loop<0) || (loop>=getNumLoops(poly)))
      throw EIndexError("Loop index out of range.");

  // Determine the adjustment for the contraint size for facevarying slots...
  int constraintdelta = ((*polys[poly])[loop])->size();
  constraintdelta = -constraintdelta + vloop.size();

  *((*polys[poly])[loop]) = vloop;

  // Update the size constraint for facevarying variables...
  // Todo: This probably shouldn't be done for every single modification
  // again and again...
  UserSizeConstraint* usc = dynamic_cast<UserSizeConstraint*>(faceVaryingSizeConstraint.get());
  //  std::cout<<"CONSTRAINTDELTA: "<<constraintdelta<<std::endl;
  if (usc!=0)
    usc->setSize(usc->getSize()+constraintdelta);
}

PolyhedronGeom::LoopIterator PolyhedronGeom::loopBegin(int poly, int loop)
{ 
  if ((loop<0) || (loop>=getNumLoops(poly)))
      throw EIndexError("Loop index out of range.");

  return (*polys[poly])[loop]->begin(); 
}

PolyhedronGeom::LoopIterator PolyhedronGeom::loopEnd(int poly, int loop)
{ 
  if ((loop<0) || (loop>=getNumLoops(poly)))
      throw EIndexError("Loop index out of range.");

  return (*polys[poly])[loop]->end(); 
}


void PolyhedronGeom::onVertsChanged(int start, int end)
{ 
  bb_cache_valid = false;
}

void PolyhedronGeom::onVertsResize(int size)
{ 
  bb_cache_valid = false;
}


/*void PolyhedronGeom::computeCog(vec3d& cog)
{
  if (!mass_props_valid)
    calcMassProperties();
  cog = _cog;
}

void PolyhedronGeom::computeInertiaTensor(mat3d& tensor)
{
  if (!mass_props_valid)
    calcMassProperties();
  tensor = _inertiatensor;
}*/



/**
  Deallocate a polygon together with its loops.
 */
void PolyhedronGeom::deletePoly(int poly)
{
  unsigned int i;
  Poly* p = polys[poly];
  // Delete loops
  for(i=0; i<p->size(); i++)
  {
    delete (*p)[i];
  }
  // Delete poly
  delete p;
  polys[poly] = 0;
}

/**
  Compute the normal for a polygon.

  Basically, the method simply takes the first three vertices A, B, C and 
  computes the normal of that triangle.
  However, it is checked that A!=B and B!=C, so it is possible that more than
  three vertices are looked up.

  \param poly Polygon index
  \param[out] N Receives the resulting normal
 */
void PolyhedronGeom::computeNormal(int poly, vec3d& N)
{
  VertexLoop& loop = *(*polys[poly])[0];
  int size = loop.size();
  int i = 2;

  if (size<3)
    return;

  const vec3d* a = &(verts.getValue(loop[0]));
  const vec3d* b = &(verts.getValue(loop[1]));
  while(a==b)
  {
    if (i>=size)
      return;
    b = &(verts.getValue(loop[i]));
    i++;
  }
  const vec3d* c = &(verts.getValue(loop[i]));
  while(b==c)
  {
    if (i>=size)
      return;
    c = &(verts.getValue(loop[i]));
    i++;
  }

  N.cross((*b)-(*a), (*c)-(*a));
  try
  {
    N.normalize(N);
  }
  catch(...)
  {
    N.set(0,0,0);
  } 
}


/**
  Convert to TriMesh
 */
void PolyhedronGeom::convert(GeomObject* target)
{
  TriMeshGeom* tm = dynamic_cast<TriMeshGeom*>(target);
  vec3d* vertsptr = verts.dataPtr();
  // Index of the corresponding facevarying (or facevertex) variable
  int facevarindex = 0;
  int i, j;

  // Check if the target geom is really a TriMesh
  if (tm==0)
  {
    throw ENotImplementedError("Conversion not supported by the PolyhedronGeom");
  }

  // No tesselation object allocated yet? Then do so once and for all...
  if (tess==0)
  {
    tess = gluNewTess();
    if (tess==0)
      return;
  }

  PolyTriangulation polyTriangulation(*this);

  // Remove any existing variable in the trimesh...
  tm->deleteAllVariables();

  dataMemManager.setDataSize(3*sizeof(GLdouble)+2*sizeof(int));

  gluTessCallback(tess, GLU_TESS_BEGIN_DATA, (TessCallback)(&onTessBegin_triangulation));
  gluTessCallback(tess, GLU_TESS_END_DATA, (TessCallback)(&onTessEnd_triangulation));
  gluTessCallback(tess, GLU_TESS_VERTEX_DATA, (TessCallback)(&onTessVertex_triangulation));
    
  gluTessProperty(tess, GLU_TESS_BOUNDARY_ONLY, GL_FALSE);
  gluTessProperty(tess, GLU_TESS_TOLERANCE, 0);
  gluTessProperty(tess, GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_ODD);

  // Iterate over all polygons...
  for(i=0; i<getNumPolys(); i++)
  {
    dataMemManager.reset();

    gluTessBeginPolygon(tess, &polyTriangulation);
    // Iterate over all loops of polygon i...
    for(j=0; j<getNumLoops(i); j++)
    {
      gluTessBeginContour(tess);
      LoopIterator it = loopBegin(i, j);
      LoopIterator itend = loopEnd(i, j);
      vec3d* v;
      for( ; it!=itend; it++)
      {
	int vidx = (*it);
	v = vertsptr + vidx;
	GLdouble* loc = (GLdouble*)dataMemManager.newDataPtr();
	loc[0] = v->x;
	loc[1] = v->y;
	loc[2] = v->z;
	int* data = (int*)(loc+3);
	data[0] = vidx;  // Vertex index
	data[1] = facevarindex;  // facevarying variable index
	gluTessVertex(tess, loc, data);
	facevarindex++;
      }
      gluTessEndContour(tess);
    }
    gluTessEndPolygon(tess);
    polyTriangulation.polyFinished();
  }

  polyTriangulation.initTriMesh(*tm);

}


}  // end of namespace
