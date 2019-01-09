#pragma once

#include "freetype_fwd.hpp"

namespace freetype {
    class glyph_slot;

    class glyph_metrics {
        friend glyph_slot;
        FT_Glyph_Metrics ft_metrics;
        glyph_metrics(FT_Glyph_Metrics ft) : ft_metrics{ ft } {}
    public:
        glyph_metrics() = default;
        glyph_metrics(const glyph_metrics&) = default;
        glyph_metrics& operator=(const glyph_metrics&) = default;

        inline long width() {return ft_metrics.width;}
        inline long height() {return ft_metrics.height;}
        inline long horizontal_bearing_x()
        {return ft_metrics.horiBearingX;}
        inline long horizontal_bearing_y()
        {return ft_metrics.horiBearingY;}
        inline long horizontal_advance() 
        {return ft_metrics.horiAdvance;}
        inline long vertical_bearing_x()
        {return ft_metrics.vertBearingX;}
        inline long vertical_bearing_y()
        {return ft_metrics.vertBearingY;}
        inline long vertical_advance()
        {return ft_metrics.vertAdvance;}
    };
}