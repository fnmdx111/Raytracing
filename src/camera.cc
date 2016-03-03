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
  double su = this->l + (this->r - this->l) * (i + 0.5) / this->pw;
  double sv = this->b + (this->t - this->b) * (j + 0.5) / this->ph;

  float3 dir = this->dir * this->d + this->u * su + this->v * sv;
  dir.normalize_();

  return Ray(dir.x, dir.y, dir.z,
             this->pos.x, this->pos.y, this->pos.z);
}

inline void
Camera::accum_pixel(int i, int j, const float3& rgba)
{
  int nj = ph - j - 1;
  pixels[nj][i].r += rgba.x;
  pixels[nj][i].g += rgba.y;
  pixels[nj][i].b += rgba.z;
}

inline void
Camera::set_pixel(int i, int j, const float3& rgb)
{
  int nj = ph - j - 1;
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
Camera::ray_color(int recursion_depth,
                  float3& accum,
                  const Ray& r,
                  const Light* shadow_lgh,
                  const Shape* inv_shape,
                  double t0, double t1) const
{
  if (recursion_depth > MAXRECUR) {
    return;
  }

  Intersection ins;

  if (r.type == 's') { // is shadow ray
    assert(inv_shape != 0);
    assert(shadow_lgh != 0);

    for (int i = 0; i < scene->shapes.size(); ++i) {
      if (scene->shapes[i] == inv_shape) {
        continue;
      }
      if (scene->shapes[i]->test_with(r, ins, t0, t1)) {
        return;
      }
    }

    accum += shadow_lgh->clr;
    return;
  }

  bool intersected = false;
  Intersection in;
  double min_dist = numeric_limits<double>::max();

  for (int i = 0; i < scene->shapes.size(); ++i) {
    if (scene->shapes[i] == inv_shape) {
      continue;
    }
    if (scene->shapes[i]->test_with(r, ins, t0, t1)) {
      if (min_dist > ins.t) {
        min_dist = ins.t;

        in = ins;
        intersected = true;
      }
    }
  }

  if (!intersected) {
    return;
  }

  const Material& mat = *(in.sp->mat);

  for (int k = 0; k < scene->lights.size(); ++k) {
    const Light& lgh = *(scene->lights[k]);

    if (lgh.type() == LightType::ambient) {
      mat.ambient(accum, lgh);
    } else {
      const float3 l = lgh.l(in);

      Ray sr(l, in.p);
      sr.type = 's';

      const double dist = lgh.dist(in);

      float3 sclr(0., 0., 0.);
      ray_color(0, sclr, sr, &lgh, in.sp, SEPSILON, dist);

      if (!sclr.is_zero()) {
        if (r.d.dot(in.n) > 0.) {
          in.n.negate();
        }

        float3 nd = r.d * -1;
        mat.diffuse(accum, l, in.n, nd, lgh.clr);

        mat.specular(accum, l, in.n, nd, lgh.clr);
      }
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
              in.sp,
              SEPSILON,
              numeric_limits<double>::max());
    accum += mat.i.pll_mul(r_accum);
  }
}

void
Camera::render()
{
  printf("Shyoujyou rendering...\n");

  auto begin = chrono::steady_clock::now();

#ifdef FEAT_ANTIALIASING
  std::random_device rd;
  std::mt19937 e2(rd());
  std::uniform_real_distribution<> dist(0.0, 0.999999999);
#endif

#ifdef PROGRESS
  double total = pw * ph;
  long cnt = 0;
#endif
  for (int x = 0; x < pw; ++x) {
    for (int y = 0; y < ph; ++y) {
#ifdef PROGRESS
      if (++cnt % 50 == 0
          && printf("\rRendered %0.03f%%", (cnt / total) * 100.)) {
        ;
      }
#endif

      float3 clr(0., 0., 0.);
#ifdef FEAT_ANTIALIASING
      for (int p = 0; p < NSAMPLE; ++p) {
        for (int q = 0; q < NSAMPLE; ++q) {
          Ray r = ray(x + (p + dist(e2)) / NSAMPLE,
                      y + (q + dist(e2)) / NSAMPLE);

          ray_color(0, clr, r, 0, 0, 0., numeric_limits<double>::max());
        }
      }

      set_pixel(x, y, clr * (1. / SQ(NSAMPLE)));
#else
      // Ray r = ray(x + 0.5, y + 0.5);
      Ray r = ray(x, y);
      ray_color(0, clr, r, 0, 0, 0., numeric_limits<double>::max());
      set_pixel(x, y, clr);
#endif
    }
  }

  auto end = chrono::steady_clock::now();
#ifdef PROGRESS
  printf(".\n");
#endif
  printf("Done in %lld seconds.\n",
         chrono::duration_cast<chrono::seconds>(end - begin).count());
}

void
Camera::save(const string& fn) const
{
  write_rgba(fn, &this->pixels[0][0], this->pw, this->ph);
}
