#ifndef __H_CAMERA__
#define __H_CAMERA__

#include "float3.hh"
#include <ImfRgbaFile.h>
#include <ImfArray.h>
#include <random>

using namespace Imf;

class Scene;

#define MAXRECUR 6

#define CAMEPSILON -0.001

class Camera
{
public:
  float3 pos, dir, u, v, w;
  double d, iw, ih;
  int pw, ph;
  double r, l, t, b;
  double lens;
  double aperture_size = 1e-8;

  std::random_device rd;
  std::mt19937 e2;
  std::uniform_real_distribution<> dst;

  const Scene* scene;

  Array2D<Rgba> pixels;

  Camera(double x, double y, double z, double vx, double vy, double vz,
         double d, double iw, double ih, int pw, int ph, Scene* scene,
         double lens = 1e-8, double aperture_size = 1e-8):
    pos(float3(x, y, z)), dir(float3(vx, vy, vz)),
    u(float3(0., 0., 0.)), v(float3(0., 0., 0.)), w(float3(-vx, -vy, -vz)),
    d(d), iw(iw), ih(ih), pw(pw), ph(ph),
    r(0.), l(0.), t(0.), b(0.),
    lens(std::isnan(lens) ? 1e100 : lens),
    aperture_size(std::isnan(aperture_size) ? 1e-100 : aperture_size),
    e2(std::mt19937(rd())),
    dst(std::uniform_real_distribution<>(0.0, 0.999999999)),
    scene(scene)
  {
    u = this->dir * float3(0, 1, 0);
    v = u * dir;

    u.normalize_();
    v.normalize_();
    w.normalize_();
    dir.normalize_();

    r = this->iw / 2.0;
    l = -r;
    t = this->ih / 2.0;
    b = -t;

    pixels.resizeErase(ph, pw);
  }

  Camera(const Camera& cam);

  Ray ray(double i, double j);
  Ray ray(double i, double j, double& dn);
  void copy(const Camera& other);
  inline void accum_pixel(int i, int j, const float3& rgb);
  inline void set_pixel(int i, int j, const float3& rgb);

  void ray_color(int recursion_depth,
                   float3& ret,
                   const Ray& r,
                   const Light* shadow_lgh,
                   double t0, double t1);

  void render();
  void save(const string& s) const;
};

#include "scene.hh"

#endif
