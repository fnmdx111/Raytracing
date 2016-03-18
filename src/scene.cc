#include "scene.hh"

#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <string>
#include <vector>
#include <tuple>
#include <algorithm>

#include "lights.hh"
#include "objs.hh"

using namespace std;

ShapeType
BVHNode::type() const
{
  return ShapeType::bvhnode;
}

bool
BVHNode::test_with(const Ray &r, Intersection &v, double t0, double t1) const
{
  if (shape != 0) {
    bool intersected = shape->test_with(r, v, t0, t1);
    if (intersected) {
      v.sp = shape;
    }

    return intersected;
  } else if (shape == 0 && aabb.test_with(r, v, t0, t1)) {
    Intersection itsl, itsr;

    bool left_hit = (chl != 0) && chl->test_with(r, itsl, t0, t1);
    bool right_hit = (chr != 0) && chr->test_with(r, itsr, t0, t1);

    if (left_hit && right_hit) {
      if (itsl.t < itsr.t) {
        v = itsl;
      } else {
        v = itsr;
      }
      return true;
    } else if (left_hit) {
      v = itsl;
      return true;
    } else if (right_hit) {
      v = itsr;
      return true;
    }

    return false;
  }

  return false;
}

inline double
midpoint_by_axis(const Shape* s, int axis)
{
  switch (axis) {
  case 0:
    return (s->aabb.xmax + s->aabb.xmin) / 2;

  case 1:
    return (s->aabb.ymax + s->aabb.ymin) / 2;

  case 2:
    return (s->aabb.zmax + s->aabb.zmin) / 2;

  default:
      return 0.0;
  }
}

size_t
partition_by_axis(vector<Shape*>& shapes, size_t h, size_t t, int axis)
{
  const Shape* pivot = shapes[t];
  const double pivot_midpoint = midpoint_by_axis(pivot, axis);

  size_t store_idx = h;

  for (size_t cmp_idx = h; cmp_idx < t - 1; ++cmp_idx) {
    if (midpoint_by_axis(shapes[cmp_idx], axis) < pivot_midpoint) {
      Shape* temp = shapes[cmp_idx];
      shapes[cmp_idx] = shapes[store_idx];
      shapes[store_idx] = temp;

      ++store_idx;
    }
  }

  Shape* temp = shapes[t];
  shapes[t] = shapes[h];
  shapes[h] = temp;

  return store_idx;
}

size_t
quickselect(vector<Shape*>& shapes, size_t h, size_t t, size_t k, int axis)
{
  while (h < t) {
    size_t mid = partition_by_axis(shapes, h, t, axis);
    if (mid == k) {
      return mid;
    } else if (mid < k) {
      h = mid + 1;
    } else {
      t = mid - 1;
    }
  }

  return h;
}

size_t
quickselect1(vector<Shape*>& shapes, size_t h, size_t t, size_t k, int axis)
{
  sort(shapes.begin() + h, shapes.begin() + t,
       [axis](const Shape* s1, const Shape* s2) {
         return midpoint_by_axis(s1, axis) < midpoint_by_axis(s2, axis);
       });

  return k;
}

void
build(vector<Shape*>& shapes, size_t h, size_t t,
      BVHNode* current, int axis)
{
  if (h == t) {
    current->shape = shapes[h];
    current->aabb.copy_from(shapes[h]->aabb);
  } else {
    size_t mid_idx = quickselect1(shapes, h, t, (h + t) / 2, axis);

    if (h <= mid_idx) {
      current->chl = new BVHNode(0);
      build(shapes, h, mid_idx, current->chl, (axis + 1) % 3);
    }

    if (mid_idx + 1 <= t) {
      current->chr = new BVHNode(0);
      build(shapes, mid_idx + 1, t, current->chr, (axis + 1) % 3);
    }

    current->aabb.copy_from(current->chl->aabb + current->chr->aabb);
  }
}

void
Scene::build_tree()
{
  printf("Building BVH tree...\n");
  build(shapes, 0, shapes.size() - 1, root, 0);
  printf("Building BVH tree done.\n");
}

double
get_token_as_float(string in, int which)
{
  double this_float = 0.;

  if (which == 0) {
    cerr << "a" << endl;
  }

  char* cstr = new char[in.size() + 1];
  strcpy(cstr, in.c_str());

  char* p = strtok(cstr, " ");
  if (p == 0) {
    cerr << "b" << endl;
    exit(-1);
  }

  for (int i = 0; i < which; ++i) {
    p = strtok(0, " ");
    if (p == 0) {
      return nanf("");
    }
  }

  this_float = atof(p);
  delete[] cstr;

  return this_float;
}

void read_wavefront_file(const string& file,
                         vector<tuple<int, int, int>>& tris,
                         vector<tuple<double, double, double>>& verts)
{
  // clear out the tris and verts vectors:
  tris.clear();
  verts.clear();

  ifstream in(file);
  char buffer[1025];
  string cmd;

  for (int line = 1; in.good(); ++line) {
    in.getline(buffer, 1024);
    buffer[in.gcount()] = 0;
    cmd = "";

    istringstream iss(buffer);

    iss >> cmd;
    if (cmd[0] == '#' or cmd.empty()) {
      // ignore comments or blank lines
      continue;
    } else if (cmd == "v") {
      // got a vertex:
      // read in the parameters:
      double pa, pb, pc;
      iss >> pa >> pb >> pc;
      verts.push_back(tuple<double, double, double>(pa, pb, pc));
    } else if (cmd == "f") {
      // got a face (triangle)

      // read in the parameters:
      int i, j, k;
      iss >> i >> j >> k;
      // vertex numbers in OBJ files start with 1, but in C++ array
      // indices start with 0, so we're shifting everything down by
      // 1
      tris.push_back(tuple<int, int, int>(i - 1, j - 1, k - 1));
    } else {
      std::cerr << "Parser error: invalid command at line "
      << line << std::endl;
    }
  }

  in.close();
}

void
Scene::parse(const string& filename)
{
  printf("Shyoujyou parsing...\n");

  vector<Shape*>& objs = this->shapes;
  vector<Light*>& lights = this->lights;
  vector<Material*>& mats = this->materials;

  ifstream in(filename.c_str());
  string line;

  if (!in.is_open()) {
    cerr << "d" << endl;
    exit(-1);
  }

  Material* last_material = 0;

  while (!in.eof()) {
    getline(in, line);

    int float_cnt = 0;
#define get_float(x) do {                       \
      ++float_cnt;                              \
      x = get_token_as_float(line, float_cnt);  \
    } while (0)

    switch (line[0]) {
    case 'w': {
      vector<tuple<int, int, int>> tris;
      vector<tuple<double, double, double>> verts;
      read_wavefront_file(line.substr(2), tris, verts);

      for (int _i = 0; _i < tris.size(); ++_i) {
        const auto& tri = tris[_i];
#define G0(t) get<0>(t)
#define G1(t) get<1>(t)
#define G2(t) get<2>(t)
        STriangle* t = new STriangle(G0(verts[G0(tri)]),
                                     G1(verts[G0(tri)]),
                                     G2(verts[G0(tri)]),
                                     G0(verts[G1(tri)]),
                                     G1(verts[G1(tri)]),
                                     G2(verts[G1(tri)]),
                                     G0(verts[G2(tri)]),
                                     G1(verts[G2(tri)]),
                                     G2(verts[G2(tri)]));
        t->set_material(last_material);
        objs.push_back(t);
      }

      break;
    }

    case 's': {
      double x, y, z, r;
      get_float(x);
      get_float(y);
      get_float(z);
      get_float(r);
      SSphere* s = new SSphere(x, y, z, r);
      s->set_material(last_material);
      objs.push_back(s);

#ifdef __DEBUG__
      printf("debug::sphere: %f %f %f, %f\n", s->pos.x, s->pos.y,
             s->pos.z, s->r);
#endif
      break;
    }

    case 't': {
      double x1, y1, z1, x2, y2, z2, x3, y3, z3;
      get_float(x1);
      get_float(y1);
      get_float(z1);
      get_float(x2);
      get_float(y2);
      get_float(z2);
      get_float(x3);
      get_float(y3);
      get_float(z3);
      STriangle* t = new STriangle(x1, y1, z1,
                                   x2, y2, z2,
                                   x3, y3, z3);
      t->set_material(last_material);
      objs.push_back(t);

#ifdef __DEBUG__
      printf("debug::triangle: %f %f %f, %f %f %f, %f %f %f\n",
             x1, y1, z1, x2, y2, z2, x3, y3, z3);
#endif
      break;
    }

    case 'p': {
      double nx, ny, nz, d;
      get_float(nx);
      get_float(ny);
      get_float(nz);
      get_float(d);
      SPlane* p = new SPlane(nx, ny, nz, d);
      p->set_material(last_material);
      objs.push_back(p);

#ifdef __DEBUG__
      printf("debug::plane: %f %f %f, %f\n", nx, ny, nz, d);
#endif
      break;
    }

    case 'c': {
      double x, y, z, vx, vy, vz, d, iw, ih, pw, ph;
      double lens;
      double asize;
      get_float(x);
      get_float(y);
      get_float(z);
      get_float(vx);
      get_float(vy);
      get_float(vz);
      get_float(d);
      get_float(iw);
      get_float(ih);
      get_float(pw);
      get_float(ph);
      get_float(lens);
      get_float(asize);

      Camera dummy(x, y, z, vx, vy, vz,
				 d, iw, ih, int(pw), int(ph), this, lens, asize);
      this->cam.copy(dummy);
      this->cam.scene = this;

#ifdef __DEBUG__
      printf("debug::camera: %f %f %f, %f %f %f, %f, %f %f, %d %d\n",
             this->cam.pos.x, this->cam.pos.y, this->cam.pos.z,
	     this->cam.dir.x, this->cam.dir.y, this->cam.dir.z,
	     this->cam.d,
	     this->cam.iw, this->cam.ih, this->cam.pw, this->cam.ph);
#endif
      break;
    }

    case 'l': {
      switch (line[2]) {
      case 'p': {
        double x, y, z, r, g, b;
        ++float_cnt;
        get_float(x);
        get_float(y);
        get_float(z);
        get_float(r);
        get_float(g);
        get_float(b);
        LPoint* p = new LPoint(x, y, z, r, g, b);
        lights.push_back(p);
#ifdef __DEBUG__
        printf("debug::light point: %f %f %f, %f %f %f\n",
               x, y, z, r, g, b);
#endif
        break;
      }

      case 's': {
        double x, y, z, nx, ny, nz, ux, uy, uz, len, r, g, b;
        ++float_cnt;
        get_float(x);
        get_float(y);
        get_float(z);
        get_float(nx);
        get_float(ny);
        get_float(nz);
        get_float(ux);
        get_float(uy);
        get_float(uz);
        get_float(len);
        get_float(r);
        get_float(g);
        get_float(b);
        LArea* la = new LArea(x, y, z, nx, ny, nz, ux, uy, uz, len, r, g, b);
        lights.push_back(la);

        break;
      }

      case 'd': {
        double vx, vy, vz, r, g, b;
        ++float_cnt;
        get_float(vx);
        get_float(vy);
        get_float(vz);
        get_float(r);
        get_float(g);
        get_float(b);
        LDirectional* ld = new LDirectional(vx, vy, vz, r, g, b);
        lights.push_back(ld);
#ifdef __DEBUG__
        printf("debug::light directional: %f %f %f, %f %f %f\n",
               vx, vy, vz, r, g, b);
#endif
        break;
      }

      case 'a': {
        double r, g, b;
        ++float_cnt;
        get_float(r);
        get_float(g);
        get_float(b);
        LAmbient* la = new LAmbient(r, g, b);
        lights.push_back(la);
#ifdef __DEBUG__
        printf("debug::light ambient: %f %f %f\n",
               r, g, b);
#endif
        break;
      }
      }
      break;
    }
    case 'm': {
      double dr, dg, db, sr, sg, sb, r, ir, ig, ib, a;
      get_float(dr);
      get_float(dg);
      get_float(db);
      get_float(sr);
      get_float(sg);
      get_float(sb);
      get_float(r);
      get_float(ir);
      get_float(ig);
      get_float(ib);
      get_float(a);
      last_material = new Material(dr, dg, db, sr, sg, sb,
                                   r, ir, ig, ib, a);
      mats.push_back(last_material);
#ifdef __DEBUG__
      printf("debug::material: %f %f %f, %f %f %f, %f, %f %f %f\n",
             dr, dg, db, sr, sg, sb, r, ir, ig, ib);
#endif
    }
    }
  }
}
