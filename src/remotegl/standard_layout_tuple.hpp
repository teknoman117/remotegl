#ifndef REMOTEGL_STANjDARD_LAYOUT_TUPLE_HPP_
#define REMOTEGL_STANjDARD_LAYOUT_TUPLE_HPP_

#include <cstddef>
#include <utility>

namespace remotegl {

    // standard layout mini-tuple
    template <typename...>
    struct tuple;

    template <typename T>
    struct tuple<T> {
        T leaf;

        tuple() = default;
        tuple(T leaf) : leaf(leaf) {}
    };

    template <typename T0, typename...Types>
    struct tuple<T0, Types...> {
        T0 leaf;
        tuple<Types...> next;

        tuple() = default;
        tuple(T0 leaf, Types...leaves)
            : leaf(leaf), next(std::forward<Types>(leaves)...) {}
    };

    static_assert(std::is_standard_layout<tuple<float, int, int*>>::value, "tuple isn't standard layout");

    namespace detail {
        template <std::size_t N, typename Enable, typename...>
        struct tuple_element_impl;

        template <std::size_t N>
        struct tuple_element_impl<N, void, tuple<>> {};

        template <std::size_t N, typename T0, typename...Types>
        struct tuple_element_impl<N, typename std::enable_if<N == 0>::type, tuple<T0, Types...>> {
            using type = T0;
        };

        template <std::size_t N, typename T0, typename...Types>
        struct tuple_element_impl<N, typename std::enable_if<N != 0>::type, tuple<T0, Types...>>
                : tuple_element_impl<N-1, void, tuple<Types...>> {};
    }

    template <std::size_t N, typename Tuple>
    using tuple_element = detail::tuple_element_impl<N, void, Tuple>;

    // get the size of the tuple
    template <typename...Types>
    struct tuple_size;

    template <typename...Types>
    struct tuple_size<tuple<Types...>> : std::integral_constant<std::size_t, sizeof...(Types)> { };

    //static_assert(std::is_same<short, tuple_element<4, tuple<float, long, double, int, short>>::type>::value, "oopsy poopsy");

    namespace detail {
        template <std::size_t N, typename Tuple, typename Enable = void>
        struct get_impl;

        template<std::size_t N, typename Tuple>
        struct get_impl<N, Tuple, typename std::enable_if<N==0>::type> {
            static inline typename tuple_element<N, Tuple>::type& get(Tuple& tuple) {
                return tuple.leaf;
            }
        };

        template<std::size_t N, typename Tuple>
        struct get_impl<N, Tuple, typename std::enable_if<N!=0>::type> {
            static inline typename tuple_element<N, Tuple>::type& get(Tuple& tuple) {
                return get_impl<N-1,decltype(tuple.next)>::get(tuple.next);
            }
        };
    }

    template<std::size_t N, typename Tuple>
    static inline auto& get(Tuple& tuple) {
        return detail::get_impl<N, Tuple>::get(tuple);
    }

    

}

#endif /* REMOTEGL_STANjDARD_LAYOUT_TUPLE_HPP_ */