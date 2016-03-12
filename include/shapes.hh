#ifndef __H_SHAPES__
#define __H_SHAPES__

#include "float3.hh"
#include "objs.hh"

#define SEPSILON 1e-7

class SSphere : public Shape
{
public:
  float3 pos;
  double r;

  SSphere(double x, double y, double z, double r): pos(float3(x, y, z)), r(r)
  {
    aabb.xmin = pos.x - r;
    aabb.xmax = pos.x + r;
    aabb.ymin = pos.y - r;
    aabb.ymax = pos.y + r;
    aabb.zmin = pos.z - r;
    aabb.zmax = pos.z + r;
  }

  bool test_with(const Ray& r,
                Intersection& v,
                double t0, double t1) const;
  ShapeType type() const;
};

class STriangle : public Shape
{
public:
  float3 pos1, pos2, pos3;
  float3 n;

  STriangle(double x1, double y1, double z1,
	    double x2, double y2, double z2,
	    double x3, double y3, double z3):
    pos1(float3(x1, y1, z1)),
    pos2(float3(x2, y2, z2)),
    pos3(float3(x3, y3, z3))
  {
    n = ((pos1 - pos2) * (pos1 - pos3)).normalize();

    aabb.xmin = min(pos1.x, min(pos2.x, pos3.x));
    aabb.xmax = max(pos1.x, max(pos2.x, pos3.x));
    aabb.ymin = min(pos1.y, min(pos2.y, pos3.y));
    aabb.ymax = max(pos1.y, max(pos2.y, pos3.y));
    aabb.zmin = min(pos1.z, min(pos2.z, pos3.z));
    aabb.zmax = max(pos1.z, max(pos2.z, pos3.z));
  }

  bool test_with(const Ray& r,
                Intersection& v,
                double t0, double t1) const;
  ShapeType type() const;
};

class SPlane : public Shape
{
public:
  float3 n;
  double d;

  SPlane(double nx, double ny, double nz, double d):
    n(float3(nx, ny, nz)), d(d) {}

  bool test_with(const Ray& r,
                Intersection& v,
                double t0, double t1) const;
  ShapeType type() const;
};

#endif
