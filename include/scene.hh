#ifndef __H_SCENE__
#define __H_SCENE__

#include <algorithm>
#include "objs.hh"
#include "lights.hh"
#include "camera.hh"

class BVHNode : public Shape
{
public:
  BVHNode* chl;
  BVHNode* chr;
  Shape* shape;
  AABoundingBox aabb;
  BVHNode(Shape* s)
  {
    shape = s;
    chl = 0;
    chr = 0;
  }

  ~BVHNode()
  {
    if (chl != 0) {
      delete chl;
    }

    if (chr != 0) {
      delete chr;
    }
  }

  ShapeType type() const;
  bool test_with(const Ray& r,
                 Intersection& v,
                 double t0, double t1) const;
};

class Scene
{
public:
  Camera cam;
  vector<Shape*> shapes;
  vector<Light*> lights;
  vector<Material*> materials;

  BVHNode* root;

  Scene(const string& fn): cam(Camera(0., 0., 0., 0., 0., 0., 0., 0., 0., 0.,
				      0., 0)),
			   shapes(vector<Shape*>()),
			   lights(vector<Light*>()),
			   materials(vector<Material*>())
  {
    parse(fn);
    printf("Parsing done: %ld objects, %ld materials, %ld lights.\n",
           shapes.size(), materials.size(), lights.size());

    root = new BVHNode(0);
    build_tree();
  }

  ~Scene()
  {
    delete root;

    std::for_each(shapes.begin(), shapes.end(),
                  [](Shape* sh) { delete sh; });
    std::for_each(lights.begin(), lights.end(),
                  [](Light* l) { delete l; });
    std::for_each(materials.begin(), materials.end(),
                  [](Material* mat) { delete mat; });  }

private:
  void parse(const string& fn);
  void build_tree();
};


struct Renderlet
{
  double r;
  double g;
  double b;
  int x;
  int y;
  Renderlet(): r(0), g(0), b(0), x(0), y(0) {}
  Renderlet(const float3& c, int px, int py): r(c.x), g(c.y), b(c.z),
                                              x(px), y(py) {}

};

#endif

