#pragma once

#include <atomic>

#include "core.h"

namespace traf {

    using word = std::conditional_t<(sizeof(void*) > sizeof(std::uint32_t)), std::uint32_t, std::uint16_t>;

    alignas(void *)
    struct instruction {
        word type;
        word argument;
    };


    template <word INSTR>
    struct inspect_code;

    template <word INSTR>
    struct instruction_traits {
        using inspector = inspect_code<INSTR>;

        constexpr static size_t stack_arguments = inspector::stack_arguments;
        constexpr static size_t stack_return = inspector::stack_return;
        constexpr static bool has_inline_argument = inspector::has_inline_argument;

        static void run(TrafThread &thread, instruction inst) { inspector::run(thread, inst); }
    };


    alignas(void *)
    struct index {
        static_assert(sizeof(float) <= sizeof(void*), "pointer size must be larger than or equal to float");

        alignas(void *) union {
            void *c_ptr;
            intptr_t c_int;
            uintptr_t c_uint;
            float c_float;
            instruction instr;
        };

        index() = default;
        index(const index &) = default;
        index(index &&) = default;
        explicit index(void *ptr) : c_ptr(ptr) {}
        explicit index(const uint32_t value) : c_uint(static_cast<uintptr_t>(value)) {}
        explicit index(const int32_t value) : c_int(static_cast<intptr_t>(value)) {}
        explicit index(const uint64_t value) : c_uint(static_cast<uintptr_t>(value)) {
            if (value != c_uint)
                throw std::overflow_error("unsigned integer overflow");
        }
        explicit index(const int64_t value) : c_int(static_cast<intptr_t>(value)) {
            if (value != c_int)
                throw std::overflow_error("signed integer overflow");
        }
        explicit index(const float value) : c_ptr(nullptr) {
            c_float = value;
        }


        index &operator=(const index &) = default;
        index &operator=(index &&) = default;
        bool operator==(const index &other) const { return c_uint == other.c_uint; }
        bool operator==(const void* ptr) const { return c_ptr == ptr; }
        bool operator==(const float value) const { return c_float == value; }


        explicit operator int64_t() const { return static_cast<int64_t>(c_int); }
        explicit operator uint64_t() const { return static_cast<uint64_t>(c_uint); }
        explicit operator int32_t() const {
            if (c_int < std::numeric_limits<int32_t>::min()
                ||  std::numeric_limits<int32_t>::max() < c_int)
                throw std::overflow_error("signed integer overflow");
            return static_cast<int32_t>(c_int);
        }
        explicit operator uint32_t() const {
            if (c_uint < std::numeric_limits<uint32_t>::min()
                ||  std::numeric_limits<uint32_t>::max() < c_uint)
                throw std::overflow_error("unsigned integer overflow");
            return static_cast<uint32_t>(c_uint);
        }
        explicit operator double() const { return static_cast<double>(c_float); }
        explicit operator float() const { return static_cast<float>(c_float); }
        explicit operator void *() const { return c_ptr; }

    };



}

