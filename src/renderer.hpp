#pragma once
#include <vector>
#include <cmath>
#include <iterator>
#include <memory>
#include <functional>
#include <array>
#include "glm/mat4x4.hpp"
#include "opengl/program.hpp"
#include "opengl/vertex_array.hpp"
#include "opengl/texture.hpp"

namespace gfx {

	struct renderer {
		virtual void render() = 0;
	};

	class shader_renderer : public renderer {
	private:
		std::shared_ptr<gl::program> program_;
	public:
		explicit shader_renderer(std::shared_ptr<gl::program> program) :program_{ program } {}
		explicit shader_renderer(gl::program&& program) :program_{ new gl::program{std::move(program)} } {};
		shader_renderer(const shader_renderer&) = delete;
		shader_renderer& operator=(const shader_renderer&) = delete;

		const gl::program* const program() {
			return (const gl::program* const) &* program_;
		}

		virtual ~shader_renderer() {}
	};

	class verticies_renderer : public shader_renderer {
	protected:
		std::shared_ptr<gl::vertex_array> vertex_array;
	public:
		verticies_renderer(std::shared_ptr<gl::program> program)
			:shader_renderer(program), vertex_array{ std::make_shared<gl::vertex_array>() } {}

		verticies_renderer(gl::program&& program)
			:shader_renderer(std::move(program)),
			vertex_array{ std::make_shared<gl::vertex_array>() } {}

		verticies_renderer(gl::program&& program, gl::vertex_array&& vao)
			:shader_renderer(std::move(program)),
			vertex_array{ std::make_shared<gl::vertex_array>(std::move(vao)) } {}

		std::shared_ptr<gl::vertex_array> get_vertex_array() {
			return vertex_array;
		}
	};

	class rectangle_renderer : public verticies_renderer {
		gl::array_buffer positions;

	public:
		rectangle_renderer(float x, float y, float w, float h, std::shared_ptr<gl::program> program, gl::buffer_usage usage = gl::buffer_usage::static_draw)
			:verticies_renderer(program)
		{
			positions.data(std::vector<float>{
				x, y,
					x + w, y,
					x, y + h,
					x + w, y + h,
			}, usage);
			gl::vertex_attribute::location pos_loc = program->attrib_location("a_position");

			vertex_array->attrib_pointer<float, 2>(pos_loc, positions);
			vertex_array->enable_attrib_array(pos_loc);
		}

		rectangle_renderer(float x, float y, float w, float h, gl::program&& program, gl::buffer_usage usage = gl::buffer_usage::static_draw)
			:rectangle_renderer(x, y, w, h, std::make_shared<gl::program>(std::move(program))) {}

		rectangle_renderer(std::shared_ptr<gl::program> program)
			:rectangle_renderer(0, 0, 0, 0, program, gl::buffer_usage::dynamic_draw) {}
		rectangle_renderer(gl::program&& program)
			:rectangle_renderer(std::make_shared<gl::program>(std::move(program))) {}

		void update(float x, float y, float w, float h) {
			positions.sub_data(0, std::vector<float>{
				x, y,
					x + w, y,
					x, y + h,
					x + w, y + h,
			});
		}

		void render() override {
			program()->draw_arrays(gl::primitive_type::triangle_strip, 0, 4, *vertex_array);
		}
	};

	class textured_rectangle_renderer : public rectangle_renderer {
		std::shared_ptr<gl::texture_2d> texture_;
		gl::array_buffer uvs;
	public:
		using rectangle_renderer::rectangle_renderer;

		void texture(std::shared_ptr<gl::texture_2d> texture) {
			texture_ = texture;
		}

		void texture(gl::texture_2d&& texture, int xo, int yo, int w, int h) {
			float tw = texture.width();
			float th = texture.height();

			uvs.data(std::vector<float> {
				xo / tw, (yo + h) / th,
					(xo + w) / tw, (yo + h) / th,
					xo / tw, yo / th,
					(xo + w) / tw, yo / th
			});
			gl::vertex_attribute::location uv_loc = program()->attrib_location("a_uv");
			vertex_array->attrib_pointer<float, 2>(uv_loc, uvs);
			vertex_array->enable_attrib_array(uv_loc);
			texture_ = std::make_shared<gl::texture_2d>(std::move(texture));
		}

		void render() override {
			gl::active_texture(*texture_, 0);
			program()->uniform<int, 1>(program()->uniform_location("u_texture"), 0);
			rectangle_renderer::render();
		}
	};
}