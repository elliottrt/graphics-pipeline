#ifndef GL_HPP
#define GL_HPP

#include "frame_buffer.hpp"
#include "math/v3.hpp"
#include "mesh.hpp"

// TODO: crossplatform opengl stuff

#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>

typedef GLuint HWTexID;

// opengl utility functions

void hwInit(void);

void hwClear(const V3 &color);

void hwDrawMesh(const Mesh &mesh, bool fill = true, HWTexID tex = 0);

void hwTexFromFb(HWTexID texId, const FrameBuffer &fb);

void hwBegin2D(int w, int h);
void hwEnd2D(void);

#endif
