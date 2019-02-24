#include "freetype/ftsystem.h"
#include <iostream>
#include <memory>

namespace freetype {
    class std_ft_stream : public FT_StreamRec_ {
        std::unique_ptr<std::istream> stream;
    public:
        std_ft_stream(std::unique_ptr<std::istream> s)
        :stream{std::move(s)}
        {
            std::cout << "creating.." << "\n";
            base = nullptr;
            size = 0x7FFFFFFF;
            std::cout << "telling" << "\n";
            std::cout << stream.get() << "\n";
            pos = stream->tellg();
            read = [](
                FT_Stream       stream,
                unsigned long   offset,
                unsigned char*  buffer,
                unsigned long   count) -> unsigned long
            {
                auto s = ((std_ft_stream*)stream)->stream.get(); 
                s->seekg(offset, std::ios::beg);
                if(count == 0)
                    return 0;
                s->read((char*)buffer, count);
                return s->gcount();
            };

            close = [](FT_Stream  stream) {
                std::cout << "destroy ft_stream" << "\n";
                ((std_ft_stream*)stream)->stream.release();
                
            };
        }
    };
}