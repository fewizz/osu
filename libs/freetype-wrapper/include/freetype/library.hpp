#pragma once

#include "freetype_fwd.hpp"
#include "face.hpp"
#include <vector>
#include <memory>
#include "internal.hpp"

namespace freetype {

	class library {
		mutable std::vector<face> faces;
		void* ft_lib;

	public:
		library();

		~library();

		face& face_from_istream(std::unique_ptr<std::istream> stream) const;
	};
}