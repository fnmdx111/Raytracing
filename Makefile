LIBPATH += -L"/usr/local/lib"
INCPATH += -Iinclude
INCPATH += -I"/usr/local/include"

LIBRARIES = `pkg-config --libs OpenEXR` -ltbb `pkg-config --libs sdl2`
INCLUDES = `pkg-config --cflags OpenEXR` `pkg-config --cflags sdl2`

AFEATS = -DNDEBUG -DTPROGRESS -DFEAT_DOF -DMULTITHD -DFEAT_GLOSSY
EFEATS = -DNDEBUG -DTPROGRESS -DFEAT_DOF -DGPRUNING -DMULTITHD -DVPROGRESS -DFEAT_GLOSSY -DFEAT_REFRACT

all:
	g++ -O3 -DNDEBUG -DTPROGRESS src/*.cc -Iinclude `pkg-config --cflags OpenEXR` `pkg-config --libs OpenEXR` -o raytra --std=c++11

adv:
	g++ -O3 $(AFEATS) src/*.cc $(LIBPATH) $(INCPATH) $(INCLUDES) $(LIBRARIES) -o raytra --std=c++11

exp:
	g++ -O3 $(EFEATS) src/*.cc $(LIBPATH) $(INCPATH) $(INCLUDES) $(LIBRARIES) -o raytra --std=c++11

dbg-exp:
	g++ -g $(EFEATS) src/*.cc $(LIBPATH) $(INCPATH) $(INCLUDES) $(LIBRARIES) -o raytra --std=c++11

adv-prof:
	g++ -pg -O3 $(AFEATS) src/*.cc $(LIBPATH) $(INCPATH) $(INCLUDES) $(LIBRARIES) -o raytra --std=c++11

profiler:
	g++ -pg -O3 -DNDEBUG -DTPROGRESS src/*.cc $(LIBPATH) $(INCPATH) $(INCLUDES) $(LIBRARIES) -o raytra --std=c++11

# -DFEAT_ANTIALIASING -DFEAT_SPECULAR_REFLECTION 
