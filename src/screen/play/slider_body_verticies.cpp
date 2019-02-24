#include "slider_body_verticies.hpp"
#include "glm/geometric.hpp"
#include <algorithm>

using namespace std;
using namespace glm;

osu::playfield::slider_body_verticies
::slider_body_verticies(const vector<vec2>& positions, float scale) {
    vector<vec2> verticies;
    float r = 64 * scale;

    auto perp_right = [&](vec2 v) {
        swap(v.x, v.y);
        v.y*=-1;
        return v;
    };

    /*verticies.push_back(
        positions.front()
        -
        r*normalize(
            perp_right(positions[1] - positions[0])
        )
    );*/

    auto get_wrapped_index = [&](int index) -> int {
        int max_index = positions.size() - 1;
        bool invert = (index / max_index) & 1;
        int m = index % max_index;

        return invert ?
            max_index - m
            :
            m; 
    };
    auto get_positions = [&](int index) -> tuple<vec2, vec2, vec2> {
        return make_tuple(
            positions[get_wrapped_index(index)],
            positions[get_wrapped_index(index + 1)],
            positions[get_wrapped_index(index + 2)]
        );
    };

    for(int i = 0; i < positions.size()*2 - 1; i++) {

        auto [a, b, c] = get_positions(i);

        vec2 p = normalize(perp_right(b-a))*r;

        verticies.push_back(a + p);
        verticies.push_back(a);
        verticies.push_back(b + p);
        verticies.push_back(b);

        float angBC =
            atan2((c-b).y, (c-b).x);
        float angBA =
            atan2((a-b).y, (a-b).x);
        if(angBC <= angBA) angBC += M_PI*2.0F;

        float angle = angBC - angBA;
        angle -= M_PI;

        float p_ang = atan2(p.y, p.x);

        for(float sub_ang = 0; sub_ang <= angle; sub_ang+=0.1F) {
            p = {cos(p_ang + sub_ang), sin(p_ang + sub_ang)};
            p*=r;
            verticies.push_back(b + p);
            verticies.push_back(b);
        }
    }

    /*int last_index = positions.size() - 1;
    verticies.push_back(
        positions[last_index]
        +
        r*normalize(perp_right(
            positions[last_index] - positions[last_index - 1]
        ))
    );*/

    data(verticies);
}