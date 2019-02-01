#include "slider_body_verticies.hpp"
#include "glm/geometric.hpp"
#include <algorithm>

using namespace std;
using namespace glm;

osu::playfield::slider_body_verticies
::slider_body_verticies(const vector<vec2>& positions) {
    vector<vec2> verticies;
    float r = 20;

    auto cv_perp = [=](vec2 a, vec2 b) {
        vec2 dir = normalize(b-a);
        swap(dir.x, dir.y);
        dir.y*=-1;
        return dir*r;
    };

    verticies.push_back(
        positions[0] - cv_perp(positions[0], positions[1])
    );

    for(int i = 0; i < positions.size() - 1; i++) {
        vec2 a = positions[i];
        vec2 b = positions[i+1];
        verticies.push_back(a + cv_perp(a, b));
        verticies.push_back(b - cv_perp(a, b));
    }

    int last_index = positions.size() - 1;
    verticies.push_back(
        positions[last_index]
        +
        cv_perp(positions[last_index - 1], positions[last_index])
    );

    data(verticies);
}