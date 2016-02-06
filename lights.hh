#ifndef __H_LIGHTS__
#define __H_LIGHTS__

#include "float3.hh"

class Light {};

class LPoint : public Light
{
public:
  float3 pos, clr;
  LPoint(float x, float y, float z, float r, float g, float b):
    pos(float3(x, y, z)), clr(float3(r, g, b)) {}
};

class LDirectional : public Light
{
public:
  float3 v, clr;
  LDirectional(float vx, float vy, float vz, float r, float g, float b):
    v(float3(vx, vy, vz)),
    clr(float3(r, g, b)) {}
};

class LAmbient : public Light
{
public:
  float3 clr;
  LAmbient(float r, float g, float b): clr(float3(r, g, b)) {}
};

#endif
