#include "hit_object.hpp"
#include "gui/circle_drawer.hpp"
#include "gui/slider_drawer.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/vec3.hpp"
#include <memory>

using namespace std;

namespace internal {
    static std::unique_ptr<circle_drawer> cd;
    static std::unique_ptr<slider_drawer> sd;
}

void osu::circle::draw(glm::mat4 lb) {
    if(!internal::cd)
        internal::cd = make_unique<circle_drawer>();

    internal::cd->draw(
        glm::translate(lb, glm::vec3{position.x, position.y, 0})
    );
}

void osu::slider::draw(glm::mat4 lb) {
    if(!internal::sd)
        internal::sd = make_unique<slider_drawer>();

    for(auto& p : positions) {
        internal::sd->draw(
            glm::translate(lb, glm::vec3{p.x, p.y, 0})
        );
    }
}