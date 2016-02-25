#include "scene.hh"

#include <cmath>
#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <string>
#include <vector>

#include "lights.hh"
#include "objs.hh"

using namespace std;

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
      cerr << "c" << endl;
      exit(-1);
    }
  }

  this_float = atof(p);
  delete[] cstr;

  return this_float;
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

      Camera dummy(x, y, z, vx, vy, vz,
				 d, iw, ih, int(pw), int(ph), this);
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
      double dr, dg, db, sr, sg, sb, r, ir, ig, ib;
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
      last_material = new Material(dr, dg, db, sr, sg, sb,
                                   r, ir, ig, ib);
      mats.push_back(last_material);
#ifdef __DEBUG__
      printf("debug::material: %f %f %f, %f %f %f, %f, %f %f %f\n",
             dr, dg, db, sr, sg, sb, r, ir, ig, ib);
#endif
    }
    }
  }
}
