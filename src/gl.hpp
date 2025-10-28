#ifndef GL_HPP
#define GL_HPP

#include "math/v3.hpp"
#include "mesh.hpp"

// TODO: crossplatform opengl stuff

#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>

// opengl utility functions

void hwClear(const V3 &color);

void hwDrawMesh(const Mesh &mesh, bool fill = true, unsigned int tex = 0);

#endif
