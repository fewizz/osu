#pragma once

#include "freetype_fwd.hpp"

namespace freetype {
	class bbox {
		friend class face;

		FT_BBox ft_bbox;

		bbox(FT_BBox raw):ft_bbox{ raw }{}
	public:
	
		inline long x_min() { return ft_bbox.xMin; }
		inline long y_min() { return ft_bbox.yMin; }
		inline long x_max() { return ft_bbox.xMax; }
		inline long y_max() { return ft_bbox.yMax; }
	};
}