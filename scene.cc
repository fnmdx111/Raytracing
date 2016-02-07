#include "objs.hh"
#include "scene.hh"
#include "hw0.hh"

void Scene::render()
{
  for (int i = 0; i < this->cam.pw; ++i) {
    for (int j = 0; j < this->cam.ph; ++j) {
      Ray r = this->cam.ray(i, j);
      vector<Intersection> intersections;
      int count = r.test_with(this->shapes, intersections);

      if (count > 0) {
	this->pixels[j][i] = Rgba(1.0, 0.0, 0.0, 1.0);
      }
    }
  }
}

void Scene::save(const string& fn) const
{
  write_rgba(fn, &this->pixels[0][0], (int) this->cam.pw, (int) this->cam.ph);
}

