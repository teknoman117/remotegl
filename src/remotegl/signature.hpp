#ifndef REMOTEGL_SIGNATURE_HPP_
#define REMOTEGL_SIGNATURE_HPP_

#include "identity.hpp"
#include "standard_layout_tuple.hpp"

namespace detail {
    // helper to get useful parts of function signatures
    template <typename R, typename...Args>
    struct signature_base {
        using return_type = R;
        using arguments_type = std::tuple<Args...>;
        using arguments_type2 = remotegl::tuple<Args...>;
        using identity_type = identity<Args...>;
        using concrete_arguments_type = typename identity_helpers<identity_type>::concrete_type_helpers::tuple_type;
        using concrete_arguments_type2 = typename identity_helpers<identity_type>::concrete_type_helpers::tuple_type2;

        constexpr static const std::size_t arguments_count = sizeof...(Args);
    };
}

// helper to decompose various kinds of types representing function signatures
template <typename... Func>
struct signature;

// get information from a function pointer
template <typename R, typename... Args>
struct signature<R(*)(Args...)> : detail::signature_base<R, Args...> {};

// get information from a function signature
template <typename R, typename... Args>
struct signature<R(Args...)> : detail::signature_base<R, Args...> {};

#endif /* REMOTEGL_SIGNATURE_HPP_ */
