#ifndef __H_SCENE__
#define __H_SCENE__

#include "objs.hh"
#include "lights.hh"
#include "camera.hh"

class Scene
{
public:
  Camera cam;
  vector<Shape*> shapes;
  vector<Light*> lights;
  vector<Material*> materials;

  Scene(const string& fn): cam(Camera(0., 0., 0., 0., 0., 0., 0., 0., 0., 0.,
				      0., 0)),
			   shapes(vector<Shape*>()),
			   lights(vector<Light*>()),
			   materials(vector<Material*>())
  {
    parse(fn);
  }

private:
  void parse(const string& fn);
};

#endif

