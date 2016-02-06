#ifndef __H_FLOAT3__
#define __H_FLOAT3__

#include <string>

struct float3
{
  float x;
  float y;
  float z;
  float3(float x, float y, float z) : x(x), y(y), z(z) {}

  float3 operator +(const float3& y);
  float3 operator -(const float3& y);
  float3 operator *(const float3& y);
  float3 operator *(float y);
  float3& operator +=(const float3& y);
  float3& operator -=(const float3& y);
  float3& operator *=(const float3& y);
  float3& operator *=(float y);
  float dot(const float3& y);
  float3& operator =(const float3& y);

  float3& normalize_();
  float3 normalize();
  float norm();
  float3& negate();

  std::string to_s();
};

#endif

