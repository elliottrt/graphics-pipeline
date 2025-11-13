#version 120

/*
    float closest_t = 100000;
    vec4 closest_color = color; // TODO: what do we do when no impostor hit?

    for (int i = 0; i < 2; i++) {
        Impostor imp = impostors[i];
        vec3 N = normalize(cross(imp.V1 - imp.V0, imp.V2 - imp.V0));

        float t = dot(N, imp.V0 - gl_FragCoord.xyz) / dot(reflected_ray, N);

        if (t >= 0) {
            vec3 Q = gl_FragCoord.xyz + reflected_ray * t;

            float tcx = dot(Q - imp.V0, imp.V1 - imp.V0) / dot(imp.V1 - imp.V0, imp.V1 - imp.V0);
            float tcy = dot(Q - imp.V0, imp.V2 - imp.V0) / dot(imp.V2 - imp.V0, imp.V2 - imp.V0);

            if (0 <= tcx && tcx <= 1 && 0 <= tcy && tcy <= 1) {
                gl_FragColor = texture2D(impostorTex, vec2((i + tcx) / 2, tcy));
            }

            // vec3 ax = normalize(imp.V2 - imp.V0);
            // vec3 ay = normalize(imp.V1 - imp.V0);
            // float Qax = dot(Q - imp.V0, ax) / length(imp.V2 - imp.V0);
            // float Qay = dot(Q - imp.V0, ay) / length(imp.V1 - imp.V0);

            // if (0 < Qax && Qax < 1 && 0 < Qay && Qay < 1) {
            //     // TODO: pick closest color
            //     gl_FragColor = texture2D(impostorTex, vec2((i + Qax) / 2, Qay));
            //     // closest_color = texColor;
            // }
        }
    }
*/

/*
gl_FragColor = color;

    vec3 V0 = impostors[1].V0;
    vec3 V1 = impostors[1].V1;
    vec3 V3 = impostors[1].V2;

    // compute eye ray

    vec3 P = position;
    vec3 eyeRay = normalize(eye-P);

    // reflect eye ray about normal to compute reflected ray
    vec3 rRay = reflect(-eyeRay, normal);

    // compute billboard normal
    vec3 bbn = normalize(cross(V1-V0, V3-V0));

    // intersect reflected ray with billboards
    // gl_FragColor.xyz = vec3(0.0f, 0.0f, 1.0f);
    float t = dot(bbn, V0-P) / dot(rRay, bbn);
    if (t < 0.0f)
        return;

    // gl_FragColor.xyz = vec3(0.0f, 1.0f, 0.0f);
    vec3 Q = P + rRay*t;
    vec3 ax = normalize(V3-V0);
    float qax = dot(Q-V0, ax) / length(V3-V0);
    vec3 ay = normalize(V1-V0);
    float qay = dot(Q-V0, ay) / length(V1-V0);
    if ((qax < 0.0f) || (qax > 1) || (qay < 0.0f) || (qay > 1))
        return;
    vec4 tcolor = texture2D(impostorTex, vec2((1 + qax) / 2, qay));
    if (tcolor.w != 0) gl_FragColor = tcolor;
*/

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
varying vec3 position;

void main() {

    // compute eye ray

    vec3 ray = normalize(position - eye);

    // reflect eye ray about normal to compute reflected ray

        vec3 reflected_ray = reflect(ray, normal);

    // intersect reflected ray with billboards

    float closest_t = 10000000;
    gl_FragColor = vec4(0.1, 0.1, 0.1, 1); // TODO: what do we do when no impostor hit?

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
