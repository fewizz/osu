#pragma once

#ifdef AL_CHECK_FOR_ERROR
#define AL_CHECK_FOR_ERROR_FN check_for al::internal::check_for_error();
#else
#define AL_CHECK_FOR_ERROR_FN
#endif