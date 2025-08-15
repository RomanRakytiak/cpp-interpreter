#ifndef TEMPLATES_H
#define TEMPLATES_H


namespace traf {
    template<template<auto> class MAP, typename SEQ>
    struct map_integer_sequence;

    template<typename INT_TYPE, template<INT_TYPE> class MAP, INT_TYPE...Is>
    struct map_integer_sequence<MAP, std::integer_sequence<INT_TYPE, Is...>> {
        using type = std::integer_sequence<decltype(MAP<0>::value), MAP<Is>::value...>;
    };

    template<template<auto> class MAP, typename SEQ>
    using map_integer_sequence_t = typename map_integer_sequence<MAP, SEQ>::type;

    template<typename INT_TYPE, INT_TYPE FIRST>
    struct template_add {
        template<INT_TYPE SECOND>
        struct type {
            static constexpr INT_TYPE value = FIRST + SECOND;
        };
    };

    template<typename INT_TYPE, INT_TYPE START, INT_TYPE END>
    using integer_range_t = map_integer_sequence_t<template_add<INT_TYPE, START>::template type, std::make_integer_sequence<INT_TYPE, END - START>>;

    template<typename...SEQs>
    struct template_join;

    template<typename INT_TYPE, INT_TYPE...Is>
    struct template_join<
        std::integer_sequence<INT_TYPE, Is...>> {
        using type = std::integer_sequence<INT_TYPE, Is...>;
    };

    template<typename INT_TYPE, INT_TYPE...I1s, INT_TYPE...I2s,  typename...SEQs>
    struct template_join<
        std::integer_sequence<INT_TYPE, I1s...>,
        std::integer_sequence<INT_TYPE, I2s...>,
        SEQs...> {
        using type = typename template_join<std::integer_sequence<INT_TYPE, I1s..., I2s...>, SEQs...>::type;
    };

    template<typename... SEQs>
    using template_join_t = typename template_join<SEQs...>::type;

    template <typename INT_TYPE, INT_TYPE... Is, typename Lambda, typename... Args>
    void template_for_each_impl(std::integer_sequence<INT_TYPE, Is...>, Lambda&& lambda, Args&&... args) {
        (lambda.template operator()<Is>(std::forward<Args>(args)...), ...);
    }

    template <typename SEQ, typename Lambda, typename... Args>
    void template_for_each(Lambda&& lambda, Args&&... args) {
        template_for_each_impl(SEQ{}, std::forward<Lambda>(lambda), std::forward<Args>(args)...);
    }

    template <typename INT_TYPE, typename Lambda, typename... Args>
    bool template_find_impl(std::integer_sequence<INT_TYPE>, Lambda&& lambda, Args&&... args) {
        return false;
    }

    template <typename INT_TYPE, INT_TYPE I, INT_TYPE... Is, typename Lambda, typename... Args>
    bool template_find_impl(std::integer_sequence<INT_TYPE, I, Is...>, Lambda&& lambda, Args&&... args) {
        if (lambda.template operator()<I>(std::forward<Args>(args)...) == false)
            return template_find_impl<INT_TYPE>(
                std::integer_sequence<INT_TYPE, Is...>{},
                std::forward<Lambda>(lambda),
                std::forward<Args>(args)...
            );
        return true;
    }

    template <typename SEQ, typename Lambda, typename... Args>
    bool template_find(Lambda&& lambda, Args&&... args) {
        return template_find_impl(SEQ{}, std::forward<Lambda>(lambda), std::forward<Args>(args)...);
    }


    template <typename SEQ, typename T>
    bool template_find_value(const T& value) {
        return template_find<SEQ>([value]<T VALUE>(){
            return VALUE == value;
        });
    }

    template <typename INT_TYPE, INT_TYPE start, INT_TYPE end, typename Lambda, typename... Args>
    void template_for(Lambda&& lambda, Args&&... args) {
        static_assert(start <= end, "start must be <= end");
        template_for_each<integer_range_t<INT_TYPE, start, end>>(
            std::forward<Lambda>(lambda),
            std::forward<Args>(args)...
        );
    }

}

#endif //TEMPLATES_H
