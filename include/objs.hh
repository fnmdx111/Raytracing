
#ifndef __H_OBJS__
#define __H_OBJS__

#include <vector>
#include "float3.hh"
#include "lights.hh"

using namespace std;

class Material
{
public:
  float3 d, s, i;
  float r;

  Material(float adr, float adg, float adb, float asr, float asg, float asb,
	   float ar, float air, float aig, float aib):
    d(float3(adr, adg, adb)), s(float3(asr, asg, asb)),
    i(float3(air, aig, aib)), r(ar) {}
};

class Ray;

class Intersection;

class Shape
{
public:
  const Material* mat;

  void set_material(const Material* material) {
    mat = material;
  }

  virtual int test_with(const Ray& r,
			vector<Intersection>& v) const = 0;
};

class Camera
{
public:
  float3 pos, dir, u, v, w;
  float d, iw, ih;
  int pw, ph;
  float r, l, t, b;
  Camera(float x, float y, float z, float vx, float vy, float vz,
	 float d, float iw, float ih, int pw, int ph):
    pos(float3(x, y, z)), dir(float3(vx, vy, vz)),
    u(float3(0., 0., 0.)), v(float3(0., 0., 0.)), w(float3(-vx, -vy, -vz)),
    d(d), iw(iw), ih(ih), pw(pw), ph(ph),
    r(0.), l(0.), t(0.), b(0.)
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
  }

  Ray ray(int i, int j);
  void copy(const Camera& other);
};

class Ray
{
public:
  float3 d, p;

  Ray(float dx, float dy, float dz,
      float px, float py, float pz):
    d(float3(dx, dy, dz)), p(float3(px, py, pz)) {}

  int test_with(const vector<Shape*>& shapes,
		vector<Intersection>& is) const;
};

#include "shapes.hh"

class Intersection
{
public:
  const bool yes;
  float t, t2;
  float3 p, n;
  const Shape* sp;
  Intersection(float t, float t2, float3 p, float3 n, const Shape* sp):
    yes(true), t(t), t2(t2), p(p), n(n), sp(sp) {}
};

#endif

