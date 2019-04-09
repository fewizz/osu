#pragma once

namespace alc {

/*class context;
namespace internal {
    context wrap_context(void* raw);
}

class context {
    friend context internal::wrap_context(void* raw);
    friend void make_context_current(context&);
    void* alc_context_ptr_;
    context(void* alc_context_ptr_) :alc_context_ptr_{ alc_context_ptr_ } {}
};

context wrap_context(void* raw) {

}*/
using context = void*;

}