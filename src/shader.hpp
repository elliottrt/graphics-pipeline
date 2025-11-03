#ifndef SHADER_HPP
#define SHADER_HPP

#include "gl.hpp"
#include <string>

struct ShaderProgram {

	GLuint program;

	ShaderProgram(const char *vertPath, const char *fragPath);
	ShaderProgram();

	void Enable();
	void Disable();

	GLint GetUniformLocation(const char *name);

	void SetUniform(GLint location, float f);
	void SetUniform(GLint location, const V3 &v);
	void SetUniform(GLint location, const M3 &m);

};

#endif
