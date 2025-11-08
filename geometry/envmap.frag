#version 120

// ray-rect idea from https://stackoverflow.com/questions/8812073/ray-and-square-rectangle-intersection-in-3d

struct Impostor {
    vec3 V0;
    vec3 V1;
    vec3 V2;
};

uniform vec3 eye;
uniform Impostor impostors[2];

uniform sampler2D impostorTex;

varying vec4 color;
varying vec3 normal;

void main() {

// compute eye ray

    vec3 ray = normalize(gl_FragCoord.xyz - eye);

// reflect eye ray about normal to compute reflected ray

    vec3 reflected_ray = reflect(ray, normal);

// intersect reflected ray with billboards

    float closest_t = 100000;
    vec4 closest_color = color; // TODO: what do we do when no impostor hit?

    for (int i = 0; i < 2; i++) {
        Impostor imp = impostors[i];
        vec3 N = normalize(cross(imp.S1, imp.S2));

        float t = dot(imp.P0 - gl_FragCoord.xyz, N) / dot(reflected_ray, N);

        if (t > 0) {
            vec3 Q = gl_FragCoord.xyz + reflected_ray * t;
            vec3 ax = normalize(imp.S1);
            vec3 ay = normalize(imp.S2);
            float Qax = dot(Q - imp.P0, ax) / length(imp.S1);
            float Qay = dot(Q - imp.P0, ay) / length(imp.S2);

            if (0 < Qax && Qax < 1 && 0 < Qay && Qay < 1) {
                vec4 texColor = texture2D(impostorTex, vec2((i + Qax) / 2, Qay));
                closest_color = texColor;
            }
        }
    }

// return color of closest intersection

    gl_FragColor = closest_color;
}
