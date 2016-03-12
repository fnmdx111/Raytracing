#include "objs.hh"
#include <vector>
#include <cassert>

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

extern int opmode;
static const float3 axis_x = float3(1.0, 0.0, 0.0);
static const float3 axis_y = float3(0.0, 1.0, 0.0);
static const float3 axis_z = float3(0.0, 0.0, 1.0);

bool
AABoundingBox::test_with(const Ray& r, Intersection& v,
                         double t0, double t1) const
{
  double tnear = t0;
  double tfar = numeric_limits<double>::max();

  char which_normal = 'a';

  double rep_dx = 1 / r.d.x;

#define SWAP(sw1, sw2) do {\
          auto temp = sw1;\
          sw1 = sw2;\
          sw2 = temp;\
        } while (0)

  double txmin = rep_dx * (xmin - r.p.x);
  double txmax = rep_dx * (xmax - r.p.x);
  if (rep_dx < 0) {
    SWAP(txmin, txmax);
  }
  if (tnear < txmin) {
    tnear = txmin;
    which_normal = 'x';
  }
  tfar = min(tfar, txmax);
  if (tnear > tfar) {
    return false;
  }

  double rep_dy = 1 / r.d.y;
  double tymin = rep_dy * (ymin - r.p.y);
  double tymax = rep_dy * (ymax - r.p.y);
  if (rep_dy < 0) {
    SWAP(tymin, tymax);
  }
  if (tnear < tymin) {
    tnear = tymin;
    which_normal = 'y';
  }
  tfar = min(tfar, tymax);
  if (tnear > tfar) {
    return false;
  }

  double rep_dz = 1 / r.d.z;
  double tzmin = rep_dz * (zmin - r.p.z);
  double tzmax = rep_dz * (zmax - r.p.z);
  if (rep_dz < 0) {
    SWAP(tzmin, tzmax);
  }
  if (tnear < tzmin) {
    tnear = tzmin;
    which_normal = 'z';
  }
  tfar = min(tfar, tzmax);
  if (tnear > tfar) {
    return false;
  }

#define UPDATE_INTERSECTION do {\
          v.t = tnear;\
          v.t2 = SQ(tnear);\
          v.p = r.p + r.d * tnear;\
        } while (0)
  if (opmode == 1 || opmode == 2) {
    switch (which_normal) {
    case 'x':
      v.n = axis_x;
      break;

    case 'y':
      v.n = axis_y;
      break;

    case 'z':
      v.n = axis_z;
      break;

    default:
      break;
    }

    UPDATE_INTERSECTION;
  }

  return true;
}
