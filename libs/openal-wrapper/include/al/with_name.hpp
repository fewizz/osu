#pragma once

#include <limits>

using uint = unsigned;

namespace al {
class with_name {
protected:
	static constexpr uint invalid_name = std::numeric_limits<uint>::max();

	void invalidate_name() {
		name = invalid_name;
	}
public:
	with_name(uint name):name{name}{}
	with_name(with_name&& o) = default;
	with_name& operator=(with_name&& o) = default;

	uint name{ invalid_name };
};
}