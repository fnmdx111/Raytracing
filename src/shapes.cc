#include "float3.hh"
#include "shapes.hh"
#include <cmath>

extern int opmode;

using namespace std;

ShapeType
SPlane::type() const
{
  return ShapeType::plane;
}

ShapeType
STriangle::type() const
{
  return ShapeType::triangle;
}

ShapeType
SSphere::type() const
{
  return ShapeType::sphere;
}

bool
SPlane::test_with(const Ray& r,
                  Intersection& v,
                  double t0, double t1) const
{
  double denom = r.d.dot(this->n);
  if (FEQ(denom, 0.0)) {
    return false;
  }

  double t = (r.p.dot(this->n) + this->d) / -denom;
  if (t <= 0.) {
    return false;
  }

  if (t < t0 || t > t1) {
    return false;
  }

  float3 p = r.p + r.d * t;

  v = Intersection(t, SQ(t), p, this->n, this);

  return true;
}

bool
STriangle::test_with(const Ray& r,
                     Intersection& v,
                     double t0, double t1) const
{
  if (opmode == 1 || opmode == 2) {
    bool intersected = aabb.test_with(r, v, t0, t1);

    if (intersected) {
      v.sp = this;
      return true;
    }
    return false;
  } else {
    bool intersected = aabb.test_with(r, v, t0, t1);

    if (!intersected) {
      return false;
    }
  }

#define DEF(x, z) const double x = (z)
  DEF(a, pos1.x - pos2.x);
  DEF(b, pos1.y - pos2.y);
  DEF(c, pos1.z - pos2.z);
  DEF(d, pos1.x - pos3.x);
  DEF(e, pos1.y - pos3.y);
  DEF(f, pos1.z - pos3.z);
  DEF(g, r.d.x);
  DEF(h, r.d.y);
  DEF(i, r.d.z);
  DEF(j, pos1.x - r.p.x);
  DEF(k, pos1.y - r.p.y);
  DEF(l, pos1.z - r.p.z);

  DEF(eimhf, e * i - h * f);
  DEF(gfmdi, g * f - d * i);
  DEF(dhmeg, d * h - e * g);
  DEF(akmjb, a * k - j * b);
  DEF(jcmal, j * c - a * l);
  DEF(blmkc, b * l - k * c);

  DEF(M, a * eimhf + b * gfmdi + c * dhmeg);

  DEF(t, -(f * akmjb + e * jcmal + d * blmkc) / M);
  if (t < t0 || t > t1) {
    return false;
  }

  DEF(C, (i * akmjb + h * jcmal + g * blmkc) / M);
  if (C < 0 || C > 1) {
    return false;
  }

  DEF(B, (j * eimhf + k * gfmdi + l * dhmeg) / M);
  if (B < 0 || B > 1 - C) {
    return false;
  }

  float3 p = r.p + r.d * t;
  v = Intersection(t, SQ(t), p, this->n, this);

  return true;
}

bool
SSphere::test_with(const Ray& r,
                   Intersection& v,
                   double t0, double t1) const
{
  if (opmode == 1 || opmode == 2) {
    bool intersected = aabb.test_with(r, v, t0, t1);

    if (intersected) {
      v.sp = this;
      return true;
    }
    return false;
  } else {
    bool intersected = aabb.test_with(r, v, t0, t1);

    if (!intersected) {
      return false;
    }
  }

  float3 emc = r.p - this->pos;

  double ddotd = r.d.dot(r.d);
  double ddotdd = r.d.dot(emc);

  double poly1 = -1 * ddotdd;
  double poly2 = SQ(ddotdd) - ddotd * (emc.dot(emc) - SQ(this->r));
  if (poly2 < 0.) {
    return false;

  } else {
    double sqrt_poly2 = sqrt(poly2);
    double t = (poly1 + sqrt_poly2) / ddotd;
    if (t < t0 && t > t1) {
      t = numeric_limits<double>::max();
    }

    if (sqrt_poly2 > 0.) {
      double tt = (poly1 - sqrt_poly2) / ddotd;
      if (tt > t0 && tt < t1) {
        if (tt < t) {
          t = tt;
        }
      }
    }

    if (t < t0 || t > t1) {
      return false;
    }

    float3 pos = r.p + r.d * t;
    v = Intersection(t, SQ(t), pos,
                     (pos - this->pos) * (1 / this->r),
                     this);

    return true;
  }
}
