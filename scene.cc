#include "objs.hh"
#include "scene.hh"

void Scene::render()
{
  for (int i = 0; i < this->cam.pw; ++i) {
    for (int j = 0; j < this->cam.ph; ++j) {
      Ray r = this->cam.ray(i, j);
     
    }
  }
}
