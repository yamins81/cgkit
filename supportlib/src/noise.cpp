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

#include "noise.h"


namespace support3d {

/*----------------------------------------------------------------------
  cellnoise

  Returns a value between 0 and 1 which is constant between integer
  lattice points.
----------------------------------------------------------------------*/
double cellnoise(double x, double y, double z, double t)
{
  int ix = int(floor(x));
  int iy = int(floor(y));
  int iz = int(floor(z));
  int it = int(floor(t));
  return uniform[tabindex4(ix,iy,iz,it)];
};

/*----------------------------------------------------------------------
  vcellnoise (2D)

  Returns a value between 0 and 1 which is constant between integer
  lattice points.
----------------------------------------------------------------------*/
void vcellnoise(double x, double y, double& ox, double& oy)
{
  ox = cellnoise(x,y,0.0,0.0);
  x+=10.0;
  oy = cellnoise(x,y,0.0,0.0);
};

/*----------------------------------------------------------------------
  vcellnoise (3D)

  Returns a value between 0 and 1 which is constant between integer
  lattice points.
----------------------------------------------------------------------*/
void vcellnoise(double x, double y, double z, double& ox, double& oy, double& oz)
{
  ox = cellnoise(x,y,z,0.0);
  x+=10.0;
  oy = cellnoise(x,y,z,0.0);
  y+=10.0;
  oz = cellnoise(x,y,z,0.0);
};

/*----------------------------------------------------------------------
  vcellnoise (4D)

  Returns a value between 0 and 1 which is constant between integer
  lattice points.
----------------------------------------------------------------------*/
void vcellnoise(double x, double y, double z, double t,
                double& ox, double& oy, double& oz, double& ot)
{
  ox = cellnoise(x,y,z,t);
  x+=10.0;
  oy = cellnoise(x,y,z,t);
  y+=10.0;
  oz = cellnoise(x,y,z,t);
  z+=10.0;
  ot = cellnoise(x,y,z,t);
};

/*----------------------------------------------------------------------
  vscellnoise (2D)

  Returns a value between -1 and 1 which is constant between integer
  lattice points.
----------------------------------------------------------------------*/
void vscellnoise(double x, double y, double& ox, double& oy)
{
  vcellnoise(x,y, ox,oy);
  ox = 2*ox - 1.0;
  oy = 2*oy - 1.0;
};

/*----------------------------------------------------------------------
  vscellnoise (3D)

  Returns a value between -1 and 1 which is constant between integer
  lattice points.
----------------------------------------------------------------------*/
void vscellnoise(double x, double y, double z, double& ox, double& oy, double& oz)
{
  vcellnoise(x,y,z, ox,oy,oz);
  ox = 2*ox - 1.0;
  oy = 2*oy - 1.0;
  oz = 2*oz - 1.0;
};

/*----------------------------------------------------------------------
  vscellnoise (4D)

  Returns a value between -1 and 1 which is constant between integer
  lattice points.
----------------------------------------------------------------------*/
void vscellnoise(double x, double y, double z, double t,
                double& ox, double& oy, double& oz, double& ot)
{
  vcellnoise(x,y,z,t, ox,oy,oz,ot);
  ox = 2*ox - 1.0;
  oy = 2*oy - 1.0;
  oz = 2*oz - 1.0;
  ot = 2*ot - 1.0;
};


/*----------------------------------------------------------------------
  snoise (2D)
  Returns a value between -1 and 1 
----------------------------------------------------------------------*/
double snoise(double x, double y)
{
  return noise_template(tabindex2,x,y);
};

/*---------------------------------------------------------------------- 
  snoise (3D)
  Returns a value between -1 and 1 
----------------------------------------------------------------------*/
double snoise(double x, double y, double z)
{
  return noise_template(tabindex3,x,y,z);
};

/*----------------------------------------------------------------------
  snoise (4D)
  Returns a value between -1 and 1 
----------------------------------------------------------------------*/
double snoise(double x, double y, double z, double t)
{
  return noise_template(tabindex4,x,y,z,t);
};

/*---------------------------------------------------------------------- 
   vsnoise (2D)
   Returns values between -1 and 1 
----------------------------------------------------------------------*/
void vsnoise(double x, double y, double& ox, double& oy)
{
  ox = noise_template(tabindex2,x,y);
  x += 10.0;
  oy = noise_template(tabindex2,x,y);
};

/*---------------------------------------------------------------------- 
   vsnoise (3D)
   Returns values between -1 and 1 
----------------------------------------------------------------------*/
void vsnoise(double x, double y, double z, double& ox, double& oy, double& oz)
{
  ox = noise_template(tabindex3,x,y,z);
  x += 10.0;
  oy = noise_template(tabindex3,x,y,z);
  y += 10.0;
  oz = noise_template(tabindex3,x,y,z);
};

/*---------------------------------------------------------------------- 
   vsnoise (4D)
   Returns values between -1 and 1 
----------------------------------------------------------------------*/
void vsnoise(double x, double y, double z, double t,
		   double& ox, double& oy, double& oz, double& ot)
{
  ox = noise_template(tabindex4,x,y,z,t);
  x += 10.0;
  oy = noise_template(tabindex4,x,y,z,t);
  y += 10.0;
  oz = noise_template(tabindex4,x,y,z,t);
  z += 10.0;
  ot = noise_template(tabindex4,x,y,z,t);
};

/*---------------------------------------------------------------------- 
   vnoise (2D)
   Returns values between 0 and 1 
----------------------------------------------------------------------*/
void vnoise(double x, double y, double& ox, double& oy)
{
  vsnoise(x, y, ox, oy);
  ox = 0.5*(ox+1.0);
  oy = 0.5*(oy+1.0);
};

/*---------------------------------------------------------------------- 
   vnoise (3D)
   Returns values between 0 and 1 
----------------------------------------------------------------------*/
void vnoise(double x, double y, double z, double& ox, double& oy, double& oz)
{
  vsnoise(x, y, z, ox, oy, oz);
  ox = 0.5*(ox+1.0);
  oy = 0.5*(oy+1.0);
  oz = 0.5*(oz+1.0);
};

/*---------------------------------------------------------------------- 
   vnoise (4D)
   Returns values between 0 and 1 
----------------------------------------------------------------------*/
void vnoise(double x, double y, double z, double t,
	    double& ox, double& oy, double& oz, double& ot)
{
  vsnoise(x, y, z, t, ox, oy, oz, ot);
  ox = 0.5*(ox+1.0);
  oy = 0.5*(oy+1.0);
  oz = 0.5*(oz+1.0);
  ot = 0.5*(ot+1.0);
};


/*----------------------------------------------------------------------
   pnoise (2D)
   Returns a value between -1 and 1 
----------------------------------------------------------------------*/
double spnoise(double x, double y, int px, int py)
{
  if ((px==0) || (py==0))
    throw EValueError("period must not be zero");
  xperiod = px;
  yperiod = py;
  return noise_template(ptabindex2,x,y);
};

/*----------------------------------------------------------------------
   pnoise (3D)
   Returns a value between -1 and 1 
----------------------------------------------------------------------*/
double spnoise(double x, double y, double z, int px, int py, int pz)
{
  if ((px==0) || (py==0) || (pz==0))
    throw EValueError("period must not be zero");
  xperiod = px;
  yperiod = py;
  zperiod = pz;
  return noise_template(ptabindex3,x,y,z);
};

/*----------------------------------------------------------------------
   pnoise (4D)
   Returns a value between -1 and 1 
----------------------------------------------------------------------*/
double spnoise(double x, double y, double z, double t, int px, int py, int pz, int pt)
{
  if ((px==0) || (py==0) || (pz==0) || (pt==0))
    throw EValueError("period must not be zero");
  xperiod = px;
  yperiod = py;
  zperiod = pz;
  tperiod = pt;
  return noise_template(ptabindex4,x,y,z,t);
};

/*---------------------------------------------------------------------- 
   vpnoise (2D)
   Returns values between -1 and 1 
----------------------------------------------------------------------*/
void vspnoise(double x, double y, int px, int py, double& ox, double& oy)
{
  if ((px==0) || (py==0))
    throw EValueError("period must not be zero");
  xperiod = px;
  yperiod = py;
  poffset = 0;
  ox = noise_template(ptabindex2offset,x,y);
  x += 10.0;
  poffset = 37;
  oy = noise_template(ptabindex2offset,x,y);
};

/*---------------------------------------------------------------------- 
   vpnoise (3D)
   Returns values between -1 and 1 
----------------------------------------------------------------------*/
void vspnoise(double x, double y, double z, 
  	    int px, int py, int pz,
		    double& ox, double& oy, double& oz)
{
  if ((px==0) || (py==0) || (pz==0))
    throw EValueError("period must not be zero");
  xperiod = px;
  yperiod = py;
  zperiod = pz;
  poffset = 0;
  ox = noise_template(ptabindex3offset,x,y,z);
  x += 10.0;
  poffset = 37;
  oy = noise_template(ptabindex3offset,x,y,z);
  y += 10.0;
  poffset = 99;
  oz = noise_template(ptabindex3offset,x,y,z);
};

/*---------------------------------------------------------------------- 
   vpnoise (4D)
   Returns values between -1 and 1 
----------------------------------------------------------------------*/
void vspnoise(double x, double y, double z, double t,
		    int px, int py, int pz, int pt,
		    double& ox, double& oy, double& oz, double& ot)
{
  if ((px==0) || (py==0) || (pz==0) || (pt==0))
    throw EValueError("period must not be zero");
  xperiod = px;
  yperiod = py;
  zperiod = pz;
  tperiod = pt;
  poffset = 0;
  ox = noise_template(ptabindex4offset,x,y,z,t);
  x += 10.0;
  poffset = 37;
  oy = noise_template(ptabindex4offset,x,y,z,t);
  y += 10.0;
  poffset = 99;
  oz = noise_template(ptabindex4offset,x,y,z,t);
  z += 10.0;
  poffset = 105;
  ot = noise_template(ptabindex4offset,x,y,z,t);
};

/*---------------------------------------------------------------------- 
   vpnoise (2D)
   Returns values between 0 and 1 
----------------------------------------------------------------------*/
void vpnoise(double x, double y, int px, int py, double& ox, double& oy)
{
  vspnoise(x,y, px,py, ox,oy);
  ox = 0.5*(ox+1.0);
  oy = 0.5*(oy+1.0);
};

/*---------------------------------------------------------------------- 
   vpnoise (3D)
   Returns values between 0 and 1 
----------------------------------------------------------------------*/
void vpnoise(double x, double y, double z, int px, int py, int pz, double& ox, double& oy, double& oz)
{
  vspnoise(x,y,z, px,py,pz, ox,oy,oz);
  ox = 0.5*(ox+1.0);
  oy = 0.5*(oy+1.0);
  oz = 0.5*(oz+1.0);
};

/*---------------------------------------------------------------------- 
   vpnoise (4D)
   Returns values between 0 and 1 
----------------------------------------------------------------------*/
void vpnoise(double x, double y, double z, double t,
	    int px, int py, int pz, int pt,
	    double& ox, double& oy, double& oz, double& ot)
{
  vspnoise(x,y,z,t, px,py,pz,pt, ox,oy,oz,ot);
  ox = 0.5*(ox+1.0);
  oy = 0.5*(oy+1.0);
  oz = 0.5*(oz+1.0);
  ot = 0.5*(ot+1.0);
};


/*----------------------------------------------------------------------
  Fractional Brownian Motion
  (as described in Advanced RenderMan)
----------------------------------------------------------------------*/
double fBm(double x, double y, double z, int octaves, double lacunarity, double gain)
{
  double res=0.0;
  double amp=1.0;

  for(int i=0; i<octaves; i+=1)
  {
    res += amp*snoise(x,y,z);
    amp *= gain;
    x *= lacunarity;
    y *= lacunarity;
    z *= lacunarity;
  }
  return 0.5*(res+1.0);
}

/*----------------------------------------------------------------------
  Vector Fractional Brownian Motion
----------------------------------------------------------------------*/
void vfBm(double x, double y, double z, 
          int octaves, double lacunarity, double gain,
          double& ox, double& oy, double& oz)
{
  double amp=1.0;
  double nx,ny,nz;

  ox=0.0;
  oy=0.0;
  oz=0.0;

  for(int i=0; i<octaves; i+=1)
  {
    vsnoise(x,y,z, nx,ny,nz);
    ox += amp*nx;
    oy += amp*ny;
    oz += amp*nz;
    amp *= gain;
    x *= lacunarity;
    y *= lacunarity;
    z *= lacunarity;
  }
  ox = 0.5*(ox+1.0);
  oy = 0.5*(oy+1.0);
  oz = 0.5*(oz+1.0);
}

/*----------------------------------------------------------------------
  Turbulence
  (as described in Advanced RenderMan)
----------------------------------------------------------------------*/
double turbulence(double x, double y, double z, int octaves, double lacunarity, double gain)
{
  double res=0.0;
  double amp=1.0;

  for(int i=0; i<octaves; i+=1)
  {
    res += amp*fabs(snoise(x,y,z));
    amp *= gain;
    x *= lacunarity;
    y *= lacunarity;
    z *= lacunarity;
  }
  return 0.5*(res+1.0);
}

/*----------------------------------------------------------------------
  Vector Turbulence
----------------------------------------------------------------------*/
void vturbulence(double x, double y, double z, 
			int octaves, double lacunarity, double gain,
			double& ox, double& oy, double& oz)
{
  double amp=1.0;
  double nx,ny,nz;

  ox=0.0;
  oy=0.0;
  oz=0.0;

  for(int i=0; i<octaves; i+=1)
  {
    vsnoise(x,y,z, nx,ny,nz);
    ox += amp*fabs(nx);
    oy += amp*fabs(ny);
    oz += amp*fabs(nz);
    amp *= gain;
    x *= lacunarity;
    y *= lacunarity;
    z *= lacunarity;
  }
  ox = 0.5*(ox+1.0);
  oy = 0.5*(oy+1.0);
  oz = 0.5*(oz+1.0);
}


} // end of namespace
