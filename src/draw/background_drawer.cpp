#include "background_drawer.hpp"
#include "glm/vec2.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "main.hpp"

using namespace glm;

void gfx::background_drawer::draw(glm::mat4 center) {
    vec2 fb_size = osu::window->framebuffer_size<vec2>();
    vec2 tex_size = tex().size<vec2>();

    float w_ratio = fb_size[0] / fb_size[1];
    float t_ratio = tex_size[0] / tex_size[1];

    vec2 scaled_size =
        tex_size
        *
        (w_ratio > t_ratio ?
            fb_size[0] / tex_size[0]
            :
            fb_size[1] / tex_size[1]
        );
    
    prog().uniform("u_dim", scaled_size);

    prog().uniform(
        "u_mat",
        translate(center, {-scaled_size[0] / 2.0f, -scaled_size[1] / 2.0f, 0})
    );

    gl::active_texture(tex(), 0);
    prog().uniform("u_tex", 0);

    prog().draw_arrays(gl::primitive_type::triangle_fan, 0, 4);
}