#ifndef __H_FLOAT3__
#define __H_FLOAT3__

#include <string>

struct float3
{
  float x;
  float y;
  float z;
  float3(float x, float y, float z) : x(x), y(y), z(z) {}

  float3 operator +(const float3& y) const;
  float3 operator -(const float3& y) const;
  float3 operator *(const float3& y) const;
  float3 operator *(float y) const;
  float3& operator +=(const float3& y);
  float3& operator -=(const float3& y);
  float3& operator *=(const float3& y);
  float3& operator *=(float y);
  float dot(const float3& y) const;
  float3& operator =(const float3& y);

  float3& normalize_();
  float3 normalize() const;
  float norm() const;
  float3& negate();

  std::string to_s() const;
};

#define SQ(x) ((x) * (x))
#define FEQ(x, y) ((x) - (y) < 1e-10)

#endif

