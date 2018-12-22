#pragma once

#include <memory>

namespace gui {

	class renderable {
	public:
		virtual void render() = 0;
	};

	template<class... Props>
	class view : public Props... {
	};

	class updatable {
	public:
		virtual void update() = 0;
	};

	class dimension {
	public:
		virtual unsigned get_w();
		virtual unsigned get_h();
		virtual void set_w(unsigned w);
		virtual void set_h(unsigned h);
	};

	typedef dimension with_dimension;

	class dimension_state : public virtual dimension {
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

	template<class Dim = dimension, class... Props>
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
		void set_h(unsigned h) override {dim->set_h(h);};*/
	};

	class pressable {
	protected:
		pressable(){}
	public:
		virtual void press();
		virtual void unpress();
	};

	class press_state : public virtual pressable {
	protected:
		bool s_pressed;
	public:
		press_state(bool state):s_pressed{state}{}
		press_state():press_state(false){}

		void press() override {s_pressed = true;}
		void unpress() override {s_pressed = false;}
	};

	template<class Dim = dimension, class Press = pressable, class... Props>
	class button : public rec_view<Dim, Press, Props...> {
		
	};
}