#include "lights.hh"
#include <cmath>

using namespace std;

float3
LPoint::l(const Intersection& in) const
{
  return this->pos - in.p;
}

float3
LDirectional::l(const Intersection& _) const
{
  return nv;
}

float3
LAmbient::l(const Intersection& _) const
{
  return float3(nanf(""), nanf(""), nanf(""));
}

LightType
LPoint::type() const
{
  return LightType::point;
}

LightType
LDirectional::type() const
{
  return LightType::directional;
}

LightType
LAmbient::type() const
{
  return LightType::ambient;
}

double
LPoint::dist(const Intersection& in) const
{
  return sqrt(pos.sq_dist(in.p));
}
