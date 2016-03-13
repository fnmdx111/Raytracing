#ifndef __H_SCENE__
#define __H_SCENE__

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
  }

private:
  void parse(const string& fn);
  void build_tree();
};

#endif

