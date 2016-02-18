#include "objs.hh"
#include <vector>

using namespace std;

int
Ray::test_with(const vector<Shape*>& shapes,
               vector<Intersection>& is) const
{
  int count = 0;

  for (size_t i = 0; i < shapes.size(); ++i) {
    count += shapes[i]->test_with(*this, is);
  }

  return count;
}

Intersection&
Intersection::operator =(const Intersection& i)
{
  this->t = i.t;
  this->t2 = i.t2;
  this->sp = i.sp;
  this->p = i.p;
  this->n = i.n;

  return *this;
}

