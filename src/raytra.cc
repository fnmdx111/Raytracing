#include "objs.hh"
#include "scene.hh"
#include <algorithm>

using namespace std;

int NSAMPLE = 1;
int SHDNSAMPLE = 1;
int GLSNSAMPLE = 1;
int DOFNSAMPLE = 1;

int
main(int argc, char** argv)
{
  if (argc < 3) {
    cout << "usage: scene-file output-path" << endl;
    return -1;
  }
  if (argc > 3) {
    NSAMPLE = atoi(argv[3]);
  }
  if (argc > 4) {
    SHDNSAMPLE = atoi(argv[4]);
  }
  if (argc > 5) {
    GLSNSAMPLE = atoi(argv[5]);
  }
  if (argc > 6) {
    DOFNSAMPLE = atoi(argv[6]);
  }

  Scene scene((string(argv[1])));
  scene.cam.render();
  scene.cam.save(string(argv[2]));

  return 0;
}
