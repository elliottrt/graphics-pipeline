#version 120

uniform vec3 eye;
varying vec3 tcs;

void main() {
    tcs = gl_Vertex.xyz - eye;
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
