#ifndef __H_OBJS__
#define __H_OBJS__

#include <vector>
#include "float3.hh"

using namespace std;

class Intersection;
class Light;

class Material
{
public:
  float3 d, s, i;
  double r;

  Material(double adr, double adg, double adb, double asr, double asg, double asb,
	   double ar, double air, double aig, double aib):
    d(float3(adr, adg, adb)), s(float3(asr, asg, asb)),
    i(float3(air, aig, aib)), r(ar) {}

  float3 diffuse(const Light& lgh, const Intersection& in) const;
  float3 specular(const Light& lgh, const Intersection& in,
                               const float3& v) const;
  float3 ambient(const Light& lgh) const;
};

#include "lights.hh"

class Ray;

enum ShapeType { sphere, triangle, plane };

class Shape
{
  const static string __sphere;
  const static string __triangle;
  const static string __plane;
  const static string __unknown;

public:
  const Material* mat;

  void set_material(const Material* material) {
    mat = material;
  }

  virtual int test_with(const Ray& r,
												vector<Intersection>& v,
                        double t0, double t1) const = 0;
  virtual ShapeType type() const = 0;
  static const string& to_s(ShapeType s) {
    if (s == ShapeType::sphere) {
      return Shape::__sphere;
    } else if (s == ShapeType::triangle) {
      return Shape::__triangle;
    } else if (s == ShapeType::plane) {
      return Shape::__plane;
    }
    return Shape::__unknown;
  }
};

class Ray
{
public:
  float3 d, p;
  char type;

  Ray(double dx, double dy, double dz,
      double px, double py, double pz):
  d(float3(dx, dy, dz)), p(float3(px, py, pz)), type('r') {}
  Ray(const float3& dd, const float3& pp): type('r')
  {
    this->d.x = dd.x;
    this->d.y = dd.y;
    this->d.z = dd.z;
    this->p.x = pp.x;
    this->p.y = pp.y;
    this->p.z = pp.z;
  };

  int test_with(const vector<Shape*>& shapes,
                vector<Intersection>& is,
                double t0, double t1) const;
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

