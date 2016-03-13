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
  ~Material() {}

  void diffuse(float3& a, const float3& l, const float3& n, const float3& d,
               const float3& clr) const;
  void specular(float3& a, const float3& l, const float3& n, const float3& d,
                const float3& clr) const;
  void ambient(float3& a, const Light& lgh) const;
};

#include "lights.hh"

class Ray;

class AABoundingBox {
public:
  double xmin, xmax;
  double ymin, ymax;
  double zmin, zmax;
  AABoundingBox(double xmi, double xma,
                double ymi, double yma,
                double zmi, double zma): xmin(xmi), xmax(xma),
  ymin(ymi), ymax(yma), zmin(zmi), zmax(zma) {}

  AABoundingBox()
  {
    xmax = ymax = zmax = numeric_limits<double>::max();
    xmin = ymin = zmin = -numeric_limits<double>::max();
  }

  void copy_from(const AABoundingBox& other)
  {
    xmin = other.xmin;
    xmax = other.xmax;
    ymin = other.ymin;
    ymax = other.ymax;
    zmin = other.zmin;
    zmax = other.zmax;
  }

  AABoundingBox operator +(const AABoundingBox& other)
  {
    return AABoundingBox(min(xmin, other.xmin), max(xmax, other.xmax),
                         min(ymin, other.ymin), max(ymax, other.ymax),
                         min(zmin, other.zmin), max(zmax, other.zmax));
  }

  bool test_with(const Ray& r,
                 Intersection& v,
                 double t0, double t1) const;
};

enum ShapeType { sphere, triangle, plane, bvhnode };

class Shape
{
  const static string __sphere;
  const static string __triangle;
  const static string __plane;
  const static string __bvhnode;
  const static string __unknown;

public:
  Shape() {}
  virtual ~Shape() {}

  AABoundingBox aabb;

  const Material* mat;

  void
  set_material(const Material* material)
  {
    mat = material;
  }

  virtual bool test_with(const Ray& r,
                         Intersection& v,
                         double t0, double t1) const = 0;
  virtual ShapeType type() const = 0;
  static const
  string& to_s(ShapeType s)
  {
    if (s == ShapeType::sphere) {
      return Shape::__sphere;
    } else if (s == ShapeType::triangle) {
      return Shape::__triangle;
    } else if (s == ShapeType::plane) {
      return Shape::__plane;
    } else if (s == ShapeType::bvhnode) {
      return Shape::__bvhnode;
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
  }
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
  Intersection(): t(0.0), t2(0.0), p(float3()), n(float3()), sp(0) {}
  Intersection& operator =(const Intersection& i);
};

#endif

