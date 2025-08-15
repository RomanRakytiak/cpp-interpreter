#pragma once

#include "Program.h"

namespace project {

    struct ConstantAddress {
        const Program::word offset;

        explicit ConstantAddress(Program::word offset);
        bool operator==(const ConstantAddress & stack) const = default;
        friend std::ostream& operator<<(std::ostream& os, const ConstantAddress& ref) { return os << ref.offset; }
    };

    struct StackAddress {
        const size_t offset;

        explicit StackAddress(size_t offset);
        bool operator==(const StackAddress & stack) const = default;
        friend std::ostream& operator<<(std::ostream& os, const StackAddress& ref) { return os << ref.offset; }

    };

    struct InstructionAddress {
        const Program::word offset;

        explicit InstructionAddress(Program::word offset);
        bool operator==(const InstructionAddress & stack) const = default;
        friend std::ostream& operator<<(std::ostream& os, const InstructionAddress& ref) { return os << ref.offset; }
    };

    struct JumpAddress  {
        InstructionAddress address;

        explicit JumpAddress(const InstructionAddress offset) : address(offset) {}
        explicit JumpAddress(const Program::word offset) : address(offset) {}
    };

    struct BytecodeBuilder;

    struct StackFrame {
        BytecodeBuilder& builder;
        size_t old_stack_pointer;
        bool pop_variables = true;
        bool is_virtual = false;

        StackFrame() = delete;
        StackFrame(BytecodeBuilder& builder, const size_t old_stack_pointer)
            : builder(builder), old_stack_pointer(old_stack_pointer) {}
        StackFrame(const StackFrame&) = delete;
        StackFrame(StackFrame&& other) noexcept
            : builder(other.builder), old_stack_pointer(other.old_stack_pointer),
            pop_variables(other.pop_variables), is_virtual(other.is_virtual)
        { other.is_virtual = true; }
        ~StackFrame() { end_frame(); }

        StackFrame& operator=(const StackFrame&) = delete;
        StackFrame& operator=(StackFrame&& other) = delete;

        void end_frame();
        [[nodiscard]] StackAddress frame_start() const { return StackAddress(old_stack_pointer); }
    };

    struct BytecodeBuilder {
        using Instruction = Program::InstructionType;

        [[nodiscard]] ConstantAddress next_constant_address() const;
        [[nodiscard]] ConstantAddress last_constant_address() const;
        [[nodiscard]] InstructionAddress next_instruction_address() const;
        [[nodiscard]] InstructionAddress last_instruction_address() const;
        ConstantAddress new_constant(const Variant& variant);
        [[nodiscard]] StackAddress stack_top() const;
        [[nodiscard]] size_t real_address(StackAddress ref) const;

        template <std::integral T>
        StackAddress push(T value);
        StackAddress push(const double value) { return push(Variant(value)); }
        StackAddress push(ConstantAddress value);
        StackAddress push(InstructionAddress value);
        StackAddress push(StackAddress value);
        StackAddress push(const Variant& value) { return push(new_constant(value)); }
        StackAddress push_unit();
        StackAddress virtual_push();

        template <class T>
        JumpAddress jump_if_positive(T&& value);
        JumpAddress jump_if_stack_top_positive();
        JumpAddress unconditional_jump();
        void update_jump_location(JumpAddress value, InstructionAddress location);

        template <class T1, class T2>
        StackAddress get(T1 &&value, T2 &&index);
        template <class T1, class T2, class T3>
        StackAddress set(T1 &&object, T2 &&index, T3 &&value);
        template <class T1, class T2>
        void stack_top_set(T1 &&index, T2 &&value);

        template <class T1, class T2>
        StackAddress equal(T1 &&first, T2 &&second);

        StackFrame new_stack_frame() { return {*this, stack_pointer}; }

        void pop(size_t amount = 1);
        void pop_until(size_t old_stack_pointer);
        void return_until(const size_t old_stack_pointer) { stack_pointer = old_stack_pointer; };
        [[nodiscard]] size_t current_stack_pointer() const { return stack_pointer; };
        void swap_top_with(uint16_t index = 1);
        void swap_top_with(StackAddress value);
        void swap(StackAddress first, StackAddress second);
        void assign_from_top(StackAddress value);
        template<typename T>
        void assign(StackAddress destination, T&& value);
        void try_delete(StackAddress value);

        template <typename T1, typename T2>
        StackAddress add(T1 &&first, T2 &&second);
        template <typename T1, typename T2>
        StackAddress sub(T1 &&first, T2 &&second);
        template <typename T1, typename T2>
        StackAddress mul(T1 &&first, T2 &&second);
        template <typename T1, typename T2>
        StackAddress div(T1 &&first, T2 &&second);
        template <typename T1, typename T2>
        StackAddress mod(T1 &&first, T2 &&second);

        template <typename REF, typename...TYPES>
        StackAddress call(REF&& callable, TYPES&&...arguments);

        void command(const Instruction instruction) { command({instruction, 0}); }
        void command(Program::Instruction instruction);

        Program build() { return {std::move(instructions), std::move(constants)}; }

        [[nodiscard]] Program::Instruction instruction_at(const size_t index) const { return instructions.at(index); }
        [[nodiscard]] const Variant& constant_at(const size_t index) const { return constants.at(index); }

        static std::optional<Instruction> evaluate_operator(char c);

    private:
        std::vector<Program::Instruction> instructions;
        std::vector<Variant> constants;
        size_t stack_pointer = 0;
    };

}

template<std::integral T>
project::StackAddress project::BytecodeBuilder::push(T value) {
    if (value < 0 && value >= Program::max_word_limit)
        return push(Variant::integer(value));
    command({
        Program::PUSH_IMMEDIATE,
        static_cast<Program::word>(value)
    });
    return stack_top();
}

template<class T>
project::JumpAddress project::BytecodeBuilder::jump_if_positive(T &&value) {
    push(std::forward<T>(value));
    return jump_if_stack_top_positive();
}

template<class T1, class T2>
project::StackAddress project::BytecodeBuilder::get(T1 &&value, T2 &&index) {
    push(std::forward<T1>(value));
    push(std::forward<T2>(index));
    command(Program::GET);
    return stack_top();
}

template<class T1, class T2, class T3>
project::StackAddress project::BytecodeBuilder::set(T1 &&object, T2 &&index, T3 &&value) {
    push(std::forward<T1>(object));
    stack_top_set(std::forward<T2>(index), std::forward<T3>(value));
    return stack_top();
}

template<class T1, class T2>
void project::BytecodeBuilder::stack_top_set(T1 &&index, T2 &&value) {
    push(std::forward<T1>(index));
    push(std::forward<T2>(value));
    command(Program::SET);
}

template<class T1, class T2>
project::StackAddress project::BytecodeBuilder::equal(T1 &&first, T2 &&second) {
    push(std::forward<T1>(first));
    push(std::forward<T2>(second));
    command(Program::EQUAL);
    return stack_top();
}

template<typename T>
void project::BytecodeBuilder::assign(const StackAddress destination, T &&value) {
    if constexpr (std::is_same_v<T, StackAddress>) {
        if (destination == value)
            return;
    }
    push(std::forward<T>(value));
    assign_from_top(destination);
}

template<typename T1, typename T2>
project::StackAddress project::BytecodeBuilder::add(T1 &&first, T2 &&second) {
    push(std::forward<T1>(first));
    push(std::forward<T2>(second));
    command(Program::OVERFLOW_ADD);
    return stack_top();
}

template<typename T1, typename T2>
project::StackAddress project::BytecodeBuilder::sub(T1 &&first, T2 &&second) {
    push(std::forward<T1>(first));
    push(std::forward<T2>(second));
    command(Program::OVERFLOW_SUB);
    return stack_top();
}

template<typename T1, typename T2>
project::StackAddress project::BytecodeBuilder::mul(T1 &&first, T2 &&second) {
    push(std::forward<T1>(first));
    push(std::forward<T2>(second));
    command(Program::OVERFLOW_MUL);
    return stack_top();
}

template<typename T1, typename T2>
project::StackAddress project::BytecodeBuilder::div(T1 &&first, T2 &&second) {
    push(std::forward<T1>(first));
    push(std::forward<T2>(second));
    command(Program::OVERFLOW_DIV);
    return stack_top();
}

template<typename T1, typename T2>
project::StackAddress project::BytecodeBuilder::mod(T1 &&first, T2 &&second) {
    push(std::forward<T1>(first));
    push(std::forward<T2>(second));
    command(Program::OVERFLOW_MOD);
    return stack_top();
}

template<typename REF, typename...TYPES>
project::StackAddress project::BytecodeBuilder::call(
    REF&& callable,
    TYPES&&...arguments
) {
    static_assert(sizeof...(arguments) >= 1, "Too few arguments");
    static_assert(sizeof...(arguments) < Program::max_word_limit, "Too many arguments");
    (push(std::forward<TYPES>(arguments)), ...);
    push(std::forward<REF>(callable));
    command({Program::CALL, sizeof...(arguments)});
    return stack_top();
}



