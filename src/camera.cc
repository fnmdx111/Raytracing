#include "hw0.hh"
#include "objs.hh"
#include "lights.hh"
#include <vector>
#include <limits>
#include "camera.hh"


using namespace std;

Ray
Camera::ray(int i, int j)
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

#define max_(__t, __d) ((__t) > (__d) ? (__t) : (__d))

void
do_lambertian(int x, int y,
              Camera& cam,
              const vector<Light*>& lights,
              const Intersection& in)
{
  const Material& mat = *(in.sp->mat);

  for (int i = 0; i < lights.size(); ++i) {
    const Light& lgh = *lights[i];
    const float3& l = lgh.l(in);

    if (isnan(l.x)) {
      // ambient light
      // cam.accum_pixel(x, y, mat.d.pll_mul(lgh.clr));
    } else {
      double d = in.n.dot(l);
      d = max_(0., d);

      cam.accum_pixel(x, y, mat.d.pll_mul(lgh.clr) * d);
    }
  }
}

void
do_specular(int x, int y,
            Camera& cam,
            const vector<Light*>& lights,
            const Intersection& in,
            const float3& v)
{
  const Material& mat = *(in.sp->mat);

  for (int i = 0; i < lights.size(); ++i) {
    const Light& lgh = *lights[i];
    const float3& l = lgh.l(in);

    if (isnan(l.x)) {
    } else {
      double d = in.n.dot(bisector(v, l));
      d = max_(0., d);
      d = pow(d, mat.r);

      cam.accum_pixel(x, y, mat.s.pll_mul(lgh.clr) * d);
    }
  }
}

void
Camera::render()
{
  for (int i = 0; i < pw; ++i) {
    for (int j = 0; j < ph; ++j) {
      set_pixel(i, j, float3(0., 0., 0.));

      Ray r = ray(i, j);
      vector<Intersection> ins;
      const int count = r.test_with(scene->shapes, ins);

      if (count <= 0) {
        continue;
      }

      int idx = -1;
      double min_dist = numeric_limits<double>::max();

      for (int k = 0; k < ins.size(); ++k) {
        const double dist = pos.sq_dist(ins[k].p);

        if (min_dist > dist) {
          idx = k;
          min_dist = dist;
        }
      }

      if (idx < 0) {
        cout << "wtf!" << endl;
        continue;
      }

      do_lambertian(i, j,
                    *this,
                    this->scene->lights,
                    ins[idx]);

#ifdef __DEBUG__
      printf("done lambertian\n");
#endif

      r.d.negate();
      do_specular(i, j,
                  *this,
                  this->scene->lights,
                  ins[idx],
                  r.d);
    }
  }
}

void
Camera::save(const string& fn) const
{
  write_rgba(fn, &this->pixels[0][0], this->pw, this->ph);
}
