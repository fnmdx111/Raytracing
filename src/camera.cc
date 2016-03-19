#include "hw0.hh"
#include "objs.hh"
#include "lights.hh"
#include <limits>
#include "camera.hh"
#include <chrono>
#include <random>
#include "shapes.hh"
#include <cassert>

#ifdef USE_TBB
#include "tbb/tbb.h"
#endif

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

Ray
Camera::ray(double i, double j, double& dn)
{
  double su = this->l + (this->r - this->l) * i / this->pw;
  double sv = this->t + (this->b - this->t) * j / this->ph;

  float3 dir = this->dir * this->d + this->u * su + this->v * sv;
  dn = dir.norm();
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
  copy_prop(lens);
  copy_prop(aperture_size);

  pixels.resizeErase(ph, pw);
}

Camera::Camera(const Camera& other)
{
  copy(other);
}

void
do_shadow(Camera& cam, float3& accum, const Ray& r, const Light& lgh,
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

extern int NSAMPLE;
extern int SHDNSAMPLE;
#ifdef FEAT_GLOSSY
extern int GLSNSAMPLE;
#endif
#ifdef FEAT_DOF
extern int DOFNSAMPLE;
#endif

void
Camera::ray_color(int recursion_depth,
                  float3& accum,
                  const Ray& r,
                  const Light* shadow_lgh,
                  double t0, double t1)
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

      float3 a;
      // if SHDNSAMPLE is odd, we need to move left 0.5
      //   0      1     2     3
      //   -2     -1    0     1
      // |  *  |  *  |  *  |  *  |
      // |     |    pos    |     |
      // |^-2+r|^-1+r| ^0+r| ^1+r|
      //   0     1      2     3     4
      //   -2    -1     0     1     2
      // |  *  |  *  |  *  |  *  |  *  |
      // |     |     | pos |     |     |
      // | ^-2-0.5+r   ^-0.5+r
      //   -2.5  -1.5   -0.5  0.5   1.5
      for (int rdi = 0; rdi < SHDNSAMPLE; ++rdi) {
        for (int rdj = 0; rdj < SHDNSAMPLE; ++rdj) {
          double ru = la->dst(la->e2) + rdi + la->hl;
          double rv = la->dst(la->e2) + rdj + la->hl;
          float3 l = la->l(in, ru, rv);

          float3 shd_accum;
          do_shadow(*this, shd_accum, r, lgh, l, in);
          a += shd_accum; // * in.n.dot(l);
        }
      }

      accum += a * (1. / SQ(SHDNSAMPLE));

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

#ifdef FEAT_GLOSSY
    if (!FEQ(mat.a, 0.0)) {
      double min_rf = min(rf.x, min(rf.y, rf.z));

      float3 t(rf);
      if (FEQ(min_rf, rf.x)) {
        t.x = 1;
      } else if (FEQ(min_rf, rf.y)) {
        t.y = 1;
      } else if (FEQ(min_rf, rf.z)) {
        t.z = 1;
      }

      float3 nu = t * rf;
      nu.normalize_();
      float3 nv = rf * nu;

      float3 glossy_accum;
      for (int _ = 0; _ < GLSNSAMPLE; ++_) {
        float3 nrf = rf + (nu * (dst(e2) - 0.5) + nv * (dst(e2) - 0.5)) * mat.a;
        nrf.normalize_();
        Ray rr(nrf, in.p);

        float3 t_accum;
        ray_color(recursion_depth + 1,
                  t_accum,
                  rr, 0,
                  SEPSILON,
                  numeric_limits<double>::max());
        glossy_accum += mat.i.pll_mul(t_accum);
      }

      accum += glossy_accum * (1. / GLSNSAMPLE);

    } else {
#endif

      Ray rr(rf, in.p);

      float3 r_accum(0., 0., 0.);
      ray_color(recursion_depth + 1,
                r_accum,
                rr, 0,
                SEPSILON,
                numeric_limits<double>::max());
      accum += mat.i.pll_mul(r_accum);
#ifdef FEAT_GLOSSY
    }
#endif
  }
}


void
Camera::render()
{
#define PROGRESS_SAMPLE_RATE 700

  cout << endl
  << "Renderer parameters:" << endl
  << "\tRendering with BVH tree = " << "Yes" << endl
  << "\tSoft shadow = " << "Yes if an area light is included in the scene"
                        << endl
#ifdef FEAT_GLOSSY
  << "\tGlossy reflection = "
  << "Yes if a glossy material is included in the scene" << endl
#endif
  << "\tProgress = " << "Yes" << endl
#ifdef FEAT_DOF
  << "\tDepth of field = " << "Yes if the camera is set with related parameters"
                        << endl
#endif
  << endl
  << "\tMaximal recursion limit = " << MAXRECUR << endl
  << "\tEpsilon = " << SEPSILON << endl
  << "\tPrimary rays per pixel = " << SQ(NSAMPLE) << endl
  << "\tShadow rays per primary ray = " << SQ(SHDNSAMPLE) << endl
#ifdef FEAT_GLOSSY
  << "\tGlossy rays per primary ray = " << GLSNSAMPLE << endl
#endif
  << "\tProgress sample rate = every " << PROGRESS_SAMPLE_RATE << " pixels"
  << endl
#ifdef FEAT_DOF
  << "\tDoF rays per primary ray = " << DOFNSAMPLE << endl
  << "\tAperture size = " << aperture_size << endl
  << "\tFocal depth = " << lens << endl
#endif
  ;
  auto begin = chrono::steady_clock::now();

  int pixel_cnt = 0;
  double total_cnt = ph * pw / 100.0;

  cout << endl
  << "Scene parameters:" << endl
  << "\tTotal pixels = " << ph * pw << endl << endl;

  printf("Shyoujyou rendering...\n");

#ifdef USE_TBB
  tbb::parallel_for(0, ph, [&](int y) {
    tbb::parallel_for(0, pw, [&](int x) {
#else
  for (int y = 0; y < ph; ++y) {
    for (int x = 0; x < pw; ++x) {
#endif
      ++pixel_cnt;
      if (pixel_cnt % PROGRESS_SAMPLE_RATE == 0) {
        auto time_elapsed = chrono::duration_cast<chrono::seconds>(
                    chrono::steady_clock::now() - begin).count();
        time_elapsed = FEQ(time_elapsed, 0.0) ? 1.0 : time_elapsed;

        printf(
#if defined(__clang__)
               "Rendered % 2.2lf%% in %lld seconds, % 2.2lld pixels/s...\r",
#else
               "Rendered % 2.2lf%% in %ld seconds, % 2.2ld pixels/s...\r",
#endif
               pixel_cnt / total_cnt,
               time_elapsed,
               pixel_cnt / time_elapsed);
        fflush(stdout);
      }

      float3 clr(0., 0., 0.);
      for (int p = 0; p < NSAMPLE; ++p) {
        for (int q = 0; q < NSAMPLE; ++q) {
          Ray r = ray(x + (p + dst(e2)) / NSAMPLE,
                      y + (q + dst(e2)) / NSAMPLE);

#ifdef FEAT_DOF
          float3 sub;
          for (int o = 0; o < DOFNSAMPLE; ++o) {
            float3 rand_point = this->u * (dst(e2) - 0.5)
                                + this->v * (dst(e2) - 0.5);
            rand_point *= this->aperture_size;

            r.d *= this->lens;
            r.d -= rand_point;
            r.d.normalize_();

            r.p += rand_point;

            ray_color(0, sub, r, 0,
                      0., numeric_limits<double>::max());
          }
          sub *= 1. / DOFNSAMPLE;
          clr += sub;
#else
          ray_color(0, clr, r, 0,
                    0., numeric_limits<double>::max());
#endif
        }
      }

      set_pixel(x, y, clr * (1. / SQ(NSAMPLE)));
#ifdef USE_TBB
    });
  });
#else
    }
  }
#endif

  auto end = chrono::steady_clock::now();

  printf(
#if defined(__clang__)
         "\nDone in %lld seconds.\n",
#else
         "\nDone in %ld seconds.\n",
#endif
         chrono::duration_cast<chrono::seconds>(end - begin).count());
}

void
Camera::save(const string& fn) const
{
  write_rgba(fn, &this->pixels[0][0], this->pw, this->ph);
}
