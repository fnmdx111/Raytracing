#include <cmath>
#include <sstream>
#include "float3.hh"

using namespace std;

#define SQ(x) ((x) * (x))

bool
float3::operator ==(const float3& o) const
{
  if (FEQ(this->x, o.x)
      && FEQ(this->y, o.y)
      && FEQ(this->z, o.z)) {
    return true;
  } else {
    return false;
  }
}

float3
float3::operator +(const float3& o) const
{
  return float3(this->x + o.x,
                this->y + o.y,
                this->z + o.z);
}

float3
float3::operator -(const float3& o) const
{
  return float3(this->x - o.x,
                this->y - o.y,
                this->z - o.z);
}

float3
float3::operator *(const float3& o) const
{
  double u1 = this->x, u2 = this->y, u3 = this->z;
  double v1 = o.x, v2 = o.y, v3 = o.z;

  return float3(u2 * v3 - u3 * v2,
                u3 * v1 - u1 * v3,
                u1 * v2 - u2 * v1);
}

float3&
float3::operator *=(const float3& o)
{
  float3 z = *this * o;
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

double
float3::dot(const float3& o) const
{
  return x * o.x + y * o.y + z * o.z;
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
float3::operator *(double y) const
{
  float3 f(this->x * y, this->y * y, this->z * y);
  return f;
}

float3&
float3::operator *=(double y)
{
  this->x *= y;
  this->y *= y;
  this->z *= y;

  return *this;
}

double
float3::norm() const
{
  return sqrt(SQ(this->x) + SQ(this->y) + SQ(this->z));
}

bool
float3::is_zero() const
{
  if (FEQ(x, 0.0) && FEQ(y, 0.0) && FEQ(z, 0.0)) {
    return true;
  }
  return false;
}

float3
float3::normalize() const
{
  double t = this->norm();

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

double
float3::sq_dist(const float3& o) const
{
  return SQ(x - o.x) + SQ(y - o.y) + SQ(z - o.z);
}

float3
float3::pll_mul(const float3& o) const
{
  return float3(this->x * o.x, this->y * o.y, this->z * o.z);
}

string
float3::to_s() const
{
  ostringstream out;
  out << "fl3(" << this->x << ", " << this->y << ", " << this->z << ")";
  return out.str();
}

float3
bisector(const float3& v, const float3& l)
{
  return (v + l).normalize();
}

#define FEPSILON 1e-3
bool
float3::is_epsilon() const
{
  return FEQ(x, FEPSILON) && FEQ(y, FEPSILON) && FEQ(z, FEPSILON);
}

bool
float3::is_nan() const
{
  return std::isnan(x) || std::isnan(y) || std::isnan(z);
}
