#pragma once

#include "opengl/texture.hpp"
#include <vector>
#include <list>
#include <cmath>
#include <utility>
#include "glm/vec2.hpp"
#include <map>
#include <numeric>

namespace gfx {
	class slot_id {
		int32_t val;
	public:
		slot_id():val{-1}{}
		slot_id(int32_t v):val{v}{}
		slot_id& operator=(const slot_id&) = default;

		operator int32_t() const {
			return val;
		}

		bool valid() const { return val != -1; }
	};

	struct slot {
		const glm::uvec2 position;
		const glm::uvec2 dimension;
	public:
		slot():position{0}, dimension{0} {}
		slot(glm::uvec2 p, glm::uvec2 d)
		:position{p}, dimension{d} {}
		slot(const slot&) = default;
		slot& operator=(const slot&) = default;
		slot(slot&&) = default;
	};

	class slot_container {
	public:
		virtual std::pair<slot_id, slot>
		occupy(glm::uvec2 tex_dim) = 0;

		virtual slot get_slot(slot_id id) = 0;

		virtual void free(slot_id) = 0;
		virtual ~slot_container() = default;
	};

	class texture_atlas : public gl::texture_2d {
		std::unique_ptr<slot_container> container;
 	public:

		template<class SC>
		texture_atlas(glm::uvec2 td, SC&& o )
		: container{std::make_unique<SC>(std::move(o))}
		{
    		image<uint8_t>(
        		gl::internal_format::rgba8,
        		0,
        		td[0],
        		td[1],
        		gl::pixel_format::rgba,
        		(uint8_t*)nullptr
    		);
		}

		texture_atlas(texture_atlas&& r) = default;
		texture_atlas& operator=(texture_atlas&& r) = default;

		std::pair<slot_id, slot>
		add(glm::uvec2 td, uint8_t* data);

		inline slot get(slot_id id) {
			return container->get_slot(id);
		}
	};

	class index_provider {
		virtual slot_id get_free() = 0;
		virtual void free(slot_id s) = 0;
	};

	class dynamic_index_provider: public index_provider {
		std::list<slot_id> occupied_slot_ids;
	public:
		slot_id get_free() override;

		inline void free(slot_id s) override {
			occupied_slot_ids.remove(s);
		}
	};

	class static_index_provider : public index_provider {
		std::unique_ptr<uint32_t[]> v;
		unsigned size;
	public:
		static_index_provider(unsigned indexes):
		v{std::make_unique<uint32_t[]>(indexes / 32 + 1)},
		size{indexes / 32 + 1} 
		{}

		slot_id get_free() override;

		inline void free(slot_id s) override {
			v[s >> 5] &= (0 << (s & 0b11111));
		}
	};

	class fixed_slot_container : public slot_container {
		glm::uvec2 slot_dim;
		glm::uvec2 slots;

		std::map<slot_id, slot> map;
		static_index_provider ip;
	public:
		fixed_slot_container (
			glm::uvec2 slot,
			unsigned slots_x,
			unsigned slots_y
		):
		slot_dim{slot},
		slots{slots_x, slots_y},
		ip{slots_x*slots_y}
		{}

		std::pair<slot_id, slot>
		occupy(glm::uvec2 tex_dim) override;

		inline slot get_slot(slot_id id) override {
			return map[id];
		};

		inline void free(slot_id id) override {
			ip.free(id);
		}
	};

	//using fixed_texture_atlas = texture_atlas<fixed_slot_occupator>;
}