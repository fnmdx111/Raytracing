#ifndef __H_SHAPES__
#define __H_SHAPES__

#include "float3.hh"
#include "objs.hh"

class SSphere : public Shape
{
public:
  float3 pos;
  float r;

  SSphere(float x, float y, float z, float r): pos(float3(x, y, z)), r(r) {}
};

class STriangle : public Shape
{
public:
  float3 pos1, pos2, pos3;

  STriangle(float x1, float y1, float z1,
	    float x2, float y2, float z2,
	    float x3, float y3, float z3):
    pos1(float3(x1, y1, z1)),
    pos2(float3(x2, y2, z2)),
    pos3(float3(x3, y3, z3)) {}
};

class SPlane : public Shape
{
public:
  float3 n;
  float d;

  SPlane(float nx, float ny, float nz, float d):
    n(float3(nx, ny, nz)), d(d) {}
};

#endif
