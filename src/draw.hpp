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
#include "opengl/core.hpp"
#include <type_traits>

namespace gfx {

	template<class... Ext>
	struct renderer: public Ext... {
		//virtual void render() = 0;
	};

	class with_program {
	protected:
		std::shared_ptr<gl::program> p;
	public:
		with_program(std::shared_ptr<gl::program> p):p{p}{}
		with_program(gl::program&& p):
			p{std::make_shared<gl::program>(std::move(p))}{}

		std::shared_ptr<gl::program> program() { return p; }
		inline std::shared_ptr<gl::program> prog() { return program(); }
	};

	template<class Tex>
	class with_texture {
	protected:
		std::shared_ptr<Tex> t;
	public:
		with_texture(std::shared_ptr<Tex> t):t{t}{}
		with_texture(gl::program&& t):
			t{std::make_shared<Tex>(std::move(t))}{}

		std::shared_ptr<Tex> texture() { return t; }
	};

	using with_texture_2d = with_texture<gl::texture_2d>;

	class with_vertex_array {
		protected:
		std::shared_ptr<gl::vertex_array> vao;
	public:
		with_vertex_array(std::shared_ptr<gl::vertex_array> vao):vao{vao} {}
		with_vertex_array(gl::vertex_array&& vao):
			vao{std::make_shared<gl::vertex_array>(std::move(vao))} {}
		with_vertex_array():vao{std::make_shared<gl::vertex_array>()} {}

		std::shared_ptr<gl::vertex_array> vertex_array() { return vao; }
	};

	namespace internal {
		static const unsigned dynamic = std::numeric_limits<unsigned>::max();
	}

	template<
		gl::primitive_type pt,
		unsigned Begin = internal::dynamic,
		unsigned Count = internal::dynamic
	>
	class vertex_array_drawer : public with_program, public with_vertex_array {
	public:

		template<class P, class VA>
		vertex_array_drawer(P&& p, VA&& va)
		:
		with_program(std::forward<P>(p)),
		with_vertex_array(std::forward<VA>(va)) {}

		template<class P>
		vertex_array_drawer(P&& p)
		:
		with_program(std::forward<P>(p)) {}

		void draw() {
			program()->draw_arrays(pt, Begin, Count);
		}
		
		void draw(unsigned begin, unsigned count) {
			program()->draw_arrays(pt, begin, count);
		}
	};

	template<gl::primitive_type pt>
	class vertex_array_drawer<pt, internal::dynamic, internal::dynamic>
	: public with_program, public with_vertex_array {
	public:
		template<class P, class VA>
		vertex_array_drawer(P&& p, VA&& va)
		:
		with_program(std::forward<P>(p)),
		with_vertex_array(std::forward<VA>(va)) {}

		template<class P>
		vertex_array_drawer(P&& p)
		:
		with_program(std::forward<P>(p)) {}
		/*vertex_array_drawer(std::shared_ptr<gl::program> p)
		:
		with_program(p){}*/

		void draw(unsigned begin, unsigned count) {
			program()->draw_arrays(pt, begin, count);
		}
	};

	template<
		unsigned Begin = internal::dynamic,
		unsigned Count = internal::dynamic
	>
	using triangles_drawer = 
		vertex_array_drawer<
			gl::primitive_type::triangles,
			Begin, Count
		>;
	
	using triangle_drawer = triangles_drawer<0, 3>;
	
	template <
		unsigned Begin = internal::dynamic,
		unsigned Count = internal::dynamic
	>
	using triangle_fan_drawer = 
		vertex_array_drawer<
			gl::primitive_type::triangle_fan,
			Begin, Count
		>;

	template<
		unsigned Begin = internal::dynamic,
		unsigned Count = internal::dynamic
	>
	using line_loop_drawer =
		vertex_array_drawer<gl::primitive_type::lines, Begin, Count>;

	class with_origin {
		glm::vec2 o;
	public:
		with_origin(glm::vec2 o)
		:o{o}{}

		glm::vec2 origin() { return o; }
	};

	class with_dynamic_origin {
		std::function<glm::vec2()> origin_provider;
	public:
		with_dynamic_origin()
		:with_dynamic_origin(glm::vec2()){}

		with_dynamic_origin(glm::vec2 o)
		:origin_provider{[=](){ return o; }}{}

		void origin(std::function<glm::vec2()> o) {
			origin_provider = o;
		}

		void origin(glm::vec2 o) {
			origin_provider = [=](){ return o; };
		}

		glm::vec2 origin() { return origin_provider(); }
	};


}

	/*template<>
	struct renderer<> {
		virtual void render() = 0;
	};*/

	/*class with_program {
	protected:
		std::shared_ptr<gl::program> p;
	public:
		std::shared_ptr<gl::program> program() { return p; }
	};

	template<class T>
	class with_texture {
	protected:
		std::shared_ptr<T> t;
	public:
		std::shared_ptr<T> texture() { return t; }
	};

	class shader_renderer : public renderer {
	private:
		std::shared_ptr<gl::program> program_;
	public:
		explicit shader_renderer(std::shared_ptr<gl::program> program) :program_{ program } {}
		explicit shader_renderer(gl::program&& program) :program_{ new gl::program{std::move(program)} } {};
		shader_renderer(shader_renderer&& r) {
			program_.swap(r.program_);
		}
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

		verticies_renderer(verticies_renderer&& v)
		:shader_renderer(std::move(v)) {
			vertex_array.swap(v.vertex_array);
		}

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
			gl::vertex_attribute::location pos_loc = program->attrib_location("a_pos");

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
			this->texture(texture, 0, 0, texture->width(), texture->height());
		}

		void texture(std::shared_ptr<gl::texture_2d> texture, int xo, int yo, int w, int h) {
			float tw = texture->width();
			float th = texture->height();

			uvs.data(std::vector<float> {
				xo / tw, (yo + h) / th,
					(xo + w) / tw, (yo + h) / th,
					xo / tw, yo / th,
					(xo + w) / tw, yo / th
			});
			gl::vertex_attribute::location uv_loc = program()->attrib_location("a_uv");
			vertex_array->attrib_pointer<float, 2>(uv_loc, uvs);
			vertex_array->enable_attrib_array(uv_loc);
			texture_ = texture;
		}

		void render() override {
			gl::active_texture(*texture_, 0);
			program()->uniform<int, 1>(program()->uniform_location("u_tex"), 0);
			rectangle_renderer::render();
		}
	};*/
//}