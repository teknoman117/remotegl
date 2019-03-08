#ifndef REMOTEGL_PROTOCOL_HPP_
#define REMOTEGL_PROTOCOL_HPP_

#include <utility>
#include <tuple>

#include <sys/uio.h>

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

// fucking voodoo magic
template<typename...Ts>
using tuple_cat_t = decltype(std::tuple_cat(std::declval<Ts>()...));

template<typename...Ts>
using concrete_tuple2 = tuple_cat_t<
    typename std::conditional<
        std::is_arithmetic<Ts>::value,
        std::tuple<Ts>,
        std::tuple<>
    >::type...
>;

// helper to get useful parts of function signatures
template <typename... Func>
struct signature;

// get information from a function pointer
template <typename R, typename... Args>
struct signature<R(*)(Args...)>
{
    using return_type = R;
    using arguments_type = std::tuple<Args...>;
    using concrete_arguments_type = concrete_tuple2<Args...>;
    constexpr static const std::size_t arguments_count = sizeof...(Args);
};

// get information from a function signature
template <typename R, typename... Args>
struct signature<R(Args...)>
{
    using return_type = R;
    using arguments_type = std::tuple<Args...>;
    using concrete_arguments_type = concrete_tuple2<Args...>;
    constexpr static const std::size_t arguments_count = sizeof...(Args);
};

template <typename Func>
struct tuple_invoker
{
    using invokable_type = typename std::decay<Func>::type;
    using storage_type = typename signature<Func>::arguments_type;
    using return_type = typename signature<Func>::return_type;
    constexpr static const std::size_t arguments_count = signature<Func>::arguments_count;

    template <typename... Args>
    static inline typename std::enable_if<sizeof...(Args) == arguments_count, return_type>::type
    invoke(invokable_type func, storage_type& STORAGE, Args... args) {
        return func(std::forward<Args>(args)...);
    }

    template <typename... Args>
    static inline typename std::enable_if<sizeof...(Args) < arguments_count, return_type>::type
    invoke(invokable_type func, storage_type& STORAGE, Args... args) {
        constexpr std::size_t N = sizeof...(Args);
        using T = typename std::tuple_element<N, storage_type>::type;
        return invoke<Args...,T>(func, STORAGE, std::forward<Args>(args)..., std::get<N>(STORAGE));
    }
};

// build an iovec for receiver
template <typename StorageType, std::size_t N = 0>
inline typename std::enable_if<N == std::tuple_size<StorageType>()>::type
receiver_setup_iovec(StorageType& STORAGE, struct iovec* MSG)
{
    return;
}

template <typename StorageType, std::size_t N = 0>
inline typename std::enable_if<N < std::tuple_size<StorageType>()>::type
receiver_setup_iovec(StorageType& STORAGE, struct iovec* MSG)
{
    MSG[N] = { .iov_base = &std::get<N>(STORAGE), .iov_len = sizeof(std::get<N>(STORAGE)) };
    return receiver_setup_iovec<StorageType,N+1>(STORAGE, MSG);
}

template <typename Func>
struct receiver_impl {
    using invokable_type = typename std::decay<Func>::type;
    using storage_type = typename signature<Func>::arguments_type;
    constexpr static const std::size_t argument_count = signature<Func>::arguments_count;

    // basic invoker when return type is void
    template<typename R = typename signature<Func>::return_type>
    static inline typename std::enable_if<std::is_void<R>::value>::type
            invoke(Func func, int fd)
    {
        storage_type STORAGE;
        struct iovec MSG[argument_count];
        receiver_setup_iovec(STORAGE, MSG);
        readv(fd, MSG, argument_count);
        tuple_invoker<Func>::invoke(func, STORAGE);
    }

    // basic invoker when return type is non-void and trivial
    template<typename R = typename signature<Func>::return_type>
    static inline typename std::enable_if<!std::is_void<R>::value && std::is_trivial<R>::value>::type
            invoke(Func func, int fd)
    {
        storage_type STORAGE;
        struct iovec MSG[argument_count];
        receiver_setup_iovec(STORAGE, MSG);
        readv(fd, MSG, argument_count);
        auto ret = tuple_invoker<Func>::invoke(func, STORAGE);
        write(fd, &ret, sizeof ret);
    }
};

struct size_helpers {
    // num helper for integers
    template<int N>
    struct num {
        template<typename... Args>
        static constexpr int value(Args... args) {
            return N;
        }
    };

    template<typename T, typename R>
    struct add {
        template<typename... Args>
        static constexpr auto value(Args... args) {
            return T::value(std::forward<Args>(args)...) + R::value(std::forward<Args>(args)...);
        }
    };

    template<typename T, typename R>
    struct mul {
        template<typename... Args>
        static constexpr auto value(Args... args) {
            return T::value(std::forward<Args>(args)...) * R::value(std::forward<Args>(args)...);
        }
    };

    template<int N>
    struct arg {
        template<typename... Args>
        static constexpr auto value(Args... args) {
            return std::get<N>(std::tuple<Args...>(std::forward<Args>(args)...));
        }
    };
};

#endif /* REMOTEGL_PROTOCOL_HPP_ */