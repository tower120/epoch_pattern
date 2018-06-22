#pragma once

namespace tower120::utils{
    class Epoch{
        using Value = unsigned int;
        Value value = 1;
        explicit constexpr Epoch(Value value) : value(value) {}
    public:
        Epoch() = default;

        Epoch& operator++() {
            value++;
            return *this;
        }
        Epoch operator++(int) {
            return Epoch(value+1);
        }

        bool operator==(const Epoch& other) const {
            return value == other.value;
        }
        bool operator!=(const Epoch& other) const {
            return value != other.value;
        }

        // return need update
        [[nodiscard]]
        bool update(const Epoch& other){
            if (value == other.value) return false;
            value = other.value;
            return true;
        }

        Value count() const {
            return value;
        }

        constexpr static Epoch Pre(){
            return Epoch{0};
        }
    };
}