#pragma once

#include <memory>
#include "properties/drawable.hpp"
#include "properties/updatable.hpp"
#include "properties/with_size.hpp"
#include "glm/vec2.hpp"
#include "glfw/window.hpp"

namespace osu {
	extern std::unique_ptr<glfw::window> window;
}

namespace gui {

	template<class... Props>
	class view : public Props... {
	};

	template<class... Props>
	struct screen :
	public view<
		prop::updatable,
		prop::drawable<>,
		prop::with_size<glm::uvec2, unsigned>,
		Props...
	> {
		glm::uvec2 get_size() override {
			return osu::window->framebuffer_size<glm::uvec2>();
		}

	};


	/*class dimension_state : public virtual with_dynamic_dimension {
	protected:
		unsigned dim_w;
		unsigned dim_h;
	public:
		dimension_state(unsigned w, unsigned h):dim_w{w}, dim_h{h}{}

		unsigned get_w() override { return dim_w;}
		unsigned get_h() override { return dim_h;}
		void set_w(unsigned w) override {dim_w = w;}
		void set_h(unsigned h) override {dim_h = h;};
	};

	template<class Dim, class... Props>
	class rec_view : public view<Dim, Props...> {
	protected:
		//dimension* dim;
	public:
		rec_view(){};
		//rec_view(std::unique_ptr<dimension> dim): dim{std::move(dim)}{}
		//rec_view(dimension* dim): dim{dim}{}

		//dimension* get_dimension() { return dim; }
		//void set_dimension(dimension* dim) {
		//	this->dim = dim;
		//}

		/*unsigned get_w() override { return dim->get_w();}
		unsigned get_h() override { return dim->get_h();}
		void set_w(unsigned w) override {dim->set_w(w);}
		void set_h(unsigned h) override {dim->set_h(h);};
	};*/

}