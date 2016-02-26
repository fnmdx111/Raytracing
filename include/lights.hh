#ifndef __H_LIGHTS__
#define __H_LIGHTS__

#include "float3.hh"
#include "objs.hh"

enum LightType { point, directional, ambient };

class Light
{
public:

  float3 clr;
  virtual float3 l(const Intersection& in) const = 0;
  virtual LightType type() const = 0;
  virtual double dist(const Intersection& in) const = 0;
  Light(double r, double g, double b): clr(float3(r, g, b)) {}
};

class LPoint : public Light
{
public:
  float3 pos;
  LPoint(double x, double y, double z, double r, double g, double b):
    Light(r, g, b),
    pos(float3(x, y, z)) {}
  float3 l(const Intersection& in) const;
  LightType type() const;
  double dist(const Intersection& in) const;
};

class LDirectional : public Light
{
public:
  float3 v, nv;
  LDirectional(double vx, double vy, double vz, double r, double g, double b):
    Light(r, g, b),
    v(float3(vx, vy, vz))
  {
    v.normalize_();
    nv = v * -1;
  }

  float3 l(const Intersection& in) const;
  LightType type() const;
  double dist(const Intersection& in) const
  {
    return numeric_limits<double>::max();
  }
};

class LAmbient : public Light
{
public:
  LAmbient(double r, double g, double b): Light(r, g, b) {}
  float3 l(const Intersection& in) const;
  LightType type() const;
  double dist(const Intersection& in) const
  {
    return numeric_limits<double>::max();
  }
};

#endif

