#include "texture_atlas.hpp"

using namespace gfx;
using namespace std;
using namespace glm;

pair<slot_id, slot>
gfx::texture_atlas::add(uvec2 td, uint8_t* data) {
    auto [id, slot] = container->occupy(td);

    sub_image(
        slot.position.x,
        slot.position.y, 
        slot.dimension[0],
        slot.dimension[1],
        gl::pixel_format::rgba,
        data
    );

    return {id, slot};
}
        
slot_id gfx::dynamic_index_provider::get_free() {
    slot_id result;

    if(occupied_slot_ids.back() == occupied_slot_ids.size() - 1) {
        result = occupied_slot_ids.size();
        occupied_slot_ids.push_back(result);
    }
    else
    for(auto s = occupied_slot_ids.begin(); s != occupied_slot_ids.end(); ++s)
    if(*s > result) {
        result = result + 1;
        occupied_slot_ids.insert(s, result);
        break;
    }

    return result;
}
        
slot_id gfx::static_index_provider::get_free() {
    for(unsigned i = 0; i < size; i++) {
        uint32_t& v0 = v[i];

        if(~v0 == 0)
            continue;

        unsigned x = 0;
        while(((v0 >> x) & 0x1) != 0)
            x++;

        v0 |= (1 << x);

        return (i << 5) | x;
    }

    throw runtime_error("free index not found");
}

#include <iostream>

pair<slot_id, slot>
gfx::fixed_slot_container::occupy(uvec2 tex_dim)
{
    slot_id id = ip.get_free();

    uvec2 coord {
        (id % slots[0]) * slot_dim[0],
        (id / slots[0]) * slot_dim[1]
    };

    std::cout << "id: " << id << "\n";
    std::cout << "coord: " << coord[0] << " " << coord[1] << "\n";

    slot s{coord, tex_dim};
    auto result = make_pair(id, s);
    map.insert(result);
    return result;
}