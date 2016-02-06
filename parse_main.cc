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

#define pf(x) printf("%f", (x));
#define pf3(t) printf("float3(%f, %f, %f)", (t).x, (t).y, (t).z);
#define sp printf(" ");
#define nl printf("\n");

  Camera* cam = &scene.cam;
  const vector<Shape*>& shapes = scene.shapes;
  const vector<Light*>& lights = scene.lights;

  pf3(cam->pos) sp pf3(cam->v) nl;

  pf3(((SSphere*) shapes[0])->pos) sp pf3(((SSphere*) shapes[0])->mat->d) nl;

  pf3(((STriangle*) shapes[2])->pos1) sp pf3(((STriangle*) shapes[2])->mat->d) nl;

  pf3(((LAmbient*) lights[1])->clr) nl;

  pf3(cam->u) sp pf3(cam->v) sp pf3(cam->w) nl;

  return 0;
}
