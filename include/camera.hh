#ifndef __H_CAMERA__
#define __H_CAMERA__

#include "float3.hh"
#include <ImfRgbaFile.h>
#include <ImfArray.h>

using namespace Imf;

class Scene;


#ifdef FEAT_ANTIALIASING
#define NSAMPLE 4
#endif

#ifdef FEAT_SPECULAR_REFLECTION
#define MAXRECUR 10
#endif

#define CAMEPSILON -0.000001

class Camera
{
public:
  float3 pos, dir, u, v, w;
  double d, iw, ih;
  int pw, ph;
  double r, l, t, b;

  const Scene* scene;

  Array2D<Rgba> pixels;

  Camera(double x, double y, double z, double vx, double vy, double vz,
	 double d, double iw, double ih, int pw, int ph, Scene* scene):
    pos(float3(x, y, z)), dir(float3(vx, vy, vz)),
    u(float3(0., 0., 0.)), v(float3(0., 0., 0.)), w(float3(-vx, -vy, -vz)),
    d(d), iw(iw), ih(ih), pw(pw), ph(ph),
    r(0.), l(0.), t(0.), b(0.), scene(scene)
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
  void copy(const Camera& other);
  inline void accum_pixel(int i, int j, const float3& rgb);
  inline void set_pixel(int i, int j, const float3& rgb);
#ifdef FEAT_SHADOW
  bool is_shadowed(const Light& lgh, const Intersection& in,
                   const Ray& pr) const;
#endif
  void ray_color(const Ray& r, float3& clr, int recursion_depth) const;
#ifdef FEAT_SPECULAR_REFLECTION
  void specular_reflection(const Intersection& in, const Ray& r,
                           float3& clr, int recursion_depth) const;
#endif
  void render();
  void save(const string& s) const;
};

#include "scene.hh"

#endif
