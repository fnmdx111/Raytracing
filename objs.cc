#include "objs.hh"
#include <vector>

using namespace std;

Ray
Camera::ray(int i, int j)
{
  float su = this->l + (this->r - this->l) * (i + 0.5) / this->pw;
  float sv = this->b + (this->t - this->b) * (j + 0.5) / this->ph;

  float3 dir = this->dir * this->d + this->u * su + this->v * sv;
  dir.normalize_();

  return Ray(dir.x, dir.y, dir.z,
	     this->pos.x, this->pos.y, this->pos.z);
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
}

int
Ray::test_with(const vector<Shape*>& shapes,
	       vector<Intersection>& is) const
{
  unsigned int count = 0;

  for (size_t i = 0; i < shapes.size(); ++i) {
    count += shapes[i]->test_with(*this, is);
  }

  return count;
}
