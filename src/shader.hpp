#ifndef SHADER_HPP
#define SHADER_HPP

#include "gl.hpp"
#include <string>

typedef GLint ShaderUniformLocation;

struct ShaderProgram {

	GLuint program;

	ShaderProgram(const char *vertPath, const char *fragPath);
	ShaderProgram();

	void Enable();
	void Disable();

	ShaderUniformLocation GetUniformLocation(const char *name);

	void SetUniform(ShaderUniformLocation location, float f);
	void SetUniform(ShaderUniformLocation location, const V3 &v);
	void SetUniform(ShaderUniformLocation location, const M3 &m);
	void SetUniform(ShaderUniformLocation location, HWTexID texID, unsigned index);

};

#endif
