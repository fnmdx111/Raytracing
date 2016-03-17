#include "hw0.hh"
#include "objs.hh"
#include "lights.hh"
#include <limits>
#include "camera.hh"
#include <chrono>
#include <random>
#include "shapes.hh"
#include <cassert>

using namespace std;

Ray
Camera::ray(double i, double j)
{
  double su = this->l + (this->r - this->l) * i / this->pw;
  double sv = this->t + (this->b - this->t) * j / this->ph;

  float3 dir = this->dir * this->d + this->u * su + this->v * sv;
  dir.normalize_();

  return Ray(dir.x, dir.y, dir.z,
             this->pos.x, this->pos.y, this->pos.z);
}

inline void
Camera::accum_pixel(int i, int j, const float3& rgba)
{
  int nj = j; // ph - j - 1;
  pixels[nj][i].r += rgba.x;
  pixels[nj][i].g += rgba.y;
  pixels[nj][i].b += rgba.z;
}

inline void
Camera::set_pixel(int i, int j, const float3& rgb)
{
  int nj = j; // ph - j - 1;
  pixels[nj][i].r = rgb.x;
  pixels[nj][i].g = rgb.y;
  pixels[nj][i].b = rgb.z;
  pixels[nj][i].a = 1.0;
}

void
Camera::copy(const Camera& other)
{
#define copy_prop(what) this->what = other.what
  copy_prop(pos);
  copy_prop(dir);
  copy_prop(u);
  copy_prop(v);
  copy_prop(w);
  copy_prop(d);
  copy_prop(iw);
  copy_prop(ih);
  copy_prop(pw);
  copy_prop(ph);
  copy_prop(r);
  copy_prop(l);
  copy_prop(t);
  copy_prop(b);

  pixels.resizeErase(ph, pw);
}

Camera::Camera(const Camera& other)
{
  copy(other);
}

void
do_shadow(const Camera& cam, float3& accum, const Ray& r, const Light& lgh,
          float3& l, const Intersection& in)
{

  const double shadow_t = l.norm();
  l.normalize_();
  Ray sr(l, in.p);

  sr.type = 's';

  float3 sclr(0., 0., 0.);
  cam.ray_color(0, sclr, sr, &lgh, SEPSILON, shadow_t);

  if (!sclr.is_zero()) {
    float3 nd = r.d * -1;
    in.sp->mat->diffuse(accum, l, in.n, nd, lgh.clr);

    in.sp->mat->specular(accum, l, in.n, nd, lgh.clr);
  }
}

extern int SHDNSAMPLE;
void
Camera::ray_color(int recursion_depth,
                  float3& accum,
                  const Ray& r,
                  const Light* shadow_lgh,
                  double t0, double t1) const
{
  if (recursion_depth > MAXRECUR) {
    return;
  }

  if (r.type == 's') { // is shadow ray
    assert(shadow_lgh != 0);

    Intersection ins;
    if (scene->root->test_with(r, ins, t0, t1)) {
      return;
    }

    accum += shadow_lgh->clr;
    return;
  }

  Intersection in;
  bool intersected = false;
  intersected = scene->root->test_with(r, in, t0, t1);

  if (!intersected) {
    return;
  }

  if (r.d.dot(in.n) > 0.) {
    in.n.negate();
  }

  const Material& mat = *(in.sp->mat);

  for (int k = 0; k < scene->lights.size(); ++k) {
    const Light& lgh = *(scene->lights[k]);

    if (lgh.type() == LightType::ambient) {
      mat.ambient(accum, lgh);
    } else if (lgh.type() == LightType::area) {
      LArea* la = dynamic_cast<LArea*>(scene->lights[k]);

      float3 ll = la->l(in);
      if (ll.dot(la->n) > 0.0) {
        // The normal of the area light is in the same direction of the ray from
        // intersection to the position of this area light.
        continue;
      }

      for (int rdi = 0; rdi < SQ(SHDNSAMPLE); ++rdi) {
        double ru = la->dst(la->e2);
        double rv = la->dst(la->e2);
        float3 l = la->l(in, ru, rv);

        float3 shd_accum;
        do_shadow(*this, shd_accum, r, lgh, l, in);
        shd_accum *= in.n.dot(l);
        accum += shd_accum * (1. / SQ(SHDNSAMPLE));
      }

    } else {
      float3 l = lgh.l(in);

      do_shadow(*this, accum, r, lgh, l, in);
    }
  }

  if (mat.i.is_zero()) {
    return;
  } else {
    float3 rf = r.d - in.n * r.d.dot(in.n) * 2;
    rf.normalize_();
    Ray rr(rf, in.p);

    float3 r_accum(0., 0., 0.);
    ray_color(recursion_depth + 1,
              r_accum,
              rr, 0,
              SEPSILON,
              numeric_limits<double>::max());
    accum += mat.i.pll_mul(r_accum);
  }
}

extern int NSAMPLE;

void
Camera::render()
{
  printf("Shyoujyou rendering...\n");

  auto begin = chrono::steady_clock::now();

  std::random_device rd;
  std::mt19937 e2(rd());
  std::uniform_real_distribution<> dist(0.0, 0.999999999);

  for (int y = 0; y < ph; ++y) {
    for (int x = 0; x < pw; ++x) {
      float3 clr(0., 0., 0.);
      for (int p = 0; p < NSAMPLE; ++p) {
        for (int q = 0; q < NSAMPLE; ++q) {
          Ray r = ray(x + (p + dist(e2)) / NSAMPLE,
                      y + (q + dist(e2)) / NSAMPLE);

          ray_color(0, clr, r, 0, 0., numeric_limits<double>::max());
        }
      }

      set_pixel(x, y, clr * (1. / SQ(NSAMPLE)));
    }
  }

  auto end = chrono::steady_clock::now();
#ifdef PROGRESS
  printf(".\n");
#endif
  printf(
#if defined(__clang__)
         "Done in %lld seconds.\n",
#else
         "Done in %ld seconds.\n",
#endif
         chrono::duration_cast<chrono::seconds>(end - begin).count());
}

void
Camera::save(const string& fn) const
{
  write_rgba(fn, &this->pixels[0][0], this->pw, this->ph);
}
