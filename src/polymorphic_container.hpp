#pragma once

#include <utility>
#include <vector>

using uint = unsigned;

namespace estd {
    namespace internal {
        
        template<class T, uint index, class T0, class... Ts>
        struct index_of {
            static const uint value =
                std::is_same_v<T, T0>
                ?
                index
                :
                index_of<T, index + 1, Ts...>::value;
        };

        template<class T, uint index, class T0>
        struct index_of<T, index, T0> {
            static const uint value
                = std::is_same_v<T, T0> ? index : -1;
        };
    }

    template<class T, class... Ts>
    struct index_of {
        static const uint value = internal::index_of<T, 0, Ts...>::value;
    };

    template<class... Ts>
    class polymorphic_container : std::vector<Ts>... {
        std::vector<std::pair<uint, uint>> gate;

        template<class T>
        T& get_(std::pair<uint, uint> key) {
            return std::vector<T>::at(key.second);
        }
    public:

        //template<class... T0s>
        struct iterator {
            polymorphic_container<Ts...>& v;
            typename decltype(gate)::iterator iter;

            struct choose {
                polymorphic_container<Ts...>& v;
                std::pair<uint, uint> key;

                template<class T>
                choose&
                when(
                    void(*f)(T& val)
                )
                {
                    if(index_of<T, Ts...>::value == key.first)
                        f(
                            v.get_<T>(key)
                        );
                    return *this;       
                }
            };

            choose get() {
                return choose{ v, *iter };
            }

            choose operator*() {
                return get();
            }

            iterator operator++() {
                return {v, ++iter};
            }

            bool operator!=(iterator it) {
                return iter != it.iter;
            }
        };

        friend typename iterator::choose;

        iterator begin() {
            return iterator{ *this, gate.begin() };
        }

        iterator end() {
            return iterator{ *this, gate.end() };
        }

        template<class T>
        void push_back(T t) {
            std::vector<T>::push_back(t);
            gate.push_back(
                {
                    index_of<T, Ts...>::value,
                    std::vector<T>::size()-1
                }
            );
        }

        size_t size() {
            return gate.size();
        }
    };
}