#version 120

uniform vec3 eye;

varying vec4 color;
varying vec3 normal;
varying vec3 position;

void main() {
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;

    color = gl_Color;
    normal = gl_Normal;
    position = gl_Vertex.xyz;
}
