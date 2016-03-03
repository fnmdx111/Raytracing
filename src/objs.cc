#include "objs.hh"
#include <vector>

using namespace std;

const string Shape::__sphere = "sphere";
const string Shape::__triangle = "triangle";
const string Shape::__plane = "plane";
const string Shape::__unknown = "unknown";

void
Material::ambient(float3& a, const Light& lgh) const
{
  a += this->d.pll_mul(lgh.clr);
}

void
Material::diffuse(float3& a,
                  const float3& l,
                  const float3& n,
                  const float3& d,
                  const float3& clr) const
{
  double sc_d = n.dot(l);

  if (sc_d > 0.) {
    a += this->d.pll_mul(clr) * sc_d;
  }
}

void
Material::specular(float3& a,
                   const float3& l,
                   const float3& n,
                   const float3& d,
                   const float3& clr) const
{
  double sc_d = n.dot(bisector(d, l));

  if (sc_d > 0.) {
    sc_d = pow(sc_d, this->r);
    a += this->s.pll_mul(clr) * sc_d;
  }
}

Intersection&
Intersection::operator =(const Intersection& i)
{
  this->t = i.t;
  this->t2 = i.t2;
  this->sp = i.sp;
  this->p = i.p;
  this->n = i.n;

  return *this;
}
