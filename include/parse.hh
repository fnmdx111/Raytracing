#ifndef __H_PARSE__
#define __H_PARSE__

#include <cmath>
#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <string>
#include <vector>

#include "objs.hh"

using namespace std;

float get_token_as_float(string in, int which);

void parse(const string& filename,
	   vector<Shape*>& objs,
	   vector<Material*>& mats,
	   vector<Light*>& lights,
	   Camera*& camera);

#endif

