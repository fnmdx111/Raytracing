#include "lights.hh"
#include <cmath>

using namespace std;

float3
LPoint::l(const Intersection& in) const
{
  return (this->pos - in.p).normalize();
}

float3
LDirectional::l(const Intersection& in) const
{
  return float3(nanf(""), nanf(""), nanf(""));
}

float3
LAmbient::l(const Intersection& _) const
{
  return float3(nanf(""), nanf(""), nanf(""));
}

