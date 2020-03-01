#ifndef REMOTEGL_PROTOCOL_HPP_
#define REMOTEGL_PROTOCOL_HPP_

#include <array>
#include <tuple>

#include <sys/uio.h>

#include "signature.hpp"
#include "size_hints.hpp"
#include "standard_layout_tuple.hpp"

enum class RemoteGLFunction
{
    // protocol functions
    SYNC,

    // window manager functions
    WindowHint,
    WindowCreate,
    WindowDestroy,
    SwapBuffers,
    SwapInterval,

    // opengl functions
    OGL_glClear,
    OGL_glClearColor,
    OGL_glGetString,
    OGL_glViewport,
    OGL_glEnable,
    OGL_glDisable,
    OGL_glCreateShader,
    OGL_glShaderSource,
    OGL_glCompileShader,
    OGL_glGetShaderiv,
    OGL_glGetShaderInfoLog,
    OGL_glDeleteShader,
    OGL_glCreateProgram,
    OGL_glDeleteProgram,
    OGL_glUseProgram,
    OGL_glAttachShader,
    OGL_glLinkProgram,
    OGL_glGetProgramiv,
    OGL_glGetProgramInfoLog,
    OGL_glGetUniformLocation,
    OGL_glGenBuffers,
    OGL_glBindBuffer,
    OGL_glBufferData,
    OGL_glBufferSubData,
    OGL_glVertexAttribPointer,
    OGL_glEnableVertexAttribArray,
    OGL_glDisableVertexAttribArray,
    OGL_glDrawArrays,

    OGL_glUniform1i,
    OGL_glUniform2i,
    OGL_glUniform3i,
    OGL_glUniform4i,
    OGL_glUniform1iv,
    OGL_glUniform2iv,
    OGL_glUniform3iv,
    OGL_glUniform4iv,
    OGL_glUniform1f,
    OGL_glUniform2f,
    OGL_glUniform3f,
    OGL_glUniform4f,
    OGL_glUniform1fv,
    OGL_glUniform2fv,
    OGL_glUniform3fv,
    OGL_glUniform4fv,
    OGL_glUniformMatrix2fv,
    OGL_glUniformMatrix3fv,
    OGL_glUniformMatrix4fv,
};

/*namespace detail
{
    template <typename Tuple, typename Res = std::tuple<>>
    struct nopointer_tuple_helper;


    template<typename Res>
    struct nopointer_tuple_helper<std::tuple<>, Res>
    {
        using type = Res;
    };

    template<typename T, typename... Ts, typename... TRes>
    struct nopointer_tuple_helper<std::tuple<T, Ts...>, std::tuple<TRes...>> :
        std::conditional<std::is_arithmetic<T>::value,
            nopointer_tuple_helper<std::tuple<Ts...>, std::tuple<TRes...,T>>,
            nopointer_tuple_helper<std::tuple<Ts...>, std::tuple<TRes...>>
        >::type
    {};
}

template <typename...Ts> struct nopointer_tuple
{
    using type = typename detail::nopointer_tuple_helper<std::tuple<Ts...>>::type;
};

static_assert(std::is_same<std::tuple<int, float>,
        typename nopointer_tuple<int, int*, float>::type>::value, "oopsy poopsy");*/

template <typename Func>
struct receiver_impl {
    using invokable_type = typename std::decay<Func>::type;
    using storage_type = typename signature<Func>::arguments_type;
    using return_type = typename signature<Func>::return_type;
    constexpr static const std::size_t argument_count = signature<Func>::arguments_count;

    // build scatter/gather list for tuple
    template <size_t...Indices>
    static inline std::array<iovec, argument_count> _setup_iovec(storage_type& STORAGE, std::index_sequence<Indices...>) {
        return { (iovec) { &std::get<Indices>(STORAGE), sizeof std::get<Indices>(STORAGE) }... };
    }

    static inline std::array<iovec, argument_count> _setup_iovec(storage_type& STORAGE) {
        return _setup_iovec(STORAGE, std::make_index_sequence<argument_count>());
    }

    // invoke a function with args from a tuple
    template <size_t... Indices>
    static inline return_type _invoke(invokable_type func, storage_type& STORAGE,
            std::index_sequence<Indices...>) {
        return func(std::get<Indices>(std::forward<storage_type>(STORAGE))...);
    }

    static inline return_type _invoke(invokable_type func, storage_type& STORAGE) {
        return _invoke(func, STORAGE,
                std::make_index_sequence<argument_count>{});
    }

    // basic invoker when return type is void
    template<typename R = typename signature<Func>::return_type>
    static inline typename std::enable_if<std::is_void<R>::value>::type
            invoke(Func func, int fd)
    {
        storage_type STORAGE;
        auto scatter = _setup_iovec(STORAGE);
        readv(fd, scatter.data(), scatter.size());
        _invoke(func, STORAGE);
    }

    // basic invoker when return type is non-void and trivial
    template<typename R = typename signature<Func>::return_type>
    static inline typename std::enable_if<!std::is_void<R>::value && std::is_trivial<R>::value>::type
            invoke(Func func, int fd)
    {
        storage_type STORAGE;
        auto scatter = _setup_iovec(STORAGE);
        readv(fd, scatter.data(), scatter.size());
        auto ret = _invoke(func, STORAGE);
        write(fd, &ret, sizeof ret);
    }
};

template <typename Func>
struct receiver_impl2 {
    using invokable_type = typename std::decay<Func>::type;
    using storage_type = typename signature<Func>::arguments_type2;
    using return_type = typename signature<Func>::return_type;
    constexpr static const std::size_t argument_count = signature<Func>::arguments_count;

    // invoke a function with args from a tuple
    template <size_t... Indices>
    static inline return_type _invoke(invokable_type func, storage_type& STORAGE,
            std::index_sequence<Indices...>) {
        return func(remotegl::get<Indices>(STORAGE)...);
    }

    static inline return_type _invoke(invokable_type func, storage_type& STORAGE) {
        return _invoke(func, STORAGE,
                std::make_index_sequence<argument_count>{});
    }

    // basic invoker when return type is void
    template<typename R = typename signature<Func>::return_type>
    static inline typename std::enable_if<std::is_void<R>::value>::type
            invoke(Func func, int fd)
    {
        storage_type STORAGE;
        read(fd, &STORAGE, sizeof STORAGE);
        _invoke(func, STORAGE);
    }

    // basic invoker when return type is non-void and trivial
    /*template<typename R = typename signature<Func>::return_type>
    static inline typename std::enable_if<!std::is_void<R>::value && std::is_trivial<R>::value>::type
            invoke(Func func, int fd)
    {
        storage_type STORAGE;
        auto scatter = _setup_iovec(STORAGE);
        readv(fd, scatter.data(), scatter.size());
        auto ret = _invoke(func, STORAGE);
        write(fd, &ret, sizeof ret);
    }*/
};

template<typename Func, RemoteGLFunction FuncCode, typename...SizeInfo>
struct request : signature<Func>::concrete_arguments_type2
{
    

private:


};

template <class T, typename...>
struct integer_sequence_cat;

template <class T>
struct integer_sequence_cat<T> {
    using type = std::integer_sequence<T>;
};

template <class T, T...Ints>
struct integer_sequence_cat<T, std::integer_sequence<T,Ints...>> {
    using type = std::integer_sequence<T,Ints...>;
};

template <class T, T...IntsA, T...IntsB>
struct integer_sequence_cat<T, std::integer_sequence<T,IntsA...>, std::integer_sequence<T,IntsB...>> {
    using type = std::integer_sequence<T,IntsA...,IntsB...>;
};

template <class T, T...IntsA, T...IntsB, typename...Sets>
struct integer_sequence_cat<T, std::integer_sequence<T,IntsA...>, std::integer_sequence<T,IntsB...>,Sets...> {
    using type = typename integer_sequence_cat<T,std::integer_sequence<T,IntsA...,IntsB...>,Sets...>::type;
};

template <typename...Sets>
using index_sequence_cat = integer_sequence_cat<std::size_t, Sets...>;

namespace detail {
    template <typename Tuple, typename Sequence>
    struct nopointer_sequence_impl;

    template <typename Tuple, std::size_t...Indices>
    struct nopointer_sequence_impl<Tuple,std::index_sequence<Indices...>> {
        using type = typename index_sequence_cat<
            typename std::conditional<
                std::is_arithmetic<typename remotegl::tuple_element<Indices,Tuple>::type>::value,
                std::index_sequence<Indices>,
                std::index_sequence<>
            >::type...
        >::type;
    };
}

namespace remotegl {
    template <class T>
    struct unwrap_refwrapper
    {
        using type = T;
    };
    
    template <class T>
    struct unwrap_refwrapper<std::reference_wrapper<T>>
    {
        using type = T&;
    };
    
    template <class T>
    using special_decay_t = typename unwrap_refwrapper<typename std::decay<T>::type>::type;

    template <class... Types>
    auto make_tuple(Types&&... args)
    {
        return tuple<special_decay_t<Types>...>(std::forward<Types>(args)...);
    }
}

namespace remotegl {
    namespace detail {
        template <typename Tuple, typename Sequence>
        struct multiget_impl;

        template <typename Tuple, std::size_t...Indices>
        struct multiget_impl<Tuple, std::index_sequence<Indices...>> {
            static inline auto multiget(Tuple& tuple) {
                return make_tuple(get<Indices>(tuple)...);
            }
        };
    };

    //template<typename
}

template <typename Tuple>
using nopointer_sequence = typename detail::nopointer_sequence_impl<Tuple,
        std::make_index_sequence<remotegl::tuple_size<Tuple>::value>>::type;

using A = remotegl::tuple<int,float,int*,char>;
using B = std::index_sequence<0,1,3>;

static_assert(std::is_same<nopointer_sequence<A>, B>::value, "oopsy poopsy!");

/*
- need to construct a way of yanking all of the elements that we want from an argument pack
- need a way to provide allocation type + size hint + hint to inject size information
- need a way to hint whether to require a round trip or not (or these could just be manual)

TODO:
- need tuple type generator which is aware of the signature rules
- define a get function which takes an argument pack and returns a specific element
- use an index set construct of the desired arguments to make a new pack of just what we want
- (send) call constructor with args for concrete elements to populate elements
- (send) if all concrete args, just send this
- (send) else, construct a scatter/gather list containing all of the args

*/

#endif /* REMOTEGL_PROTOCOL_HPP_ */