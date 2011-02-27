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

/*
 Noise

 The basic noise implementation is a Perlin noise as described in
 Texturing & Modeling.
*/

//#include <stdio.h>
//#include <stdlib.h>
#include <math.h>
#include "vec3.h"
#include "common_exceptions.h"

namespace support3d {

// Table mask which is used to create indexing values
// (must be length of the tables - 1)
#define TABMASK 0xff

#include "noisetabs.h"

// Period values for the pnoise() function family
// (this should actually be parameters to the pnoise() functions, but
// then the tabindex templates wouldn't work anymore)
static int xperiod = 1;
static int yperiod = 1;
static int zperiod = 1;
static int tperiod = 1;
static int poffset = 0;

/*----------------------------------------------------------------------
  Modulo function.
  Returns a mod b.
----------------------------------------------------------------------*/
inline int imod(int a, int b)
{
  int n = (int)(a/b);
  a-=n*b;
  if (a<0)
    a+=b;
  return a;
}

/*----------------------------------------------------------------------
  Tabindex functions.

  These functions create a "random" index between 0 and 255 which is 
  dependent only on up to 4 integer input values.
----------------------------------------------------------------------*/

inline unsigned char tabindex2(int ix, int iy)
{
  return perm[(ix + perm[iy&TABMASK])&TABMASK];
}

inline unsigned char tabindex3(int ix, int iy, int iz)
{
  return perm[(ix + perm[(iy + perm[iz&TABMASK])&TABMASK])&TABMASK];
}

inline unsigned char tabindex4(int ix, int iy, int iz, int it)
{
  return perm[(it + perm[(ix + perm[(iy + perm[iz&TABMASK])&TABMASK])&TABMASK])&TABMASK];
}

/*----------------------------------------------------------------------
  Periodic tabindex functions.

  These functions create a "random" index between 0 and 255 which is 
  dependent only on up to 4 integer input values.
  The return value is periodic with a period stored in the global
  variables xperiod, yperiod, zperiod and tperiod.
  These period variables have to be set before calling the functions.
----------------------------------------------------------------------*/

inline unsigned char ptabindex2(int ix, int iy)
{
  ix=imod(ix,xperiod);
  iy=imod(iy,yperiod);
  return perm[(ix + perm[iy&TABMASK])&TABMASK];
}

inline unsigned char ptabindex3(int ix, int iy, int iz)
{
  ix=imod(ix,xperiod);
  iy=imod(iy,yperiod);
  iz=imod(iz,zperiod);
  return perm[(ix + perm[(iy + perm[iz&TABMASK])&TABMASK])&TABMASK];
}

inline unsigned char ptabindex4(int ix, int iy, int iz, int it)
{
  ix=imod(ix,xperiod);
  iy=imod(iy,yperiod);
  iz=imod(iz,zperiod);
  it=imod(it,tperiod);
  return perm[(it + perm[(ix + perm[(iy + perm[iz&TABMASK])&TABMASK])&TABMASK])&TABMASK];
}

/*----------------------------------------------------------------------
  Periodic tabindex functions with offset.

  Same as before, but they use an additional offset (global variable
  poffset) that is used in the calculation.
  These versions are used for the vector version of the pnoise()
  functions.
----------------------------------------------------------------------*/

inline unsigned char ptabindex2offset(int ix, int iy)
{
  ix=imod(ix,xperiod);
  iy=imod(iy,yperiod)+poffset;
  return perm[(ix + perm[iy&TABMASK])&TABMASK];
}

inline unsigned char ptabindex3offset(int ix, int iy, int iz)
{
  ix=imod(ix,xperiod);
  iy=imod(iy,yperiod);
  iz=imod(iz,zperiod)+poffset;
  return perm[(ix + perm[(iy + perm[iz&TABMASK])&TABMASK])&TABMASK];
}

inline unsigned char ptabindex4offset(int ix, int iy, int iz, int it)
{
  ix=imod(ix,xperiod);
  iy=imod(iy,yperiod);
  iz=imod(iz,zperiod)+poffset;
  it=imod(it,tperiod);
  return perm[(it + perm[(ix + perm[(iy + perm[iz&TABMASK])&TABMASK])&TABMASK])&TABMASK];
}

/*----------------------------------------------------------------------
 lerp - Linear interpolation between a and b
 ----------------------------------------------------------------------*/
inline double lerp(double t, double a, double b)
{
  return a+(b-a)*t;
}


/**
  noise template (2D)

  Noise template for a 2D noise or pnoise function.
  The template parameter specifies the function that's used to create
  an index for the random values. If this function is periodic you'll
  also get a periodic noise.
  Returns a value between -1 and 1 
*/
template<class T_IndexFunc>
double noise_template(const T_IndexFunc idxfunc, double x, double y)
{
  // Determine cell
  int ix = int(floor(x));
  int iy = int(floor(y));

  // Coordinate components relative to the cell vertices
  double rx0 = x-ix;
  double ry0 = y-iy;
  double rx1 = rx0-1.0;
  double ry1 = ry0-1.0;

  double sx = rx0*rx0*(3.0-2.0*rx0);
  double sy = ry0*ry0*(3.0-2.0*ry0);

  double *g;
  double u,v;
  double a,b;

  g = grads2[idxfunc(ix,iy)];
  u = g[0]*rx0 + g[1]*ry0;
  g = grads2[idxfunc(ix+1,iy)];
  v = g[0]*rx1 + g[1]*ry0;
  a = lerp(sx,u,v);

  g = grads2[idxfunc(ix,iy+1)];
  u = g[0]*rx0 + g[1]*ry1;
  g = grads2[idxfunc(ix+1,iy+1)];
  v = g[0]*rx1 + g[1]*ry1;
  b = lerp(sx,u,v);

  return lerp(sy,a,b);
};

/*---------------------------------------------------------------------- 
  noise template (3D)

  Noise template for a 3D noise or pnoise function.
  The template parameter specifies the function that's used to create
  an index for the random values. If this function is periodic you'll
  also get a periodic noise.
  Returns a value between -1 and 1 
----------------------------------------------------------------------*/
template<class T_IndexFunc>
double noise_template(const T_IndexFunc idxfunc, double x, double y, double z)
{
  // Determine cell
  int ix = int(floor(x));
  int iy = int(floor(y));
  int iz = int(floor(z));

  // Coordinate components relative to the cell vertices
  double rx0 = x-ix;
  double ry0 = y-iy;
  double rz0 = z-iz;
  double rx1 = rx0-1.0;
  double ry1 = ry0-1.0;
  double rz1 = rz0-1.0;

  double sx = rx0*rx0*(3.0-2.0*rx0);
  double sy = ry0*ry0*(3.0-2.0*ry0);
  double sz = rz0*rz0*(3.0-2.0*rz0);

  double *g;
  double u,v;
  double a,b,c,d;

  g = grads3[idxfunc(ix,iy,iz)];
  u = g[0]*rx0 + g[1]*ry0 + g[2]*rz0;
  g = grads3[idxfunc(ix+1,iy,iz)];
  v = g[0]*rx1 + g[1]*ry0 + g[2]*rz0;
  a = lerp(sx,u,v);

  g = grads3[idxfunc(ix,iy+1,iz)];
  u = g[0]*rx0 + g[1]*ry1 + g[2]*rz0;
  g = grads3[idxfunc(ix+1,iy+1,iz)];
  v = g[0]*rx1 + g[1]*ry1 + g[2]*rz0;
  b = lerp(sx,u,v);

  c = lerp(sy,a,b);

  g = grads3[idxfunc(ix,iy,iz+1)];
  u = g[0]*rx0 + g[1]*ry0 + g[2]*rz1;
  g = grads3[idxfunc(ix+1,iy,iz+1)];
  v = g[0]*rx1 + g[1]*ry0 + g[2]*rz1;
  a = lerp(sx,u,v);

  g = grads3[idxfunc(ix,iy+1,iz+1)];
  u = g[0]*rx0 + g[1]*ry1 + g[2]*rz1;
  g = grads3[idxfunc(ix+1,iy+1,iz+1)];
  v = g[0]*rx1 + g[1]*ry1 + g[2]*rz1;
  b = lerp(sx,u,v);

  d = lerp(sy,a,b);
  
  return lerp(sz,c,d);
};

/*----------------------------------------------------------------------
  noise template (4D)

  Noise template for a 4D noise or pnoise function.
  The template parameter specifies the function that's used to create
  an index for the random values. If this function is periodic you'll
  also get a periodic noise.
  Returns a value between -1 and 1 
----------------------------------------------------------------------*/
template<class T_IndexFunc>
double noise_template(const T_IndexFunc idxfunc, double x, double y, double z, double t)
{
  // Determine cell
  int ix = int(floor(x));
  int iy = int(floor(y));
  int iz = int(floor(z));
  int it = int(floor(t));

  // Coordinate components relative to the cell vertices
  double rx0 = x-ix;
  double ry0 = y-iy;
  double rz0 = z-iz;
  double rt0 = t-it;
  double rx1 = rx0-1.0;
  double ry1 = ry0-1.0;
  double rz1 = rz0-1.0;
  double rt1 = rt0-1.0;

  double sx = rx0*rx0*(3.0-2.0*rx0);
  double sy = ry0*ry0*(3.0-2.0*ry0);
  double sz = rz0*rz0*(3.0-2.0*rz0);
  double st = rt0*rt0*(3.0-2.0*rt0);

  double *g;
  double u,v;
  double a,b,c,d,e,f;

  // at it
  g = grads4[idxfunc(ix,iy,iz,it)];
  u = g[0]*rx0 + g[1]*ry0 + g[2]*rz0 + g[3]*rt0;
  g = grads4[idxfunc(ix+1,iy,iz,it)];
  v = g[0]*rx1 + g[1]*ry0 + g[2]*rz0 + g[3]*rt0;
  a = lerp(sx,u,v);

  g = grads4[idxfunc(ix,iy+1,iz,it)];
  u = g[0]*rx0 + g[1]*ry1 + g[2]*rz0 + g[3]*rt0;
  g = grads4[idxfunc(ix+1,iy+1,iz,it)];
  v = g[0]*rx1 + g[1]*ry1 + g[2]*rz0 + g[3]*rt0;
  b = lerp(sx,u,v);

  c = lerp(sy,a,b);

  g = grads4[idxfunc(ix,iy,iz+1,it)];
  u = g[0]*rx0 + g[1]*ry0 + g[2]*rz1 + g[3]*rt0;
  g = grads4[idxfunc(ix+1,iy,iz+1,it)];
  v = g[0]*rx1 + g[1]*ry0 + g[2]*rz1 + g[3]*rt0;
  a = lerp(sx,u,v);

  g = grads4[idxfunc(ix,iy+1,iz+1,it)];
  u = g[0]*rx0 + g[1]*ry1 + g[2]*rz1 + g[3]*rt0;
  g = grads4[idxfunc(ix+1,iy+1,iz+1,it)];
  v = g[0]*rx1 + g[1]*ry1 + g[2]*rz1 + g[3]*rt0;
  b = lerp(sx,u,v);

  d = lerp(sy,a,b);

  e = lerp(sz,c,d);

  // at it+1
  g = grads4[idxfunc(ix,iy,iz,it+1)];
  u = g[0]*rx0 + g[1]*ry0 + g[2]*rz0 + g[3]*rt1;
  g = grads4[idxfunc(ix+1,iy,iz,it+1)];
  v = g[0]*rx1 + g[1]*ry0 + g[2]*rz0 + g[3]*rt1;
  a = lerp(sx,u,v);

  g = grads4[idxfunc(ix,iy+1,iz,it+1)];
  u = g[0]*rx0 + g[1]*ry1 + g[2]*rz0 + g[3]*rt1;
  g = grads4[idxfunc(ix+1,iy+1,iz,it+1)];
  v = g[0]*rx1 + g[1]*ry1 + g[2]*rz0 + g[3]*rt1;
  b = lerp(sx,u,v);

  c = lerp(sy,a,b);

  g = grads4[idxfunc(ix,iy,iz+1,it+1)];
  u = g[0]*rx0 + g[1]*ry0 + g[2]*rz1 + g[3]*rt1;
  g = grads4[idxfunc(ix+1,iy,iz+1,it+1)];
  v = g[0]*rx1 + g[1]*ry0 + g[2]*rz1 + g[3]*rt1;
  a = lerp(sx,u,v);

  g = grads4[idxfunc(ix,iy+1,iz+1,it+1)];
  u = g[0]*rx0 + g[1]*ry1 + g[2]*rz1 + g[3]*rt1;
  g = grads4[idxfunc(ix+1,iy+1,iz+1,it+1)];
  v = g[0]*rx1 + g[1]*ry1 + g[2]*rz1 + g[3]*rt1;
  b = lerp(sx,u,v);

  d = lerp(sy,a,b);

  f = lerp(sz,c,d);
  
  return lerp(st,e,f);
};

// cellnoise
double cellnoise(double x, double y, double z, double t);
void vcellnoise(double x, double y, double& ox, double& oy);
void vcellnoise(double x, double y, double z, double& ox, double& oy, double& oz);

void vcellnoise(double x, double y, double z, double t,
                double& ox, double& oy, double& oz, double& ot);

inline double cellnoise(const vec3d& p, double t=0.0) { return cellnoise(p.x, p.y, p.z, t); }

// scellnoise
inline double scellnoise(double x, double y, double z, double t) { return 2.0*cellnoise(x,y,z,t)-1.0; }
inline double scellnoise(const vec3d& p, double t=0.0) { return scellnoise(p.x, p.y, p.z, t); }

void vscellnoise(double x, double y, double& ox, double& oy);
void vscellnoise(double x, double y, double z, double& ox, double& oy, double& oz);

void vscellnoise(double x, double y, double z, double t,
                double& ox, double& oy, double& oz, double& ot);


// snoise
double snoise(double x, double y);
double snoise(double x, double y, double z);
double snoise(double x, double y, double z, double t);
inline double snoise(const vec3d& p) { return snoise(p.x, p.y, p.z); }
inline double snoise(const vec3d& p, double t) { return snoise(p.x, p.y, p.z, t); }

// noise
inline double noise(double x, double y) { return 0.5*(snoise(x,y)+1.0); }
inline double noise(double x, double y, double z) { return 0.5*(snoise(x,y,z)+1.0); }
inline double noise(double x, double y, double z, double t) { return 0.5*(snoise(x,y,z,t)+1.0); }
inline double noise(const vec3d& p) { return noise(p.x, p.y, p.z); }
inline double noise(const vec3d& p, double t) { return noise(p.x, p.y, p.z, t); }

// vnoise
void vnoise(double x, double y, double& ox, double& oy);
void vnoise(double x, double y, double z, double& ox, double& oy, double& oz);
void vnoise(double x, double y, double z, double t,
	    double& ox, double& oy, double& oz, double& ot);

// vsnoise
void vsnoise(double x, double y, double& ox, double& oy);
void vsnoise(double x, double y, double z, double& ox, double& oy, double& oz);
void vsnoise(double x, double y, double z, double t,
		   double& ox, double& oy, double& oz, double& ot);

// spnoise
double spnoise(double x, double y, int px, int py);
double spnoise(double x, double y, double z, int px, int py, int pz);
double spnoise(double x, double y, double z, double t, int px, int py, int pz, int pt);
inline double spnoise(const vec3d& p, const vec3d& pp) { return spnoise(p.x, p.y, p.z, int(pp.x), int(pp.y), int(pp.z)); }
inline double spnoise(const vec3d& p, double t, const vec3d& pp, int pt) { return spnoise(p.x, p.y, p.z, t, int(pp.x), int(pp.y), int(pp.z), pt); }

// pnoise
inline double pnoise(double x, double y, int px, int py) { return 0.5*(spnoise(x,y,px,py)+1.0); }
inline double pnoise(double x, double y, double z, int px, int py, int pz) { return 0.5*(spnoise(x,y,z,px,py,pz)+1.0); }
inline double pnoise(double x, double y, double z, double t, int px, int py, int pz, int pt) { return 0.5*(spnoise(x,y,z,t,px,py,pz,pt)+1.0); }
inline double pnoise(const vec3d& p, const vec3d& pp) { return pnoise(p.x, p.y, p.z, int(pp.x), int(pp.y), int(pp.z)); }
inline double pnoise(const vec3d& p, double t, const vec3d& pp, int pt) { return pnoise(p.x, p.y, p.z, t, int(pp.x), int(pp.y), int(pp.z), pt); }


// vspnoise
void vspnoise(double x, double y, int px, int py, double& ox, double& oy);
void vspnoise(double x, double y, double z, int px, int py, int pz,
		    double& ox, double& oy, double& oz);
void vspnoise(double x, double y, double z, double t,
		    int px, int py, int pz, int pt,
		    double& ox, double& oy, double& oz, double& ot);

// vpnoise
void vpnoise(double x, double y, int px, int py, double& ox, double& oy);
void vpnoise(double x, double y, double z, int px, int py, int pz,
	     double& ox, double& oy, double& oz);
void vpnoise(double x, double y, double z, double t,
	     int px, int py, int pz, int pt,
	     double& ox, double& oy, double& oz, double& ot);

// fBm
double fBm(double x, double y, double z, int octaves, double lacunarity, double gain);
inline double fBm(const vec3d& p, int octaves, double lacunarity, double gain) { return fBm(p.x, p.y, p.z, octaves, lacunarity, gain); }
void vfBm(double x, double y, double z, 
          int octaves, double lacunarity, double gain,
          double& ox, double& oy, double& oz);

// turbulence
double turbulence(double x, double y, double z, int octaves, double lacunarity, double gain);
inline double turbulence(const vec3d& p, int octaves, double lacunarity, double gain) { return turbulence(p.x, p.y, p.z, octaves, lacunarity, gain); }
void vturbulence(double x, double y, double z, 
			int octaves, double lacunarity, double gain,
      double& ox, double& oy, double& oz);


} // end of namespace

