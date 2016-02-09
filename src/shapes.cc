#include "float3.hh"
#include "shapes.hh"
#include <cmath>
#include <vector>

using namespace std;

int
SPlane::test_with(const Ray& r,
		  vector<Intersection>& v) const
{
  return 0;
}

int
STriangle::test_with(const Ray& r,
		     vector<Intersection>& v) const
{
  return 0;
}

int
SSphere::test_with(const Ray& r,
		   vector<Intersection>& v) const
{
  float3 delta = r.p - this->pos;

  float ddotd = r.d.dot(r.d);
  float ddotdd = r.d.dot(delta);

  float poly1 = -1 * ddotdd;
  float poly2 = SQ(ddotdd) - ddotd * (delta.dot(delta) - SQ(this->r));

  if (poly2 < 0.) {
    return 0;

  } else if (FEQ(poly2, 0.0)) {
    float t = poly1 / ddotd;
    float3 pos = r.p + r.d * t;
    v.push_back(Intersection(t, SQ(t),
			     pos,
			     (pos - this->pos) * (1 / this->r),
			     this));

    return 1;

  } else {
    poly2 = -sqrt(poly2);
  REPEAT:
    float t = (poly1 + poly2) / ddotd;
    float3 pos = r.p + r.d * t;
    v.push_back(Intersection(t, SQ(t),
			     pos,
			     (pos - this->pos) * (1 / this->r),
			     this));
    if (poly2 > 0) {
      return 2;
    }

    poly2 = -poly2;
    goto REPEAT;
  }
}
