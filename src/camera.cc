#include "hw0.hh"
#include "objs.hh"
#include "lights.hh"
#include <limits>
#include "camera.hh"
#include <chrono>
#include <random>
#include "shapes.hh"
#include <cassert>
#include "scene.hh"

#ifdef USE_TBB
#include "tbb/tbb.h"
#ifdef PROGRESS
#include "tbb/concurrent_queue.h"
#include <thread>
#endif
#endif

#ifdef VISPROG
#include "tbb/concurrent_queue.h"
extern tbb::concurrent_bounded_queue<Renderlet*> rlet_queue;
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

#ifdef FEAT_REFRACT
bool
refract(const float3& dir, const float3& n, const double ni, float3& t)
{
  double ddotn = dir.dot(n);
  double discrim = (1.0 - SQ(ddotn)) / SQ(ni);
  if (1.0 >= discrim) {
    t = (dir - n * ddotn) * (1.0 / ni) - n * sqrt(1.0 - discrim);
    t.normalize_(); // TODO ??
    return true;
  }

  return false;
}
#endif

void
do_dirty(Camera& cam, float3& accum,
         const Ray& ori_r, const Ray& new_r,
         const Material& mat,
         int recursion_depth, const Intersection& in)
{
#ifdef FEAT_REFRACT
  if (mat.is_refractive) {
    double c = 0.0;
    float3 k;

    float3 t;
    if (ori_r.d.dot(in.n) < 0) {
      refract(ori_r.d, in.n, mat.ni, t);
      c = -ori_r.d.dot(in.n);

      k.x = k.y = k.z = 1.0;
    } else {
      k.x = exp(mat.l.x * in.t);
      k.y = exp(mat.l.y * in.t);
      k.z = exp(mat.l.z * in.t);
      if (refract(ori_r.d, in.n * -1, 1 / mat.ni, t)) {
        c = t.dot(in.n);

      } else {
        float3 k_accum;
        cam.ray_color(recursion_depth + 1, k_accum, new_r, 0,
                      SEPSILON, numeric_limits<double>::max());
        accum += k.pll_mul(k_accum);
        return;
      }
    }

    double rR0 = SQ(mat.ni - 1) / SQ(mat.ni + 1);
    double rR = rR0 + (1 - rR0) * pow(1 - c, 5);

    Ray tr(t, new_r.p);


    float3 ret_accum;
    float3 acc1;
    cam.ray_color(recursion_depth + 1, acc1, new_r, 0,
                  SEPSILON, numeric_limits<double>::max());
    ret_accum += acc1 * rR;

    cam.ray_color(recursion_depth + 1, acc1, tr, 0,
                  SEPSILON, numeric_limits<double>::max());
    ret_accum += acc1 * (1 - rR);

    accum += k.pll_mul(ret_accum);
  } else
#endif
    if (mat.is_reflective) {
  float3 t_accum;
  cam.ray_color(recursion_depth + 1, t_accum, new_r, 0,
                SEPSILON, numeric_limits<double>::max());
  accum += mat.i.pll_mul(t_accum);
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
extern int TILE_SIZE;

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

#ifdef SCUTTHRU
      int samples_taken = 0;
      float3 last_shd_accum(nanf(""), nanf(""), nanf(""));
#endif

      for (int rdi = 0; rdi < SHDNSAMPLE; ++rdi) {
        for (int rdj = 0; rdj < SHDNSAMPLE; ++rdj) {
#ifdef SCUTTHRU
          ++samples_taken;
#endif
          double ru = la->dst(la->e2) + rdi + la->hl;
          double rv = la->dst(la->e2) + rdj + la->hl;
          float3 l = la->l(in, ru, rv);

          float3 shd_accum;
          do_shadow(*this, shd_accum, r, lgh, l, in);
          a += shd_accum; // * in.n.dot(l);

#ifdef SCUTTHRU
          if (!last_shd_accum.is_nan()
              && (shd_accum - last_shd_accum).is_epsilon()) {
            // CUT-THROUGH
            break;
          } else {
            last_shd_accum = shd_accum;
          }
#endif
        }
      }

#ifdef CUTTHRU
      accum += a * (1. / samples_taken);
#else
      accum += a * (1. / SQ(SHDNSAMPLE));
#endif

    } else {
      float3 l = lgh.l(in);

      do_shadow(*this, accum, r, lgh, l, in);
    }
  }

  if (!mat.is_reflective && !mat.is_refractive) {
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
#ifdef GCUTTHRU
      float3 last_accum(nanf(""), nanf(""), nanf(""));
#endif
      int _cnt = 0;
      for (; _cnt < GLSNSAMPLE; ++_cnt) {
        float3 nrf = rf + (nu * (dst(e2) - 0.5) + nv * (dst(e2) - 0.5)) * mat.a;
        nrf.normalize_();
        Ray rr(nrf, in.p);

#ifdef GCUTTHRU
        float3 tmp_accum;
        do_dirty(*this, tmp_accum, r, rr, mat, recursion_depth, in);
        glossy_accum += tmp_accum;

        if (!last_accum.is_nan() && (tmp_accum - last_accum).is_epsilon()) {
          break;
        } else {
          last_accum = tmp_accum;
        }
#else
        do_dirty(*this, glossy_accum, r, rr, mat, recursion_depth, in);
#endif
      }

      accum += glossy_accum * (1. / _cnt);

    } else {
#endif
      Ray rr(rf, in.p);

      do_dirty(*this, accum, r, rr, mat, recursion_depth, in);
#ifdef FEAT_GLOSSY
    }
#endif
  }
}

void
Camera::render(int pxs, int pxe, int pys, int pye)
{

}

void
Camera::render()
{
#ifdef FEAT_DOF
  if (aperture_size < 1e-10) {
    DOFNSAMPLE = 1;
  }
#endif

  cout << endl
  << "Renderer parameters:" << endl
  << "\tRendering with BVH tree = " << "Yes" << endl
  << "\tSoft shadow = " << "Yes if an area light is included in the scene"
                        << endl
  << "\tGlossy effect = "

#ifdef FEAT_GLOSSY
  << "Yes if a glossy material is included in the scene"
#else
  << "No"
#endif
  << endl

  << "\tRefractions = "
#ifdef FEAT_REFRACT
  << "Yes if a refractive material is included in the scene"
#else
  << "No"
#endif
  << endl


  << "\tDepth of field = "
#ifdef FEAT_DOF
  << "Yes if the camera is set with related parameters"
#else
  << "No"
#endif
  << endl


  << "\tRay color epsilon cut-through = "
#if defined(SCUTTHRU) || defined(GCUTTHRU) || defined(DCUTTHRU)
  << "Yes"
#else
  << "No"
#endif
  << endl


  << "\tTiling = "
#ifdef USE_TILING
  << "Yes"
#else
  << "No"
#endif
 << endl


  << "\tProgress = "
#ifdef PROGRESS
  << "Yes"
#else 
  << "No"
#endif
  << endl

  << endl
  << "\tMaximal recursion limit = " << MAXRECUR << endl
  << "\tEpsilon = " << SEPSILON << endl
  << "\tPrimary rays per pixel = " << SQ(NSAMPLE) << endl
  << "\tShadow rays per primary ray = " << SQ(SHDNSAMPLE) << endl
#ifdef FEAT_GLOSSY
  << "\tGlossy rays per primary ray = " << GLSNSAMPLE << endl
#endif
#ifdef USE_TILING
  << "\tTile size = " << TILE_SIZE << endl
#endif
#ifdef PROGRESS
  << "\tProgress sample rate = every " << PROGRESS_SAMPLE_RATE << PROGRESS_UNIT
#endif
  << endl
#ifdef FEAT_DOF
  << "\tDoF rays per primary ray = " << DOFNSAMPLE << endl
  << "\tAperture size = " << aperture_size << endl
  << "\tFocal depth = " << lens << endl
#endif
  ;
  auto begin = chrono::steady_clock::now();

#ifdef USE_TILING
  int sqtile = SQ(TILE_SIZE);
  int total_tiles = std::ceil((1.0 * ph * pw) / sqtile);
#ifdef PROGRESS
  double dtotal_tiles = total_tiles / 100.0;
#endif

  int total_xn = std::ceil(1.0 * pw / TILE_SIZE);
  int total_yn = std::ceil(1.0 * ph / TILE_SIZE);
#else
#ifdef PROGRESS
  double total_pixels = ph * pw / 100.0;
#endif
#endif
  cout << endl
  << "Scene parameters:" << endl
  << "\tTotal pixels = " << ph * pw << endl
#ifdef USE_TILING
  << "\tTotal tiles = " << int(total_tiles) << endl
#endif
  << endl;

  printf("Shyoujyou rendering...\n");

#ifdef USE_TILING
  int pxs, pxe, pys, pye;
#endif

  int tn = 0;

#ifdef USE_TBB
#ifdef PROGRESS
#ifdef USE_TILING
  tbb::concurrent_bounded_queue<int> prog_queue;
  std::thread prog_t([&]() {
    auto acc = 0;
    int __t;
    while (1) {
      prog_queue.pop(__t);
      if (__t < 0) {
        break;
      }

      ++acc;
      if (acc % PROGRESS_SAMPLE_RATE != 0) {
        continue;
      }

      auto time_elapsed = chrono::
      duration_cast<chrono::seconds>(chrono::
                                     steady_clock::now() - begin).count();
      time_elapsed = FEQ(time_elapsed, 0.0) ? 1.0 : time_elapsed;

      printf(
#if defined(__clang__)
             "Rendered % 2.2lf%% in %lld seconds, % 2.2lld pixels/s...\r",
#else
             "Rendered % 2.2lf%% in %ld seconds, % 2.2ld pixels/s...\r",
#endif
             acc / dtotal_tiles,
             time_elapsed,
             acc * sqtile / time_elapsed);
      fflush(stdout);
    }
  });

  prog_t.detach();
#endif
#endif

#ifdef USE_TILING
  tbb::parallel_for(0, total_xn, [&](int tx) {
    tbb::parallel_for(0, total_yn, [&](int ty) {
#else
  tbb::parallel_for(0, pw, [&](int x) {
    tbb::parallel_for(0, ph, [&](int y) {
#endif

#else

#ifdef USE_TILING

  for (int tx = 0; tx < total_xn; ++tx) {
    for (int ty = 0; ty < total_yn; ++ty) {

#else
  for (int x = 0; x < pw; ++x) {
    for (int y = 0; y < ph; ++y) {
#endif

#endif
      ++tn;
#ifdef USE_TILING
      pxs = tx * TILE_SIZE;
      pxe = min((tx + 1) * TILE_SIZE, pw);
      pys = ty * TILE_SIZE;
      pye = min((ty + 1) * TILE_SIZE, ph);

      for (int x = pxs; x < pxe; ++x) {
        for (int y = pys; y < pye; ++y) {
#endif
          float3 clr(0., 0., 0.);
          for (int p = 0; p < NSAMPLE; ++p) {
            for (int q = 0; q < NSAMPLE; ++q) {
              Ray r = ray(x + (p + dst(e2)) / NSAMPLE,
                          y + (q + dst(e2)) / NSAMPLE);

#ifdef FEAT_DOF
              float3 sub;
              int o = 0;

#ifdef DCUTTHRU
              float3 last_tmp_sub(nanf(""), nanf(""), nanf(""));
#endif

              for (; o < DOFNSAMPLE; ++o) {
                float3 rand_point = this->u * (dst(e2) - 0.5)
                + this->v * (dst(e2) - 0.5);
                rand_point *= this->aperture_size;

                r.d *= this->lens;
                r.d -= rand_point;
                r.d.normalize_();

                r.p += rand_point;

#ifdef DCUTTHRU
                float3 tmp_sub;
                ray_color(0, tmp_sub, r, 0, 0., numeric_limits<double>::max());
                sub += tmp_sub;
                if (!last_tmp_sub.is_nan()
                    && (tmp_sub - last_tmp_sub).is_epsilon()) {
                  break;
                } else {
                  last_tmp_sub = tmp_sub;
                }
#else
                ray_color(0, sub, r, 0,
                          0., numeric_limits<double>::max());
#endif
              }
              sub *= 1. / o;
              clr += sub;
#else
              ray_color(0, clr, r, 0,
                        0., numeric_limits<double>::max());
#endif
            }
          }
          clr *= 1. / SQ(NSAMPLE);

#ifdef VISPROG
          rlet_queue.push(new Renderlet(clr, x, y));
#endif

          set_pixel(x, y, clr);
#ifdef USE_TILING
        }
      }
#endif

#ifdef PROGRESS
#if defined(USE_TILING) && defined(USE_TBB)
      prog_queue.push((pxe - pxs) * (pye - pys));
#else
      if (tn % PROGRESS_SAMPLE_RATE == 0) {
        auto time_elapsed = chrono::
        duration_cast<chrono::seconds>(chrono::
                                       steady_clock::now() - begin).count();
        time_elapsed = FEQ(time_elapsed, 0.0) ? 1.0 : time_elapsed;

        printf(
#if defined(__clang__)
               "Rendered % 2.2lf%% in %lld seconds, % 2.2lld pixels/s...\r",
#else
               "Rendered % 2.2lf%% in %ld seconds, % 2.2ld pixels/s...\r",
#endif
#ifdef USE_TILING
               tn / dtotal_tiles,
               time_elapsed,
               tn * sqtile / time_elapsed);
#else
               tn / total_pixels,
               time_elapsed,
               tn / time_elapsed);
#endif
        fflush(stdout);
      }
#endif
#endif

#ifdef USE_TBB
    });
  });
#if defined(PROGRESS) && defined(USE_TBB) && defined(USE_TILING)
  prog_queue.push(-1);
#endif

#else
    }
  }
#endif

#ifdef VISPROG
  rlet_queue.push(new Renderlet(float3(), -1, -1));
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
