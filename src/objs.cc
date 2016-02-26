#include "objs.hh"
#include <vector>

using namespace std;

const string Shape::__sphere = "sphere";
const string Shape::__triangle = "triangle";
const string Shape::__plane = "plane";
const string Shape::__unknown = "unknown";

float3
Material::ambient(const Light& lgh) const
{
  return this->d.pll_mul(lgh.clr);
}

float3
Material::diffuse(const Light& lgh, const Intersection& in) const
{
  double d = in.n.dot(lgh.l(in));
  d = max(0., d);

  return this->d.pll_mul(lgh.clr) * d;
}

float3
Material::specular(const Light& lgh, const Intersection& in,
                   const float3& v) const
{
  double d = in.n.dot(bisector(v, lgh.l(in)));
  d = max(0., d);
  d = pow(d, this->r);

  return this->s.pll_mul(lgh.clr) * d;
}

int
Ray::test_with(const vector<Shape*>& shapes,
               vector<Intersection>& is,
               double t0, double t1) const
{
  int count = 0;

  for (size_t i = 0; i < shapes.size(); ++i) {
    count += shapes[i]->test_with(*this, is, t0, t1);
  }

  return count;
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
