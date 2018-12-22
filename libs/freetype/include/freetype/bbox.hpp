#pragma once

namespace freetype {
	class bbox {
	public:
		bbox(int x_min, int y_min, int x_max, int y_max)
			:x_min{ x_min }, y_min{ y_min }, x_max{ x_max }, y_max{ y_max }{}

		const int x_min, y_min, x_max, y_max;
	};
}