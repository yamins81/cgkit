/*
  mass properties

  Original C version by:

  Brian Mirtich, "Fast and Accurate Computation of
  Polyhedral Mass Properties," journal of graphics
  tools, volume 1, number 2, 1996.

  http://www.acm.org/jgt/papers/Mirtich96/

 */

	/*******************************************************
  *                                                      *
	*  volInt.c                                            *
	*                                                      *
	*  This code computes volume integrals needed for      *
	*  determining mass properties of polyhedral bodies.   *
	*                                                      *
	*  For more information, see the accompanying README   *
	*  file, and the paper                                 *
	*                                                      *
	*  Brian Mirtich, "Fast and Accurate Computation of    *
	*  Polyhedral Mass Properties," journal of graphics    *
	*  tools, volume 1, number 1, 1996.                    *
	*                                                      *
	*  This source code is public domain, and may be used  *
	*  in any way, shape or form, free of charge.          *
	*                                                      *
	*  Copyright 1995 by Brian Mirtich                     *
	*                                                      *
	*  mirtich@cs.berkeley.edu                             *
	*  http://www.cs.berkeley.edu/~mirtich                 *
  *                                                      *
	*******************************************************/

/*
	Revision history

	26 Jan 1996	Program creation.

	 3 Aug 1996	Corrected bug arising when polyhedron density
			is not 1.0.  Changes confined to function main().
			Thanks to Zoran Popovic for catching this one.

	27 May 1997     Corrected sign error in translation of inertia
	                product terms to center of mass frame.  Changes 
			            confined to function main().  Thanks to 
			            Chris Hecker.
*/


#include <stdio.h>
#include <string.h>
#include <math.h>
#include "massproperties.h"

/*
   ============================================================================
   constants
   ============================================================================
*/

#define X 0
#define Y 1
#define Z 2

namespace support3d {

// Helper functions  

template<class T>
T SQR(T x) { return x*x; }

template<class T>
T CUBE(T x) { return x*x*x; }

// Originally the above have been the following macros:
//#define SQR(x) ((x)*(x))
//#define CUBE(x) ((x)*(x)*(x))


/*
   ============================================================================
   compute mass properties
   ============================================================================
*/

MassProperties::MassProperties()
  : density(1.0), mass(0.0), volume(0.0), density_flag(true)
{

}

/**
  Set the constant density for the entire body.

  You can either call this method or the setMass() method
  to determine density/mass.

  \param d Density
  \see setMass
*/
void MassProperties::setDensity(double d)
{
  density = d;
  density_flag = true;
}

/**
  Set the total mass for the entire body.

  You can either call this method or the setDensity() method
  to determine density/mass.

  \param m Mass
  \see setDensity
*/
void MassProperties::setMass(double m)
{
  mass = m;
  density_flag = false;
}

/**
  Computes the mass properties.

  The mesh faces are traversed by the face enumerator.

  \param faceenum  Face enumerator.
 */
void MassProperties::compute(FaceEnum& faceenum)
{
  meshBegin();
  compVolumeIntegrals(faceenum);
  meshEnd();
}

/**
   Start computing the mass properties.

   This method has be called before you start passing faces
   using the face() method.

   \see meshEnd(), face()
 */
void MassProperties::meshBegin()
{
  T0 = T1[X] = T1[Y] = T1[Z] 
     = T2[X] = T2[Y] = T2[Z] 
     = TP[X] = TP[Y] = TP[Z] = 0;
}

/**
  Pass the next mesh face to the computation.

  \param f Initialized face.
  \see meshBegin(), meshEnd()
 */
void MassProperties::face(const FACE& f)
{
  double nx, ny, nz;

  nx = fabs(f.norm[X]);
  ny = fabs(f.norm[Y]);
  nz = fabs(f.norm[Z]);
  if (nx > ny && nx > nz) C = X;
  else C = (ny > nz) ? Y : Z;
  A = (C + 1) % 3;
  B = (A + 1) % 3;

  compFaceIntegrals(&f);

  T0 += f.norm[X] * ((A == X) ? Fa : ((B == X) ? Fb : Fc));
  
  T1[A] += f.norm[A] * Faa;
  T1[B] += f.norm[B] * Fbb;
  T1[C] += f.norm[C] * Fcc;
  T2[A] += f.norm[A] * Faaa;
  T2[B] += f.norm[B] * Fbbb;
  T2[C] += f.norm[C] * Fccc;
  TP[A] += f.norm[A] * Faab;
  TP[B] += f.norm[B] * Fbbc;
  TP[C] += f.norm[C] * Fcca;
}

/**
  Terminate the mass property calculation.

  You have to call this method after you've passed all mesh faces
  to the computation. The result of the computation is then stored
  in the public properties \a volume, \a r (center of mass), 
  \a J (inertia tensor), \a density and \a mass.
 */
void MassProperties::meshEnd()
{
  T1[X] /= 2; T1[Y] /= 2; T1[Z] /= 2;
  T2[X] /= 3; T2[Y] /= 3; T2[Z] /= 3;
  TP[X] /= 2; TP[Y] /= 2; TP[Z] /= 2;

  volume = T0;

  if (fabs(T0)<1E-20)
  {
    r[X] = r[Y] = r[Z] = 0.0;
    J[X][X] = J[X][Y] = J[X][Z] = 0.0;
    J[Y][X] = J[Y][Y] = J[Y][Z] = 0.0;
    J[Z][X] = J[Z][Y] = J[Z][Z] = 0.0;
    mass = 0.0;
    density = 0.0;
    return;
  }

  if (density_flag)
  {
    mass = density * volume;
  }
  else
  {
    density = mass/volume;
  }

  /* compute center of mass */
  r[X] = T1[X] / T0;
  r[Y] = T1[Y] / T0;
  r[Z] = T1[Z] / T0;

  /* compute inertia tensor */
  J[X][X] = density * (T2[Y] + T2[Z]);
  J[Y][Y] = density * (T2[Z] + T2[X]);
  J[Z][Z] = density * (T2[X] + T2[Y]);
  J[X][Y] = J[Y][X] = - density * TP[X];
  J[Y][Z] = J[Z][Y] = - density * TP[Y];
  J[Z][X] = J[X][Z] = - density * TP[Z];

  /* translate inertia tensor to center of mass */
/*  J[X][X] -= mass * (r[Y]*r[Y] + r[Z]*r[Z]);
  J[Y][Y] -= mass * (r[Z]*r[Z] + r[X]*r[X]);
  J[Z][Z] -= mass * (r[X]*r[X] + r[Y]*r[Y]);
  J[X][Y] = J[Y][X] += mass * r[X] * r[Y];
  J[Y][Z] = J[Z][Y] += mass * r[Y] * r[Z];
  J[Z][X] = J[X][Z] += mass * r[Z] * r[X];*/
}

/*
  modified version of the original
  (could now also be used for several mesh parts)

  meshBegin() has to be called previously.
  At the end you can call meshEnd() and the result is ready.
 */
void MassProperties::compVolumeIntegrals(FaceEnum& faceenum)
{
  FACE f;
  double nx, ny, nz;

  while(faceenum.next(f))
  {
    nx = fabs(f.norm[X]);
    ny = fabs(f.norm[Y]);
    nz = fabs(f.norm[Z]);
    if (nx > ny && nx > nz) C = X;
    else C = (ny > nz) ? Y : Z;
    A = (C + 1) % 3;
    B = (A + 1) % 3;

    compFaceIntegrals(&f);

    T0 += f.norm[X] * ((A == X) ? Fa : ((B == X) ? Fb : Fc));

    T1[A] += f.norm[A] * Faa;
    T1[B] += f.norm[B] * Fbb;
    T1[C] += f.norm[C] * Fcc;
    T2[A] += f.norm[A] * Faaa;
    T2[B] += f.norm[B] * Fbbb;
    T2[C] += f.norm[C] * Fccc;
    TP[A] += f.norm[A] * Faab;
    TP[B] += f.norm[B] * Fbbc;
    TP[C] += f.norm[C] * Fcca;
  }
}

void MassProperties::compFaceIntegrals(const FACE *f)
{
  const double *n;
  double w;
  double k1, k2, k3, k4;

  compProjectionIntegrals(f);

  w = f->w;
  n = f->norm;
  k1 = 1 / n[C]; k2 = k1 * k1; k3 = k2 * k1; k4 = k3 * k1;

  Fa = k1 * Pa;
  Fb = k1 * Pb;
  Fc = -k2 * (n[A]*Pa + n[B]*Pb + w*P1);

  Faa = k1 * Paa;
  Fbb = k1 * Pbb;
  Fcc = k3 * (SQR(n[A])*Paa + 2*n[A]*n[B]*Pab + SQR(n[B])*Pbb
	 + w*(2*(n[A]*Pa + n[B]*Pb) + w*P1));

  Faaa = k1 * Paaa;
  Fbbb = k1 * Pbbb;
  Fccc = -k4 * (CUBE(n[A])*Paaa + 3*SQR(n[A])*n[B]*Paab 
	   + 3*n[A]*SQR(n[B])*Pabb + CUBE(n[B])*Pbbb
	   + 3*w*(SQR(n[A])*Paa + 2*n[A]*n[B]*Pab + SQR(n[B])*Pbb)
	   + w*w*(3*(n[A]*Pa + n[B]*Pb) + w*P1));

  Faab = k1 * Paab;
  Fbbc = -k2 * (n[A]*Pabb + n[B]*Pbbb + w*Pbb);
  Fcca = k3 * (SQR(n[A])*Paaa + 2*n[A]*n[B]*Paab + SQR(n[B])*Pabb
	 + w*(2*(n[A]*Paa + n[B]*Pab) + w*Pa));
}

/* compute various integrations over projection of face */
void MassProperties::compProjectionIntegrals(const FACE *f)
{
  double a0, a1, da;
  double b0, b1, db;
  double a0_2, a0_3, a0_4, b0_2, b0_3, b0_4;
  double a1_2, a1_3, b1_2, b1_3;
  double C1, Ca, Caa, Caaa, Cb, Cbb, Cbbb;
  double Cab, Kab, Caab, Kaab, Cabb, Kabb;
  int i;

  P1 = Pa = Pb = Paa = Pab = Pbb = Paaa = Paab = Pabb = Pbbb = 0.0;

  for (i = 0; i < f->numVerts; i++) {
    a0 = f->verts[i][A];
    b0 = f->verts[i][B];
    a1 = f->verts[(i+1) % f->numVerts][A];
    b1 = f->verts[(i+1) % f->numVerts][B];
    da = a1 - a0;
    db = b1 - b0;
    a0_2 = a0 * a0; a0_3 = a0_2 * a0; a0_4 = a0_3 * a0;
    b0_2 = b0 * b0; b0_3 = b0_2 * b0; b0_4 = b0_3 * b0;
    a1_2 = a1 * a1; a1_3 = a1_2 * a1; 
    b1_2 = b1 * b1; b1_3 = b1_2 * b1;

    C1 = a1 + a0;
    Ca = a1*C1 + a0_2; Caa = a1*Ca + a0_3; Caaa = a1*Caa + a0_4;
    Cb = b1*(b1 + b0) + b0_2; Cbb = b1*Cb + b0_3; Cbbb = b1*Cbb + b0_4;
    Cab = 3*a1_2 + 2*a1*a0 + a0_2; Kab = a1_2 + 2*a1*a0 + 3*a0_2;
    Caab = a0*Cab + 4*a1_3; Kaab = a1*Kab + 4*a0_3;
    Cabb = 4*b1_3 + 3*b1_2*b0 + 2*b1*b0_2 + b0_3;
    Kabb = b1_3 + 2*b1_2*b0 + 3*b1*b0_2 + 4*b0_3;

    P1 += db*C1;
    Pa += db*Ca;
    Paa += db*Caa;
    Paaa += db*Caaa;
    Pb += da*Cb;
    Pbb += da*Cbb;
    Pbbb += da*Cbbb;
    Pab += db*(b1*Cab + b0*Kab);
    Paab += db*(b1*Caab + b0*Kaab);
    Pabb += da*(a1*Cabb + a0*Kabb);
  }

  P1 /= 2.0;
  Pa /= 6.0;
  Paa /= 12.0;
  Paaa /= 20.0;
  Pb /= -6.0;
  Pbb /= -12.0;
  Pbbb /= -20.0;
  Pab /= 24.0;
  Paab /= 60.0;
  Pabb /= -60.0;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

bool FACE::init()
{
  double dx1, dy1, dz1, dx2, dy2, dz2, nx, ny, nz, len;

  /* compute face normal and offset w from first 3 vertices */
  dx1 = verts[1][X] - verts[0][X];
  dy1 = verts[1][Y] - verts[0][Y];
  dz1 = verts[1][Z] - verts[0][Z];
  dx2 = verts[2][X] - verts[1][X];
  dy2 = verts[2][Y] - verts[1][Y];
  dz2 = verts[2][Z] - verts[1][Z];
  nx = dy1 * dz2 - dy2 * dz1;
  ny = dz1 * dx2 - dz2 * dx1;
  nz = dx1 * dy2 - dx2 * dy1;
  len = sqrt(nx * nx + ny * ny + nz * nz);
  if (len<=1E-12)
  {
    // ...degenerate face (triangle)...
    // (could be absolutely ok for polygons with more than 3 vertices!)
    return false;
  }
  norm[X] = nx / len;
  norm[Y] = ny / len;
  norm[Z] = nz / len;
  w = - norm[X] * verts[0][X] - norm[Y] * verts[0][Y] - norm[Z] * verts[0][Z];
  return true;
}

void FACE::setVert(int n, double x, double y, double z)
{
  verts[n][0] = x;
  verts[n][1] = y;
  verts[n][2] = z;
}

} // end of namespace
