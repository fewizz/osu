#pragma once

#include "freetype_fwd.hpp"

namespace freetype {
    class glyph_slot;

    class bitmap {
        friend glyph_slot;
    
        FT_Bitmap ft_bitmap;

        bitmap(FT_Bitmap raw) : ft_bitmap{ raw } {}
    public:
        bitmap(const bitmap&) = delete;

        template<class T>
        inline T* data() { return (T*)(ft_bitmap.buffer); }
        inline unsigned rows() { return ft_bitmap.rows; }
        inline unsigned width() { return ft_bitmap.width; }
        inline unsigned pitch() { return ft_bitmap.pitch; }
    };
}