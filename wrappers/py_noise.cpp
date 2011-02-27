/*
 Noise functions
 */

#include <boost/python.hpp>
#include "noise.h"
#include "vec3.h"
#include "vec4.h"

using namespace boost::python;
using namespace support3d;

// vnoise variations

double vnoise_x(double x)
{
  return noise(x,0);
}

object vnoise_xy(double x, double y)
{
  double ox, oy;
  vnoise(x,y, ox,oy);
  return make_tuple(ox, oy);
}

vec3d vnoise_xyz(double x, double y, double z)
{
  double ox, oy, oz;
  vnoise(x,y,z, ox,oy,oz);
  return vec3d(ox,oy,oz);
}

vec4d vnoise_xyzt(double x, double y, double z, double t)
{
  double ox, oy, oz, ot;
  vnoise(x,y,z,t, ox,oy,oz,ot);
  return vec4d(ox,oy,oz,ot);
}

vec3d vnoise_vec3(const vec3d& p)
{
  double ox, oy, oz;
  vnoise(p.x,p.y,p.z, ox,oy,oz);
  return vec3d(ox,oy,oz);
}

vec3d vnoise_vec3t(const vec3d& p, double t)
{
  double ox, oy, oz, ot;
  vnoise(p.x,p.y,p.z,t, ox,oy,oz,ot);
  return vec3d(ox,oy,oz);
}

// vsnoise variations

double vsnoise_x(double x)
{
  return snoise(x,0);
}

object vsnoise_xy(double x, double y)
{
  double ox, oy;
  vsnoise(x,y, ox,oy);
  return make_tuple(ox, oy);
}

vec3d vsnoise_xyz(double x, double y, double z)
{
  double ox, oy, oz;
  vsnoise(x,y,z, ox,oy,oz);
  return vec3d(ox,oy,oz);
}

vec4d vsnoise_xyzt(double x, double y, double z, double t)
{
  double ox, oy, oz, ot;
  vsnoise(x,y,z,t, ox,oy,oz,ot);
  return vec4d(ox,oy,oz,ot);
}

vec3d vsnoise_vec3(const vec3d& p)
{
  double ox, oy, oz;
  vsnoise(p.x,p.y,p.z, ox,oy,oz);
  return vec3d(ox,oy,oz);
}

vec3d vsnoise_vec3t(const vec3d& p, double t)
{
  double ox, oy, oz, ot;
  vsnoise(p.x,p.y,p.z,t, ox,oy,oz,ot);
  return vec3d(ox,oy,oz);
}

// vpnoise variations

double vpnoise_x(double x, int px)
{
  return pnoise(x,0, px, 1);
}

object vpnoise_xy(double x, double y, int px, int py)
{
  double ox, oy;
  vpnoise(x,y, px,py, ox,oy);
  return make_tuple(ox, oy);
}

vec3d vpnoise_xyz(double x, double y, double z, int px, int py, int pz)
{
  double ox, oy, oz;
  vpnoise(x,y,z, px,py,pz, ox,oy,oz);
  return vec3d(ox,oy,oz);
}

vec4d vpnoise_xyzt(double x, double y, double z, double t, int px, int py, int pz, int pt)
{
  double ox, oy, oz, ot;
  vpnoise(x,y,z,t, px,py,pz,pt, ox,oy,oz,ot);
  return vec4d(ox,oy,oz,ot);
}

// vspnoise variations

double vspnoise_x(double x, int px)
{
  return spnoise(x,0, px, 1);
}

object vspnoise_xy(double x, double y, int px, int py)
{
  double ox, oy;
  vspnoise(x,y, px,py, ox,oy);
  return make_tuple(ox, oy);
}

vec3d vspnoise_xyz(double x, double y, double z, int px, int py, int pz)
{
  double ox, oy, oz;
  vspnoise(x,y,z, px,py,pz, ox,oy,oz);
  return vec3d(ox,oy,oz);
}

vec4d vspnoise_xyzt(double x, double y, double z, double t, int px, int py, int pz, int pt)
{
  double ox, oy, oz, ot;
  vspnoise(x,y,z,t, px,py,pz,pt, ox,oy,oz,ot);
  return vec4d(ox,oy,oz,ot);
}


// vcellnoise variations

double vcellnoise_x(double x)
{
  return cellnoise(x,0,0,0);
}

object vcellnoise_xy(double x, double y)
{
  double ox, oy;
  vcellnoise(x,y, ox,oy);
  return make_tuple(ox, oy);
}

vec3d vcellnoise_xyz(double x, double y, double z)
{
  double ox, oy, oz;
  vcellnoise(x,y,z, ox,oy,oz);
  return vec3d(ox,oy,oz);
}

vec4d vcellnoise_xyzt(double x, double y, double z, double t)
{
  double ox, oy, oz, ot;
  vcellnoise(x,y,z,t, ox,oy,oz,ot);
  return vec4d(ox,oy,oz,ot);
}

vec3d vcellnoise_vec3(const vec3d& p)
{
  double ox, oy, oz;
  vcellnoise(p.x,p.y,p.z, ox,oy,oz);
  return vec3d(ox,oy,oz);
}

vec3d vcellnoise_vec3t(const vec3d& p, double t)
{
  double ox, oy, oz, ot;
  vcellnoise(p.x,p.y,p.z,t, ox,oy,oz,ot);
  return vec3d(ox,oy,oz);
}

// vscellnoise variations

double vscellnoise_x(double x)
{
  return scellnoise(x,0,0,0);
}

object vscellnoise_xy(double x, double y)
{
  double ox, oy;
  vscellnoise(x,y, ox,oy);
  return make_tuple(ox, oy);
}

vec3d vscellnoise_xyz(double x, double y, double z)
{
  double ox, oy, oz;
  vscellnoise(x,y,z, ox,oy,oz);
  return vec3d(ox,oy,oz);
}

vec4d vscellnoise_xyzt(double x, double y, double z, double t)
{
  double ox, oy, oz, ot;
  vscellnoise(x,y,z,t, ox,oy,oz,ot);
  return vec4d(ox,oy,oz,ot);
}

vec3d vscellnoise_vec3(const vec3d& p)
{
  double ox, oy, oz;
  vscellnoise(p.x,p.y,p.z, ox,oy,oz);
  return vec3d(ox,oy,oz);
}

vec3d vscellnoise_vec3t(const vec3d& p, double t)
{
  double ox, oy, oz, ot;
  vscellnoise(p.x,p.y,p.z,t, ox,oy,oz,ot);
  return vec3d(ox,oy,oz);
}

// vfBm

vec3d vfbm_vec3(const vec3d& p, int octaves, double lacunarity, double gain)
{
  double ox, oy, oz;
  vfBm(p.x,p.y,p.z, octaves, lacunarity, gain, ox,oy,oz);
  return vec3d(ox,oy,oz);
}

// turbulence

vec3d vturbulence_vec3(const vec3d& p, int octaves, double lacunarity, double gain)
{
  double ox, oy, oz;
  vturbulence(p.x,p.y,p.z, octaves, lacunarity, gain, ox,oy,oz);
  return vec3d(ox,oy,oz);
}


//////////////////////////////////////////////////////////////////////
void def_noises()
{
  double (*cellnoise_xyzt)(double x, double y, double z, double t) = cellnoise;
  double (*cellnoise_vec3t)(const vec3d& p, double t) = cellnoise;
  double (*scellnoise_xyzt)(double x, double y, double z, double t) = scellnoise;
  double (*scellnoise_vec3t)(const vec3d& p, double t) = scellnoise;
  double (*noise_xy)(double x, double y) = noise;
  double (*noise_xyz)(double x, double y, double z) = noise;
  double (*noise_xyzt)(double x, double y, double z, double t) = noise;
  double (*noise_vec3)(const vec3d& p) = noise;
  double (*noise_vec3t)(const vec3d& p, double t) = noise;
  double (*snoise_xy)(double x, double y) = snoise;
  double (*snoise_xyz)(double x, double y, double z) = snoise;
  double (*snoise_xyzt)(double x, double y, double z, double t) = snoise;
  double (*snoise_vec3)(const vec3d& p) = snoise;
  double (*snoise_vec3t)(const vec3d& p, double t) = snoise;
  double (*pnoise_xy)(double x, double y, int px, int py) = pnoise;
  double (*pnoise_xyz)(double x, double y, double z, int px, int py, int pz) = pnoise;
  double (*pnoise_xyzt)(double x, double y, double z, double t, int px, int py, int pz, int pt) = pnoise;
  double (*pnoise_vec3)(const vec3d& p, const vec3d& pp) = pnoise;
  double (*pnoise_vec3t)(const vec3d& p, double t, const vec3d& pp, int pt) = pnoise;
  double (*spnoise_xy)(double x, double y, int px, int py) = spnoise;
  double (*spnoise_xyz)(double x, double y, double z, int px, int py, int pz) = spnoise;
  double (*spnoise_xyzt)(double x, double y, double z, double t, int px, int py, int pz, int pt) = spnoise;
  double (*spnoise_vec3)(const vec3d& p, const vec3d& pp) = spnoise;
  double (*spnoise_vec3t)(const vec3d& p, double t, const vec3d& pp, int pt) = spnoise;

  //  double (*fbm_xyz)(double x, double y, double z, int octaves, double lacunarity, double gain) = fBm;
  double (*fbm_vec3)(const vec3d& p, int octaves, double lacunarity, double gain) = fBm;
  //  double (*turbulence_xyz)(double x, double y, double z, int octaves, double lacunarity, double gain) = turbulence;
  double (*turbulence_vec3)(const vec3d& p, int octaves, double lacunarity, double gain) = turbulence;


  // noise
  def("noise", noise_xy,
      (arg("x"), arg("y")=0.0),
      "noise(x[, y[, z[, t]]]) -> float\n\n"
      "Returns a noise value (Perlin) in the range from 0 to 1. The arguments\n"
      "can be up to 4 floating point values or a sequence with up to 4 floating\n"
      "point values. Time can be specified separately.");

  def("noise", noise_xyz, (arg("x"), arg("y"), arg("z")));
  def("noise", noise_xyzt, (arg("x"), arg("y"), arg("z"), arg("t")));
  def("noise", noise_vec3, (arg("p")));
  def("noise", noise_vec3t, (arg("p"), arg("t")));

  // snoise
  def("snoise", snoise_xy,
      (arg("x"), arg("y")=0.0),
      "snoise(x[, y[, z[, t]]]) -> float\n\n"
      "Returns a signed noise value (Perlin) in the range from -1 to 1.\n"
      "A call to snoise(args) is equivalent to 2*noise(args)-1.");

  def("snoise", snoise_xyz, (arg("x"), arg("y"), arg("z")));
  def("snoise", snoise_xyzt, (arg("x"), arg("y"), arg("z"), arg("t")));
  def("snoise", snoise_vec3, (arg("p")));
  def("snoise", snoise_vec3t, (arg("p"), arg("t")));

  // pnoise
  def("pnoise", pnoise_xy,
      (arg("x"), arg("y"), arg("px"), arg("py")),
      "pnoise(point, period) / pnoise(point, t, pperiod, tperiod)-> float\n\n"
      "Periodic noise function. Basically this is the same than noise but with\n"
      "a periodic return value: pnoise(point) = pnoise(point+period). The time\n"
      "value can be either part of the point or it can be specified separately.\n"
      "The point and period must always have the same dimension. The return \n"
      "value is in the range from 0 to 1.");

  def("pnoise", pnoise_xyz, (arg("x"), arg("y"), arg("z"), arg("px"), arg("py"), arg("pz")));
  def("pnoise", pnoise_xyzt, (arg("x"), arg("y"), arg("z"), arg("t"), arg("px"), arg("py"), arg("pz"), arg("pt")));
  def("pnoise", pnoise_vec3, (arg("p"), arg("pp")));
  def("pnoise", pnoise_vec3t, (arg("p"), arg("t"), arg("pp"), arg("tp")));

  // spnoise
  def("spnoise", spnoise_xy,
      (arg("x"), arg("y"), arg("px"), arg("py")),
      "spnoise(point, period) / spnoise(point, t, pperiod, tperiod)-> float\n\n"
      "Signed periodic noise function. The return value is in the range from\n"
      "-1 to 1. A call to spnoise(args) is equivalent to 2*pnoise(args)-1.");

  def("spnoise", spnoise_xyz, (arg("x"), arg("y"), arg("z"), arg("px"), arg("py"), arg("pz")));
  def("spnoise", spnoise_xyzt, (arg("x"), arg("y"), arg("z"), arg("t"), arg("px"), arg("py"), arg("pz"), arg("pt")));
  def("spnoise", spnoise_vec3, (arg("p"), arg("pp")));
  def("spnoise", spnoise_vec3t, (arg("p"), arg("t"), arg("pp"), arg("tp")));

  // cellnoise
  def("cellnoise", cellnoise_xyzt,
      (arg("x"), arg("y")=0.0, arg("z")=0.0, arg("t")=0.0),
      "cellnoise(x, y=0, z=0, t=0) -> float\n\n"
      "Returns a cellnoise value in the range from 0 to 1. The return value\n"
      "is constant between integer lattice points. The arguments can be up\n"
      "to 4 floating point values or a sequence with up to 4 floating point\n"
      "values. Time can be specified separately.");

  def("cellnoise", cellnoise_vec3t, (arg("p"), arg("t")=0.0));

  // scellnoise
  def("scellnoise", scellnoise_xyzt,
      (arg("x"), arg("y")=0.0, arg("z")=0.0, arg("t")=0.0),
      "scellnoise(x, y=0, z=0, t=0) -> float\n\n"
      "Signed cell noise. The return value is in the range from -1 to 1.\n"
      "A call to scellnoise(args) is equivalent to 2*cellnoise(args)-1.");

  def("scellnoise", scellnoise_vec3t, (arg("p"), arg("t")=0.0));

  // fBm
  def("fBm", fbm_vec3, 
      (arg("p"), arg("octaves"), arg("lacunarity")=2.0, arg("gain")=0.5), 
      "fBm(point, octaves, lacunarity=2.0, gain=0.5) -> float\n\n"
      "Fractional Brownian motion. The argument point must be a sequence of\n"
      "either 2 or 3 float values (e.g. a vec3). This function is a sum of\n"
      "noise values with different frequencies and amplitudes");

  // turbulence
  def("turbulence", turbulence_vec3, 
      (arg("p"), arg("octaves"), arg("lacunarity")=2.0, arg("gain")=0.5), 
      "turbulence(point, octaves, lacunarity=2.0, gain=0.5) -> float\n\n"
      "The code of the turbulence function is very similar to fBm. The\n"
      "difference is that it sums up abs(snoise) instead of snoise.\n"
      "However, the return value is in the range from 0 to 1 again.");

  // vnoise
  def("vnoise", vnoise_x, arg("x"));
  def("vnoise", vnoise_xy, (arg("x"), arg("y")));
  def("vnoise", vnoise_xyz, (arg("x"), arg("y"), arg("z")));
  def("vnoise", vnoise_xyzt, (arg("x"), arg("y"), arg("z"), arg("t")));
  def("vnoise", vnoise_vec3, (arg("p")));
  def("vnoise", vnoise_vec3t, (arg("p"), arg("t")));

  // vsnoise
  def("vsnoise", vsnoise_x, arg("x"));
  def("vsnoise", vsnoise_xy, (arg("x"), arg("y")));
  def("vsnoise", vsnoise_xyz, (arg("x"), arg("y"), arg("z")));
  def("vsnoise", vsnoise_xyzt, (arg("x"), arg("y"), arg("z"), arg("t")));
  def("vsnoise", vsnoise_vec3, (arg("p")));
  def("vsnoise", vsnoise_vec3t, (arg("p"), arg("t")));

  // vpnoise
  def("vpnoise", vpnoise_x, arg("x"));
  def("vpnoise", vpnoise_xy, (arg("x"), arg("y")));
  def("vpnoise", vpnoise_xyz, (arg("x"), arg("y"), arg("z")));
  def("vpnoise", vpnoise_xyzt, (arg("x"), arg("y"), arg("z"), arg("t")));

  // vspnoise
  def("vspnoise", vspnoise_x, arg("x"));
  def("vspnoise", vspnoise_xy, (arg("x"), arg("y")));
  def("vspnoise", vspnoise_xyz, (arg("x"), arg("y"), arg("z")));
  def("vspnoise", vspnoise_xyzt, (arg("x"), arg("y"), arg("z"), arg("t")));

  // vcellnoise
  def("vcellnoise", vcellnoise_x, arg("x"));
  def("vcellnoise", vcellnoise_xy, (arg("x"), arg("y")));
  def("vcellnoise", vcellnoise_xyz, (arg("x"), arg("y"), arg("z")));
  def("vcellnoise", vcellnoise_xyzt, (arg("x"), arg("y"), arg("z"), arg("t")));
  def("vcellnoise", vcellnoise_vec3, (arg("p")));
  def("vcellnoise", vcellnoise_vec3t, (arg("p"), arg("t")));

  // vscellnoise
  def("vscellnoise", vscellnoise_x, arg("x"));
  def("vscellnoise", vscellnoise_xy, (arg("x"), arg("y")));
  def("vscellnoise", vscellnoise_xyz, (arg("x"), arg("y"), arg("z")));
  def("vscellnoise", vscellnoise_xyzt, (arg("x"), arg("y"), arg("z"), arg("t")));
  def("vscellnoise", vscellnoise_vec3, (arg("p")));
  def("vscellnoise", vscellnoise_vec3t, (arg("p"), arg("t")));

  // vfBm
  def("vfBm", vfbm_vec3, (arg("p"), arg("octaves"), arg("lacunarity")=2.0, arg("gain")=0.5));

  // vturbulence
  def("vturbulence", vturbulence_vec3, (arg("p"), arg("octaves"), arg("lacunarity")=2.0, arg("gain")=0.5));

}
