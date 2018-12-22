#include "gui.hpp"
#include "opengl/core.hpp"

class map_list_screen : public gui::view<gui::renderable, gui::with_dimension> {
    
public:
    void render() override {
        gl::clear(gl::clear_buffer::color);
    }        
};