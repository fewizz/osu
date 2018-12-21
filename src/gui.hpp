#pragma once

namespace gui {

	class view {
	public:
		virtual void update() = 0;
		virtual void render() = 0;
	};

	class rectangle_view : public virtual view {

	};

	class pressable {
		virtual void press() = 0;
		virtual void unpress() = 0;
	};

	
}