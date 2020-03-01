#ifndef REMOTEGL_IDENTITY_HPP_
#define REMOTEGL_IDENTITY_HPP_

#include <type_traits>

#include "standard_layout_tuple.hpp"

// variadic templates require some kind of wrapper, so we have a simple helper type
template <typename...Ts>
struct identity {};

namespace detail {
    // combine multiple identities into a larger identity
    template <typename...>
    struct identity_cat;

    template <>
    struct identity_cat<> {
        using type = identity<>;
    };

    template <typename...Ts>
    struct identity_cat<identity<Ts...>> {
        using type = identity<Ts...>;
    };

    // base case where we're left with only two identities
    template <typename...Ts,typename...Us>
    struct identity_cat<identity<Ts...>,identity<Us...>> {
        using type = identity<Ts...,Us...>;
    };

    // normal iteration case (combine the next two identities)
    template <typename...Ts,typename...Us,typename...Is>
    struct identity_cat<identity<Ts...>,identity<Us...>,Is...> {
        using type = typename identity_cat<identity<Ts...,Us...>,Is...>::type;
    };
}

// helpers struct for various identity functions
template <typename...Ts>
struct identity_helpers;

template <typename...Ts>
struct identity_helpers<identity<Ts...>> {
    // identity we are wrapping
    using identity_type = identity<Ts...>;

    // works by making a 1-tuple out of non pointers and a 0-tuple if it is a pointer, then concat
    using concrete_type = typename detail::identity_cat<
        typename std::conditional<
    	    std::is_arithmetic<Ts>::value,
            identity<Ts>,
            identity<>
        >::type...>::type;

    using concrete_type_helpers = identity_helpers<concrete_type>;

    using tuple_type = std::tuple<Ts...>;
    using tuple_type2 = remotegl::tuple<Ts...>;
};

// helper to return if the identity is concrete
template <typename...>
struct is_concrete;

template <typename...Ts>
struct is_concrete<identity<Ts...>> {
    constexpr static bool value = std::is_same<identity<Ts...>,
            typename identity_helpers<identity<Ts...>>::concrete_type>::value;
};

static_assert(is_concrete<identity<float, int>>::value, "identity<float,int> is not returning concrete, though it should be");
static_assert(is_concrete<identity<float, int*>>::value == false, "identity<float,int*> is returning concrete, though it should not be");

#endif /* REMOTEGL_IDENTITY_HPP_ */