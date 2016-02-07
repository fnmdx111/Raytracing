#ifndef __H_SCENE__
#define __H_SCENE__

#include "parse.hh"
#include <ImfRgbaFile.h>
#include <ImfArray.h>

using namespace Imf;

class Scene
{
public:
  Camera cam;
  vector<Shape*> shapes;
  vector<Light*> lights;
  vector<Material*> materials;

  Array2D<Rgba> pixels;

  Scene(const string& fn): cam(Camera(0., 0., 0., 0., 0., 0., 0., 0., 0., 0.,
				      0.)),
			   shapes(vector<Shape*>()),
			   lights(vector<Light*>()),
			   materials(vector<Material*>())
  {
    Camera* dummy = 0;
    parse(fn, this->shapes, this->materials, this->lights, dummy);
    this->cam.copy(*dummy);
    delete dummy;

    pixels.resizeErase(this->cam.ph, this->cam.pw);
  }

  void render();
  void save(const string& fn) const;
};

#endif

