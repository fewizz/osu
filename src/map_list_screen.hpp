#pragma once

#include "gui.hpp"
#include "opengl/core.hpp"
#include <vector>
#include <filesystem>
#include <memory>
#include "text_renderer.hpp"
#include "opengl/shader.hpp"
#include "glm/mat4x4.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "main.hpp"
#include "GLFW/glfw3.h"

class map_list_screen : public gui::view<gui::renderable>
{
    std::vector<gfx::text_renderer> name_renderers;
    class outline_renderer : public gfx::shader_renderer
    {
      public:
        using gfx::shader_renderer::shader_renderer;
        inline void render() override
        {
            program()->draw_arrays(gl::primitive_type::line_loop, 0, 4);
        }
    } outline;
    unsigned current_map = 0;

  public:
    map_list_screen(freetype::face &face)
        : name_renderers{create_name_renderers(face)},
          outline{gl::program{
              gl::vertex_shader{R"(
            #version 130
            #define H 60
            uniform mat4 u_mat;
            void main() {
                vec2 positions[4] = vec2[](
                    vec2(-1, 0),
                    vec2(2, 0),
                    vec2(2, H),
                    vec2(-1, H)
                );
                gl_Position = u_mat * vec4(positions[gl_VertexID] - vec2(0, 10), 0, 1);
                gl_Position.x = positions[gl_VertexID].x;
            }
        )"},
              gl::fragment_shader{R"(
            #version 130
            void main() {
                gl_FragColor = vec4(1);
            }
        )"}}}
    {
        osu::window->set_key_callback([&](int key, int scancode, int action, int mods) {
            if (current_map > 0 && key == GLFW_KEY_UP)
                current_map--;
            if (current_map < name_renderers.size() - 1 && key == GLFW_KEY_DOWN)
                current_map++;
        });
    }

    std::vector<gfx::text_renderer> create_name_renderers(freetype::face &face)
    {
        std::vector<std::string> names;

        for (auto &de : std::filesystem::directory_iterator("Songs"))
        {
            names.push_back((++de.path().begin())->string());
        }

        auto program = std::make_shared<gl::program>(
            gl::vertex_shader{R"(
#version 420 core
uniform mat4 u_mat;
in vec2 a_position;
in vec2 a_uv;
out vec2 uv_vs;
void main() {
	uv_vs = a_uv;
	gl_Position = u_mat * vec4(a_position, 0, 1);
}
)"},
            gl::fragment_shader{R"(
#version 420 core
uniform sampler2D u_atlas;
in vec2 uv_vs;
out vec4 color;
void main() {
	color = texture(u_atlas, uv_vs);
}
)"});

        std::vector<gfx::text_renderer> result;
        for (std::string s : names)
        {
            result.push_back({s, face, program});
        }

        return result;
    }

    void render() override
    {
        glm::mat mat = glm::ortho<float>(
            0,
            osu::window->get_framebuffer_size().first,
            -osu::window->get_framebuffer_size().second,
            0);

        mat = glm::translate(mat, {0, -100, 0});
        for (int i = 0; i < name_renderers.size(); i++)
        {
            auto &r = name_renderers[i];
            r.program()->uniform<float, 4, 4>(
                r.program()->uniform_location("u_mat"),
                mat);
            r.render();

            if (i == current_map)
            {
                outline.program()->uniform<float, 4, 4>(
                    outline.program()->uniform_location("u_mat"),
                    mat);
            }

            mat = glm::translate(mat, {0, -80, 0});
        }
        outline.render();
    }
};