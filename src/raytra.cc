#include "objs.hh"
#include "scene.hh"
#include <algorithm>

using namespace std;

int
main(int argc, char** argv)
{
  if (argc < 3) {
    cout << "usage: scene-file output-path" << endl;
    return -1;
  }

  Scene scene((string(argv[1])));
  scene.cam.render();
  scene.cam.save(string(argv[2]));

  for_each(scene.shapes.begin(), scene.shapes.end(),
           [](Shape* sh) { delete sh; });
  for_each(scene.lights.begin(), scene.lights.end(),
           [](Light* l) { delete l; });
  for_each(scene.materials.begin(), scene.materials.end(),
           [](Material* mat) { delete mat; });

  return 0;
}
