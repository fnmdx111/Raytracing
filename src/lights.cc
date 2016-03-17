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

float3
LArea::l(const Intersection& in) const
{
  return pos - in.p;
}

float3
LArea::l(const Intersection& in, double su, double sv) const
{
  float3 pos = this->pos +
               this->u * (su - 0.5) * len +
               this->v * (sv - 0.5) * len;
  return pos - in.p;
}

LightType
LArea::type() const
{
  return LightType::area;
}

double
LArea::dist(const Intersection& in) const
{
  return nanf("");
}
