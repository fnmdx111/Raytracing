#include <cmath>
#include <sstream>
#include "float3.hh"

using namespace std;

float3
float3::operator +(const float3& y)
{
  return float3(this->x + y.x,
		this->y + y.y,
		this->z + y.z);
}

float3
float3::operator -(const float3& y)
{
  return float3(this->x - y.x,
		this->y - y.y,
		this->z - y.z);
}

float3
float3::operator *(const float3& y)
{
  float u1 = this->x, u2 = this->y, u3 = this->z;
  float v1 = y.x, v2 = y.y, v3 = y.z;
    
  return float3(u2 * v3 - u3 * v2,
		u3 * v1 - u1 * v3,
		u1 * v2 - u2 * v1);
}

float3&
float3::operator *=(const float3& y)
{
  float3 z = *this * y;
  this->x = z.x;
  this->y = z.y;
  this->z = z.z;
  return *this;
}

float3&
float3::operator +=(const float3& y)
{
  float3 z = *this + y;
  this->x = z.x;
  this->y = z.y;
  this->z = z.z;
  return *this;
}

float3&
float3::operator -=(const float3& y)
{
  float3 z = *this - y;
  this->x = z.x;
  this->y = z.y;
  this->z = z.z;
  return *this;
}

float
float3::dot(const float3& y)
{
  return this->x * y.x + this->y * y.y + this->z * y.z;
}

float3&
float3::operator =(const float3& y)
{
  this->x = y.x;
  this->y = y.y;
  this->z = y.z;

  return *this;
}

float3
float3::operator *(float y)
{
  float3 f(this->x * y, this->y * y, this->z * y);
  return f;
}

float3&
float3::operator *=(float y)
{
  this->x *= y;
  this->y *= y;
  this->z *= z;

  return *this;
}

float
float3::norm()
{
#define SQ(x) ((x) * (x))
  return sqrt(SQ(this->x) + SQ(this->y) +SQ(this->z));
}

float3
float3::normalize()
{
#define SQ(x) ((x) * (x))
  float t = this->norm();

  float3 f(this->x, this->y, this->z);
  f.x /= t;
  f.y /= t;
  f.z /= t;

  return f;
}

float3&
float3::normalize_()
{
  return *this = this->normalize();
}

float3&
float3::negate()
{
  *this *= -1;
  return *this;
}

string
float3::to_s()
{
  ostringstream out;
  out << "fl3(" << this->x << ", " << this->y << ", " << this->z << ")";
  return out.str();
}

