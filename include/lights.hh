#ifndef __H_LIGHTS__
#define __H_LIGHTS__

#include "float3.hh"
#include "objs.hh"
#include <random>

enum LightType { point, directional, ambient, area };

class Light
{
public:

  float3 clr;
  virtual ~Light() {}
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

class LArea : public Light
{
public:
  float3 pos, n, u, v;
  double len;

  std::random_device rd;
  std::mt19937 e2;
  std::uniform_real_distribution<> dst;

  LArea(double px, double py, double pz, double nx, double ny, double nz,
        double ux, double uy, double uz, double len, double r, double b,
        double g): Light(r, g, b), pos(float3(px, py, pz)), n(nx, ny, nz),
  u(ux, uy, uz), len(len),
  e2(std::mt19937(rd())),
  dst(std::uniform_real_distribution<>(0.0, 0.99999999))
  {
    v = u * n;
    v.normalize_();
  }

  float3 l(const Intersection& in) const;
  LightType type() const;
  double dist(const Intersection& in) const;
  float3 l(const Intersection& in, double su, double sv) const;
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

