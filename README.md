Course project for Computer Graphics
====

This project is a raytracing renderer.

Main features
----

* Limited scene primitives (sphere, triangle, and plane)
* Bounding Volume Hierarchy intersection acceleration
* Wavefront obj support
* Soft shadow
* Reflections
* Renderer's progress report (available with compiler flag `-DPROGRESS`)


Experimental features
----

* Glossy effect (available with compiler flag `-DFEAT_GLOSSY`)
* Refractions (available with compiler flag `-DFEAT_REFRACT`)
* Depth of field effect (simple camera model) (available with compiler flag `-DFEAT_DOF`)
* Multithreaded rendering (available with compiler flag `-DUSE_TBB`, need Intel Threading Building Blocks)


Unstable features
----

* Tiled rendering (available with compiler flag `-DUSE_TILING`) (Warning: it actually makes rendering slower, I guess it's because of all the overheads, but I'm not sure.)
* Cut-through, i.e. in distribution ray tracing, when the difference between this sample and the last sample is small enough, no more samples are taken. It could accelerate the sampling process in ray tracing. (available with compiler flag `-DCUTTHRU`) (It actually doesn't improve the rendering speed much, so it's off by default.)
