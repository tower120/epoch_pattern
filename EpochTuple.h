#pragma once

#include <tuple>
#include "Epoch.h"
#include "EpochTupleView.h"

// For now, have no advantages over std::tuple<Epoch...>

namespace tower120::utils{

    template<class ...Args>
    class EpochTuple{
        static_assert( (std::is_base_of_v<Epoch, Args> && ...) );

        using Self = EpochTuple<Args...>;
        using Tuple = std::tuple<Args...>;
        Tuple tuple;

        EpochTuple(Tuple&& tuple) : tuple(std::move(tuple)){}
    public:
        EpochTuple() = default;

        template<auto i>
        const auto& get() const{
            return std::get<i>(tuple);
        }
        template<class T>
        const auto& get() const{
            return std::get<T>(tuple);
        }


        template<class ...GuardedEpochs>
        [[nodiscard]] auto guard(const GuardedEpochs&... guard_epochs){
            return epoch_tuple(tuple).guard(guard_epochs...);
        }

        [[nodiscard]] bool update(const Args&... epochs){
            return epoch_tuple(tuple).update(epochs...);
        }

        bool operator==(const EpochTuple& other) const {
            return tuple == other.tuple;
        }
        bool operator!=(const EpochTuple& other) const {
            return tuple != other.tuple;
        }

        const Tuple& as_tuple() const{
            return tuple;
        }

        constexpr static EpochTuple<Args...> Pre(){
            return {{Args::Pre()...}};
        }
    };

}