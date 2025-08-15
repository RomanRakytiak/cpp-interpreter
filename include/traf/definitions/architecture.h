
#ifndef ARCHITECTURE_H
#define ARCHITECTURE_H

#include "core.h"
#include "runtime_basics.h"

namespace traf {


    template<class I>
    concept Instruction = requires(I inst, TrafThread& ctx) {
        { inst.execute(ctx) } -> std::same_as<void>;
        { inst.compile(*static_cast<void*>(nullptr), std::declval<word>()) } -> std::same_as<bool>;
        { I::name() } -> std::convertible_to<const char*>;
    };

    template<Instruction I, Instruction...Is>
    struct InstructionSet {
        using opcodes = std::make_integer_sequence<size_t, sizeof...(Is)>;

        template<size_t Op>
        using decode = std::tuple_element_t<Op, std::tuple<I, Is>>;

        template<Instruction T>
        constexpr static bool contains = std::is_same_v<T, I> || InstructionSet<Is...>::template contains<T>;

        template<Instruction T>
        using append = std::conditional_t<contains<T>, InstructionSet, InstructionSet<I, Is..., T>>;

        static void test() {
            I i;
        }
    };
}

#endif //ARCHITECTURE_H
