#include "shader.hpp"
#include <OpenGL/gl.h>
#include <fstream>
#include <iostream>
#include <sstream>

ShaderProgram::ShaderProgram(const char *vertPath, const char *fragPath) {

	std::ifstream vertIfs(vertPath);
	std::ifstream fragIfs(fragPath);

	if (!vertIfs.is_open() || !fragIfs.is_open()) {
		std::cerr << "error: failed to open shader files" << std::endl;
		program = 0;
		return;
	}

	std::stringstream vertSs;
	std::stringstream fragSs;
	vertSs << vertIfs.rdbuf();
	fragSs << fragIfs.rdbuf();

	std::string vertSource = vertSs.str();
	std::string fragSource = fragSs.str();

	vertIfs.close();
	fragIfs.close();

	const char *vertexShaderSource = vertSource.c_str();
	const char *fragmentShaderSource = fragSource.c_str();

	// from https://learnopengl.com/Getting-started/Shaders

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "error: vertex shader compilation failed:\n" << infoLog << std::endl;
    }


    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "error: fragment shader compilation failed:\n" << infoLog << std::endl;
    }


    program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    // check for linking errors
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        std::cerr << "error: shader linking failed:\n" << infoLog << std::endl;
		program = 0;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

}

ShaderProgram::ShaderProgram(): program(0) {}

void ShaderProgram::Enable() {
	glUseProgram(program);
}

void ShaderProgram::Disable() {
	glUseProgram(0);
}

GLint ShaderProgram::GetUniformLocation(const char *name) {
	return glGetUniformLocation(program, name);
}

void ShaderProgram::SetUniform(GLint location, float f) {
    glUniform1f(location, f);
}

void ShaderProgram::SetUniform(GLint location, const V3 &v) {
    glUniform3f(location, v.x(), v.y(), v.z());
}

void ShaderProgram::SetUniform(GLint location, const M3 &m) {
    glUniformMatrix3fv(location, 1, GL_FALSE, m);
}
