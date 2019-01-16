#pragma once

namespace prop {

    template<class T>
    struct pressable {
        virtual T is_pressed() = 0;
        virtual void set_pressed(T val) = 0;
    };

    template<class T=bool, T Val=false>
    struct with_pressable_state : public pressable<T> {
        virtual void set_pressed(bool val) override {this->val = val;}
        virtual T is_pressed() override { return val; }
    protected:
        T val = Val;
    };
}