#include "parse.hh"
#include "objs.hh"
#include "scene.hh"
#include <vector>

using namespace std;

int
main(int argc, char** argv)
{
  if (argc < 3) {
    cout << "usage: scene-file output-path" << endl;
    return -1;
  }

  Scene scene((string(argv[1])));
  scene.render();
  scene.save(string(argv[2]));

  return 0;
}
