#pragma once

#include <memory>

namespace gui {

	template<class... Props>
	class view : public Props... {
	};

	class updatable {
	public:
		virtual void update() = 0;
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