#pragma once

#include "freetype_fwd.hpp"

namespace freetype {
    class size_metrics {
        friend class face;
        
        FT_Size_Metrics ft_size_metrics;

        size_metrics(FT_Size_Metrics raw)
        :ft_size_metrics{ raw } {}
    public:
        inline unsigned short x_ppem() {
            return ft_size_metrics.x_ppem;
        }

        inline unsigned short y_ppem() {
            return ft_size_metrics.y_ppem;
        }

        inline signed long height() {
            return ft_size_metrics.height;
        }
	};
}