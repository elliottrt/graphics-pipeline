#version 120

struct Impostor {
    vec3 V0;
    vec3 V1;
    vec3 V2;
};

uniform vec3 eye;
uniform Impostor impostors[2];

uniform sampler2D impostorTex;
uniform samplerCube cubemap;

varying vec4 color;
varying vec3 normal;
varying vec3 position;

void main() {

    // compute eye ray

    vec3 ray = normalize(position - eye);

    // reflect eye ray about normal to compute reflected ray

        vec3 reflected_ray = reflect(ray, normal);

    // intersect reflected ray with billboards

    float closest_t = 10000000;
    gl_FragColor = textureCube(cubemap, reflected_ray);

    for (int i = 0; i < 2; i++) {
        Impostor imp = impostors[i];
        vec3 N = normalize(cross(imp.V1 - imp.V0, imp.V2 - imp.V0));

        float t = dot(N, imp.V0 - position) / dot(reflected_ray, N);

        if (t >= 0) {
            vec3 Q = position + reflected_ray * t;

            vec3 ax = normalize(imp.V2 - imp.V0);
            vec3 ay = normalize(imp.V1 - imp.V0);
            float tcx = dot(Q - imp.V0, ax) / length(imp.V2 - imp.V0);
            float tcy = dot(Q - imp.V0, ay) / length(imp.V1 - imp.V0);

            if (0 <= tcx && tcx <= 1 && 0 <= tcy && tcy <= 1) {
                // hack to have multiple textures in one
                vec4 tcolor = texture2D(impostorTex, vec2((i + tcx) / 2, tcy));
                // don't use color if billboard is transparent here
                if (abs(tcolor.w) > 1e-3 && t < closest_t) {
                    gl_FragColor = tcolor;
                    closest_t = t;
                }
            }
        }
    }

}
