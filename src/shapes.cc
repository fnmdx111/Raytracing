#include "float3.hh"
#include "shapes.hh"
#include <cmath>
#include <vector>


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

int
SPlane::test_with(const Ray& r,
                  vector<Intersection>& v,
                  double t0, double t1) const
{
  double denom = r.d.dot(this->n);
  if (FEQ(denom, 0.0)) {
    return 0;
  }

  float3 nn = this->n;
  if (denom > 0.0) {
    nn *= -1;
    denom *= -1;
  }

  double t = (r.p.dot(nn) + this->d) / -denom;
  if (t < t0 || t > t1) {
    return 0;
  }

  float3 p = r.p + r.d * t;

  v.push_back(Intersection(t, SQ(t),
                           p, nn, this));
  return 1;
}

int
STriangle::test_with(const Ray& r,
                     vector<Intersection>& v,
                     double t0, double t1) const
{
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
    return 0;
  }

  DEF(C, (i * akmjb + h * jcmal + g * blmkc) / M);
  if (C < 0 || C > 1) {
    return 0;
  }

  DEF(B, (j * eimhf + k * gfmdi + l * dhmeg) / M);
  if (B < 0 || B > 1 - C) {
    return 0;
  }

  float3 p = r.p + r.d * t;
  v.push_back(Intersection(t, SQ(t), p, this->n, this));

  return 1;
}

int
SSphere::test_with(const Ray& r,
                   vector<Intersection>& v,
                   double t0, double t1) const
{
  float3 delta = r.p - this->pos;

  double ddotd = r.d.dot(r.d);
  double ddotdd = r.d.dot(delta);

  double poly1 = -1 * ddotdd;
  double poly2 = SQ(ddotdd) - ddotd * (delta.dot(delta) - SQ(this->r));
  if (poly2 < 0.) {
    return 0;

  } else if (FEQ(poly2, 0.0)) {
    double t = poly1 / ddotd;
    if (t < t0 || t > t1) {
      return 0;
    }

    float3 pos = r.p + r.d * t;
    v.push_back(Intersection(t, SQ(t),
                             pos,
                             (pos - this->pos) * (1 / this->r),
                             this));

    return 1;

  } else {
    poly2 = -sqrt(poly2);
    int count = 0;
    REPEAT:
    double t = (poly1 + poly2) / ddotd;

    if (t < t0 || t > t1) {
      return count;
    }
    ++count;

    float3 pos = r.p + r.d * t;
    v.push_back(Intersection(t, SQ(t),
                             pos,
                             (pos - this->pos) * (1 / this->r),
                             this));
    if (poly2 > 0) {
      return count;
    }

    poly2 = -poly2;
    goto REPEAT;
  }
}
