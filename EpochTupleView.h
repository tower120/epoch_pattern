#pragma once

#include <tuple>
#include <functional>

namespace tower120::utils{

    namespace details::GuardedUpdate{

        template<std::size_t i>
        struct index_t{
            static constexpr const std::size_t value = i;
        };

        template<std::size_t ...is>
        constexpr auto index_sequence_to_tuple(std::index_sequence<is...>){
            return std::make_tuple(index_t<is>{}...);
        }

    }


    template<class ...Args>
    class EpochTupleView{
        static_assert( (std::is_base_of_v<Epoch, Args> && ...) );

        using Self = EpochTupleView<Args...>;
        using Tuple = std::tuple<Args...>;
        Tuple* tuple;

    public:
        EpochTupleView(Tuple& tuple)
            : tuple(&tuple)
        {}


        template<class Tuple, class GuardedEpochTuple>
        class GuardedUpdate{
            friend Self;

            Tuple* tuple;
            GuardedEpochTuple guard_tuple;

            GuardedUpdate(Tuple* tuple, GuardedEpochTuple guard_tuple)
                    : tuple(tuple)
                    , guard_tuple(std::move(guard_tuple)){}
        public:

            // return need update
            template<class ...Epochs>
            [[nodiscard]] bool update(const Epochs&... epochs){
                constexpr const std::size_t guard_size = std::tuple_size_v< std::decay_t<GuardedEpochTuple> >;
                constexpr const std::size_t epochs_size  = sizeof...(epochs);

                static_assert(guard_size + epochs_size == std::tuple_size_v< std::decay_t<Tuple> >);

                using namespace details::GuardedUpdate;
                constexpr const auto guard_seq  = std::make_index_sequence< guard_size >{};
                const bool guards_match =
                        std::apply([&](auto... args){
                            return
                            (... &&
                                ( std::get< args.value >(*tuple) == std::get<args.value>(guard_tuple) )
                            );
                        }, index_sequence_to_tuple(guard_seq));

                if (guard_size>0 && guards_match){
                    // all ok, update not needed
                    return false;
                }

                constexpr const auto epochs_seq   = std::make_index_sequence< epochs_size >{};
                //const std::tuple<Epochs&...> epochs_tuple{epochs...};
                const bool match =
                        std::apply([&](auto... args){
                            return
                            (... &&
                              ( std::get< args.value + guard_size >(*tuple) == /*std::get<args.value>(epochs_tuple)*/epochs )
                            );
                        }, index_sequence_to_tuple(epochs_seq));

                if (match) return false;

                // 1. update guards
                std::apply([&](auto... args){
                    (... ,
                       ( std::get< args.value >(*tuple) = std::get<args.value>(guard_tuple) )
                    );
                }, index_sequence_to_tuple(guard_seq));

                // 2. update others
                std::apply([&](auto... args){
                    (... ,
                        ( std::get< args.value + guard_size >(*tuple) = /*std::get<args.value>(epochs_tuple)*/epochs )
                    );
                }, index_sequence_to_tuple(epochs_seq));

                return true;
            }
        };

        template<class ...GuardedEpochs>
        [[nodiscard]] auto guard(const GuardedEpochs&... epochs){
            return GuardedUpdate<Tuple, std::tuple<GuardedEpochs&...>>(tuple, std::tuple<GuardedEpochs&...>{epochs...});
        }

        template<class ...Epochs>
        [[nodiscard]] bool update(const Epochs&... epochs){
            return guard().update(epochs...);
        }
    };


    template<class ...Args>
    [[nodiscard]] auto epoch_tuple(std::tuple<Args...>& tuple){
        return EpochTupleView{tuple};
    }
}