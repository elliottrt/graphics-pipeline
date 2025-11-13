#version 120

uniform vec3 eye;
uniform samplerCube cubemap;

varying vec3 tcs;

void main() {
	gl_FragColor = textureCube(cubemap, tcs);
}
