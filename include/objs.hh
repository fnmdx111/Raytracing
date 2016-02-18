#ifndef __H_OBJS__
#define __H_OBJS__

#include <vector>
#include "float3.hh"

using namespace std;

class Material
{
public:
  float3 d, s, i;
  double r;

  Material(double adr, double adg, double adb, double asr, double asg, double asb,
	   double ar, double air, double aig, double aib):
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

class Ray
{
public:
  float3 d, p;

  Ray(double dx, double dy, double dz,
      double px, double py, double pz):
    d(float3(dx, dy, dz)), p(float3(px, py, pz)) {}

  int test_with(const vector<Shape*>& shapes,
                vector<Intersection>& is) const;
};

#include "shapes.hh"

class Intersection
{
public:
  double t, t2;
  float3 p, n;
  const Shape* sp;
  Intersection(double t, double t2, float3 p, float3 n, const Shape* sp):
    t(t), t2(t2), p(p), n(n), sp(sp) {}
  Intersection& operator =(const Intersection& i);
};

#endif

