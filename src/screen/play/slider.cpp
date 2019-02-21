#include "slider.hpp"
#include "glm/vec2.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/norm.hpp"
#include <cmath>
#include <vector>
#include <algorithm>

using namespace std;
using namespace glm;
using cp_cursor = vector<uvec2>::const_iterator;

vec2 bezier(vector<vec2>& ps, float t) {
    if(ps.size() == 1) return ps[0];
    vector<vec2> ps0;
    for(int i = 0; i < ps.size() - 1; i++) {
        ps0.push_back(mix(ps[i], ps[i + 1], t));
    }
    return bezier(ps0, t);
}

void bezier(cp_cursor begin, cp_cursor end, vector<vec2>& res) {
    //if(end - begin != 3)
    //    throw runtime_error("not valid contol points: " + to_string(end - begin));
    //vec2 a = begin[0];
    //vec2 b = begin[1];
    //vec2 c = begin[2];
    vector<vec2> copy(begin, end);

    for(double i = 0; i <= 1; i+= (1.0/100.0)) {
        res.push_back(
            bezier(copy, i)
        );
    }
}

void catmull(cp_cursor begin, cp_cursor end, vector<vec2>& res) {
    // just stolen

    for(cp_cursor cur = begin; cur < end - 1; cur++) {
        vec2 a = cur != begin ? cur[-1] : *cur;
        vec2 b = *cur;
        vec2 c = cur < end - 1 ? (vec2)cur[1] : b*2.0f - a;
        vec2 d = cur < end - 2 ? (vec2)cur[2] : c*2.0f - b;

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
}

void arc(cp_cursor begin, cp_cursor end, vector<vec2>& res) {
    if(end - begin != 3)
        throw runtime_error("not valid contol points: " + to_string(end - begin));
    vec2 a = begin[0];
    vec2 b = begin[1];
    vec2 c = begin[2];
    float pa2 = distance2(b, c);
    float pb2 = distance2(a, c);
    float pc2 = distance2(a, b);
    float a0 = pa2*(pb2 + pc2 - pa2);
    float b0 = pb2*(pa2 + pc2 - pb2);
    float c0 = pc2*(pb2 + pa2 - pc2);

    vec2 center = (a*a0 + b*b0 + c*c0) / (a0 + b0 + c0);
    float r = length(b - center);

    auto cross = [](vec2 a, vec2 b) { return a.x*b.y-b.x*a.y; };

    bool cw = cross(b - a, c - b) > 0;
    int side_sign = (int)cw*2-1;
    float beg_angle = atan2((a-center).y, (a-center).x);
    float end_angle = atan2((c-center).y, (c-center).x);

    bool end_trunced =
        side_sign*(beg_angle - end_angle) > 0;

    end_angle +=
        2.0F*M_PI*end_trunced*float(side_sign);
    float step_rad = 0.1F * side_sign;

    int added = 0;
    for(float angle = beg_angle; side_sign*(end_angle-angle) >= 0; angle += step_rad) {
        res.push_back({center.x + cos(angle)*r, center.y + sin(angle)*r});
        added++;
    }
    
    if(added == 1)
        res.pop_back();
    if(added < 2) {
        res.push_back(a);
        res.push_back(b);
        res.push_back(c);
    }
}

void linear(cp_cursor begin, cp_cursor end, vector<vec2>& res) {
    for_each(begin, end, [&](const uvec2& cp){ res.push_back(cp); });
}

osu::playfield::slider::slider(const osu::beatmap::slider& s)
:slider_info{s}
{

    auto fun =
    [&](cp_cursor begin, cp_cursor end) {
        switch(s.type) {
            case osu::beatmap::slider::type_t::arc:
                return arc(begin, end, positions);
            case osu::beatmap::slider::type_t::bezier:
                return bezier(begin, end, positions);
            case osu::beatmap::slider::type_t::catmull:
                return catmull(begin, end, positions);
            case osu::beatmap::slider::type_t::linear:
                return linear(begin, end, positions);
        }
    };

    auto b = s.control_points.begin();
    auto e = s.control_points.begin() + 1;

    for(;e <= s.control_points.end(); e++) {
        if(e == s.control_points.end() || *(e - 1) == *e) {
            fun(b, e);
            b = e;
        }
    }
}