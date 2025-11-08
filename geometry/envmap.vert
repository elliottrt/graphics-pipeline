#version 120

uniform vec3 eye;

varying vec4 color;
varying vec3 normal;

void main() {
    color = gl_Color;
    normal = gl_Normal;

    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
