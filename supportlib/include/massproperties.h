/*
  mass properties

  Original C version by:

  Brian Mirtich, "Fast and Accurate Computation of
  Polyhedral Mass Properties," journal of graphics
  tools, volume 1, number 2, 1996.

  http://www.acm.org/jgt/papers/Mirtich96/

 */

/*
  TODO: - Normal calculation for polygons with more than 3 vertices
        - Error checking for degenerate faces
        - Error checking for zero volume (DivisionByZero)
 */

#ifndef VOLINT_H
#define VOLINT_H

#define MAX_POLYGON_SZ 10 /* maximum number of verts per polygonal face */

namespace support3d {

/**
  Face structure (helper class for determining mass properties).

  This structure describes one individual polygon of the B-Rep.
  TODO: Fix normal calculation for polygons with more than 3 vertices.

  \see FaceEnum
*/
class FACE 
{
  public:
  /// Number of vertices.
  int    numVerts;
  /// Vertices in counter-clockwise order (right-handed system).
  double verts[MAX_POLYGON_SZ][3];
  /// Normal (is computed by init()).
  double norm[3];
  /// Plane equation offset (is computed by init()).
  double w;

  public:
  FACE() {}
  
  bool init();
  void setVert(int n, double x, double y, double z);
};

/**
  FaceEnum

  Enumerate faces.
  There is only one method next() which has to fill a FACE 
  structure (numVerts and verts) and call the init() method
  of the face which computes the normal and the offset w.
  The return value of next() indicates if the face is valid (true)
  or if there were no more faces left (false).
  The vertices must be set in a counter-clockwise order (when using
  a right-handed system).

  \see FACE
*/
class FaceEnum
{
  public:
  FaceEnum() {}
  virtual ~FaceEnum() {}
  virtual bool next(FACE& face) = 0;
};


/**
  Calculates the mass properties of a polyhedron.

  Usage:

  Before computing the mass properties you have to set either the
  (constant) density or the total mass of the body (using setDensity()
  or setMass()). 
  You can compute the mass properties in two ways:

  - By calling compute() which takes an enumerator object that enumerates
    all faces of the mesh.
  - By calling meshBegin(), face(), meshEnd() and providing each face
    to the face() method.

  Either way, the result is stored in the attributes density, mass, 
  volume, r (center of mass), and J (inertia tensor).

  \see FACE, FaceEnum
*/
class MassProperties
{
  public:
  double density;
  double mass;
  double volume;
  double r[3];     // center of mass
  double J[3][3];  // inertia tensor 

  MassProperties();

  // Input parameters
  void setDensity(double d);
  void setMass(double m);

  // 1st possibility
  void compute(FaceEnum& faceenum);

  // 2nd possibility
  void meshBegin();
  void face(const FACE& f);
  void meshEnd();

  //////////////////////////////////////////////////////////////////////
  protected:

  /// True, if density is given by user otherwise the mass is given.
  bool density_flag;

  int A;   /* alpha */
  int B;   /* beta */
  int C;   /* gamma */

  /* projection integrals */
  double P1, Pa, Pb, Paa, Pab, Pbb, Paaa, Paab, Pabb, Pbbb;

  /* face integrals */
  double Fa, Fb, Fc, Faa, Fbb, Fcc, Faaa, Fbbb, Fccc, Faab, Fbbc, Fcca;

  /* volume integrals */
  double T0, T1[3], T2[3], TP[3];

  void compVolumeIntegrals(FaceEnum& faceenum);
  void compFaceIntegrals(const FACE *f);
  void compProjectionIntegrals(const FACE *f);  

};

}  // end of namespace

#endif

