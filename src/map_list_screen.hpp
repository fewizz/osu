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
#include "main.hpp"

class map_list_screen : public gui::view<gui::renderable> {
    gfx::text_renderer rend;
public:
    map_list_screen(freetype::face& face):rend{create_tex_renderer(face)}{}

    static gfx::text_renderer create_tex_renderer(freetype::face& face) {
        std::vector<std::string> names;

        for(auto& de : std::filesystem::directory_iterator("Songs")) {
            names.push_back(de.path().string());
        }

        std::string result;
        for(std::string s : names) {
            result += s + "\n";
        }
        std::cout << result << "\n";
        std::cout << "program" << "\n";
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
)"}
        );
        return {result, face, program};
    }

    void render() override {
        rend.program()->uniform<float, 4, 4>(
            rend.program()->uniform_location("u_mat"),
            glm::ortho<float>(
                osu::window->get_framebuffer_size().first / -2.0f,
                osu::window->get_framebuffer_size().first / 2.0f,
                osu::window->get_framebuffer_size().second / -2.0f,
                osu::window->get_framebuffer_size().second / 2.0f
            )
        );
        rend.render();
    }
};