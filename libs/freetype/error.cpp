#include <string>

namespace freetype {
	namespace internal {
		#define FT_ERRORDEF_(e, v, s) e = v,
		#define FT_NOERRORDEF_(e, v, s) e = v,
		enum error {
			#include "freetype/fterrdef.h"
		};

		#define FT_ERRORDEF_(e, v, s) if( code == v ) return s ;
		#define FT_NOERRORDEF_(e, v, s) if( code == v ) return s ;
		inline std::string get_error_name(unsigned code) {
		#include "freetype/fterrdef.h"
			throw std::runtime_error("undefined freetype error code");
		}
		#undef FT_ERRORDEF_
		#undef FT_NOERRORDEF_

		void check_for_errors(unsigned error) {
			if (error != Ok) {
				std::string error_name = get_error_name(error);
				throw std::runtime_error("freetype error: " + error_name);
			}
		}
	}
}