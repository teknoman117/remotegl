#ifndef REMOTEGL_SIZE_HINTS_HPP_
#define REMOTEGL_SIZE_HINTS_HPP_

#include <utility>

// size hints system to allow information to be statically provided about how large pointer
// arguments are
struct size_hints {
    template<std::size_t N>
    struct num {
        //static constexpr bool compiletime

        template<typename... Args>
        static constexpr std::size_t value(Args... args) {
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

#endif /* REMOTEGL_SIZE_HINTS_HPP_ */
