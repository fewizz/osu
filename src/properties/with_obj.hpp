#pragma once

#include <memory>

namespace prop {

    template<class T>
    class with_shared_object {
        std::shared_ptr<T> o;

    public:
        with_shared_object() = default;

        with_shared_object(T&& t)
        :o{std::make_shared<T>(std::move(o))}{}

        with_shared_object(std::shared_ptr<T> o)
        :o{o}{}

        T& ref() { return *o; }
        std::shared_ptr<T> shared() { return o; }
    };

    template<class T>
    class with_unique_object {
        std::unique_ptr<T> o;

    public:
        with_unique_object() = default;

        with_unique_object(T&& t)
        :o{std::make_unique<T>(std::move(o))}{}

        with_unique_object(std::unique_ptr<T> o)
        :o{o}{}

        T& ref() { return *o; }
        std::unique_ptr<T>& unique() { return o; }
    };

    template<class T>
    class with_owned_object {
        T o;
    public:
        with_owned_object() = default;
        with_owned_object(T&& o): o{std::move(o)}{}
        T& ref() { return o; }
    };

    template<class T>
    class with_object_ref {
        T& o;
    public:
        with_object_ref(T& o): o{o}{}
        T& ref() { return o; }
    };
}