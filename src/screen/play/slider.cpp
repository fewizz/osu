#include "slider.hpp"
#include "glm/vec2.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/norm.hpp"
#include <cmath>
#include <vector>

using namespace std;
using namespace glm;

vector<vec2> bezier(vector<uvec2> points) {
    vec2 a = points[0];
    vec2 b = points[1];
    vec2 c = points[2];
    vector<vec2> res;

    for(double i = 0; i <= 1; i+= (1.0/100.0)) {
        res.push_back(
            mix(mix(a, b, i), mix(b, c, i), i)
        );
    }
    return res;
}

vector<vec2> catmull(vector<uvec2> points) {
    vector<vec2> res;
    // just stolen

    for(int i = 0; i < points.size() - 1; i++) {
        vec2 a = i > 0 ? points[i - 1] : points[i];
        vec2 b = points[i];
        vec2 c = i < points.size() - 1 ? (vec2)points[i + 1] : b*2.0f - a;
        vec2 d = i < points.size() - 2 ? (vec2)points[i + 2] : c*2.0f - b;

        for(float t = 0; t <= 1; t+= (1.0/100.0)) {
            float t2 = t * t;
            float t3 = t * t2;

            res.push_back(
                 0.5f * (2.0f * b + (-a + c) * t
                + (2.0f * a - 5.0f * b + 4.0f * c - d) * t2
                + (-a + 3.0f * b - 3.0f * c + d) * t3)
            );
        }
    }
    return res;
}

vector<vec2> arc(vector<uvec2>& ps) {
    vec2 a = ps[0];
    vec2 b = ps[1];
    vec2 c = ps[2];
    float pa2 = distance2(b, c);
    float pb2 = distance2(a, c);
    float pc2 = distance2(a, b);
    float a0 = pa2*(pb2 + pc2 - pa2);
    float b0 = pb2*(pa2 + pc2 - pb2);
    float c0 = pc2*(pb2 + pa2 - pc2);

    vec2 center = (a*a0 + b*b0 + c*c0) / (a0 + b0 + c0);
    float r = length(a - center);

    auto cross = [](vec2 a, vec2 b) { return a.x*b.y-b.x*a.y; };

    bool cw = cross(b - a, b - c) > 0;
    int side_sign = (int)cw*2-1;
    float beg_angle = atan2((a-center).x, (a-center).y);
    float end_angle = atan2((c-center).x, (c-center).y);

    bool end_trunced =
        side_sign*(beg_angle - end_angle) > 0;
    end_angle +=
        2*M_PI*end_trunced*side_sign;
    float step_rad = side_sign / 100.0f;

    vector<vec2> res;
    for(float angle = beg_angle; side_sign*(end_angle-angle) > 0; angle += step_rad) {
        res.push_back({center.x + sin(angle)*r, center.y + cos(angle)*r});
    }
    return res;
}

vector<vec2> linear(vector<uvec2>& ps) {
    vector<vec2> res;

    res.push_back(ps[0]);
    for(int i = 0; i < ps.size() - 1; i++) {
        vec2 a = ps[i];
        vec2 b = ps[i + 1];

        for(int k = 0; k < 200; k++) {
            res.push_back(a + (b - a)*((float)k / 200.0f));
        }
        res.push_back(ps[i]);
    }
    return res;
}

osu::playfield::slider::slider(osu::beatmap::slider& s) {
    auto fun =
    [&](osu::beatmap::slider& s) -> vector<vec2> {
        switch(s.type) {
            case osu::beatmap::slider::type_t::arc:
                return arc(s.control_points);
            case osu::beatmap::slider::type_t::bezier:
                return bezier(s.control_points);
            case osu::beatmap::slider::type_t::catmull:
                return catmull(s.control_points);
            case osu::beatmap::slider::type_t::linear:
                return linear(s.control_points);
        }
    };

    positions = move(fun(s));
}