#ifndef __H_FLOAT3__
#define __H_FLOAT3__

#include <string>
#include <cmath>

struct float3
{
  double x;
  double y;
  double z;
  float3(double x, double y, double z): x(x), y(y), z(z) {}
  float3(): x(0.), y(0.), z(0.) {}

  float3 operator +(const float3& y) const;
  float3 operator -(const float3& y) const;
  float3 operator *(const float3& y) const;
  float3 operator *(double y) const;
  float3& operator +=(const float3& y);
  float3& operator -=(const float3& y);
  float3& operator *=(const float3& y);
  float3& operator *=(double y);
  double dot(const float3& y) const;
  float3& operator =(const float3& y);

  float3& normalize_();
  float3 normalize() const;
  double norm() const;
  float3& negate();

  float3 pll_mul(const float3& y) const;

  double sq_dist(const float3& y) const;

  std::string to_s() const;
};

#define SQ(x) ((x) * (x))
#define FEQ(x, y) (std::fabs((x) - (y)) < 1e-10)

float3 bisector(const float3& v, const float3& l);

#endif

