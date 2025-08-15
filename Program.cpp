//
// Created by roman on 7.12.2024.
//

#include "Program.h"

#include <iostream>
#include <span>
#include <stdexcept>

#define BINARY_OPERATION(opr, call) case opr: { \
Variant second___ {pop(stack)};             \
Variant first___ {pop(stack)};              \
stack.push_back(call(first___, second___)); \
} break

using namespace project;

Variant::Variant() : data(Unit()) {}

Variant::Variant(long long value) : data(value) {}

Variant::Variant(size_t value) : data(value) {}

Variant::Variant(double value) : data(value) {}

Variant::Variant(function func) : data(std::move(func)) {}

Variant::Variant(map map) : data(std::move(map)) {}

bool Variant::is_inlined() const {
    return true;
}

bool Variant::jumps_on_jump_if_positive() const {
    return std::visit([]<class T>(const T& self) -> bool {
        if constexpr (std::is_arithmetic_v<T>) {
            return self > 0;
        }
        return false;
    }, data);
}

template<typename FUNCTOR>
static auto double_visit(FUNCTOR func, const Variant::variant &first, const Variant::variant &second) -> decltype(func(first, second)) {
    return std::visit([&](auto& a) {
        return std::visit([&](auto& b) {
            return func(a, b);
        }, second);
    }, first);
}


std::optional<Variant> Variant::overflow_add(const Variant &other) const {
    return double_visit([&]<class T1, class T2>(T1& a, T2& b) -> std::optional<Variant> {
        if constexpr (std::is_integral_v<T1> && std::is_integral_v<T2>)
            return std::make_optional(Variant::integer(a + b));
        else if constexpr (std::is_floating_point_v<T1> && std::is_integral_v<T2>
            || std::is_integral_v<T1> && std::is_floating_point_v<T2>
            || std::is_floating_point_v<T1> && std::is_floating_point_v<T2>)
            return std::make_optional(Variant::floating_point(a + b));
        return std::nullopt;
    }, data, other.data);
}

std::optional<Variant> Variant::overflow_sub(const Variant &other) const {
    return double_visit([&]<class T1, class T2>(T1& a, T2& b) -> std::optional<Variant> {
        if constexpr (std::is_integral_v<T1> && std::is_integral_v<T2>)
            return std::make_optional(Variant::integer(a - b));
        else if constexpr (std::is_floating_point_v<T1> && std::is_integral_v<T2>
            || std::is_integral_v<T1> && std::is_floating_point_v<T2>
            || std::is_floating_point_v<T1> && std::is_floating_point_v<T2>)
            return std::make_optional(Variant::floating_point(a - b));
        return std::nullopt;
    }, data, other.data);
}

std::optional<Variant> Variant::overflow_mul(const Variant &other) const {
    return double_visit([&]<class T1, class T2>(T1& a, T2& b) -> std::optional<Variant> {
        if constexpr (std::is_integral_v<T1> && std::is_integral_v<T2>)
            return std::make_optional(Variant::integer(a * b));
        else if constexpr (std::is_floating_point_v<T1> && std::is_integral_v<T2>
            || std::is_integral_v<T1> && std::is_floating_point_v<T2>
            || std::is_floating_point_v<T1> && std::is_floating_point_v<T2>)
            return std::make_optional(Variant::floating_point(a * b));
        return std::nullopt;
    }, data, other.data);
}

std::optional<Variant> Variant::overflow_div(const Variant &other) const {
    return double_visit([&]<class T1, class T2>(T1& a, T2& b) -> std::optional<Variant> {
        if constexpr (std::is_integral_v<T1> && std::is_integral_v<T2>)
            return std::make_optional(Variant::integer(a / b));
        else if constexpr (std::is_floating_point_v<T1> && std::is_integral_v<T2>
            || std::is_integral_v<T1> && std::is_floating_point_v<T2>
            || std::is_floating_point_v<T1> && std::is_floating_point_v<T2>)
            return std::make_optional(Variant::floating_point(a / b));
        return std::nullopt;
    }, data, other.data);
}

std::optional<Variant> Variant::overflow_mod(const Variant &other) const {
    return double_visit([&]<class T1, class T2>(T1& a, T2& b) -> std::optional<Variant> {
        if constexpr (std::is_integral_v<T1> && std::is_integral_v<T2>)
            return std::make_optional(Variant::integer(a % b));
        return std::nullopt;
    }, data, other.data);
}

std::optional<Variant> Variant::negate() const {
    return std::visit([]<class T>(const T& self) -> std::optional<Variant> {
        if constexpr (std::is_integral_v<T>) {
            return { Variant(-static_cast<long long>(self)) };
        }
        if constexpr (std::is_same_v<T, double>) {
            return { Variant(-self) };
        }
        return std::nullopt;
    }, data);
}

std::optional<Variant> Variant::get(const Variant &index) const {
    return std::visit([&]<class T>(const T& self) -> std::optional<Variant> {
        if constexpr (std::is_same_v<T, map>) {
            auto true_index = std::visit([&]<class U>(const U& index_self)
                -> std::optional<map_index> {
                if constexpr (std::is_integral_v<U>) {
                    return { index_self };
                }
                return std::nullopt;
            }, index.data);
            if (true_index.has_value() && self.contains(true_index.value()))
                return { *self.at(true_index.value()) };
            return std::nullopt;
        }
        return std::nullopt;
    }, data);
}

bool Variant::set(const Variant &index, Variant value) {
    return std::visit([&]<class T>(T& self) -> bool {
        if constexpr (std::is_same_v<T, map>) {
            auto true_index = std::visit([&]<class U>(const U& index_self)
                -> std::optional<map_index> {
                if constexpr (std::is_integral_v<U>) {
                    return { index_self };
                }
                return std::nullopt;
            }, index.data);
            if (true_index.has_value()) {
                self[true_index.value()] = std::make_shared<Variant>(std::move(value));
                return true;
            }
            return false;
        }
        return false;
    }, data);
}

Variant Variant::or_else(const Variant &other) const {
    return std::visit([&]<class T>(const T&) {
        if constexpr (std::is_same_v<T, Unit>)
            return other;
        return *this;
    }, data);
}

Variant Variant::and_else(const Variant &other) const {
    return std::visit([&]<class T>(const T&) {
        if constexpr (std::is_same_v<T, Unit>)
            return *this;
        return other;
    }, data);
}

std::optional<size_t> Variant::try_to_index() const {
    return std::visit([]<class T>(const T& self) -> std::optional<size_t> {
        if constexpr (std::is_integral_v<T> || std::is_floating_point_v<T>)
            return { static_cast<size_t>(self) };
        return std::nullopt;
   }, data);
}

void Variant::unchecked_delete() {
    std::visit([](auto&& self) {
        /* delete pointer */
    }, data);
    data = Unit();
}

Variant Variant::call(const std::span<Variant>& arguments) {
    return std::visit<std::optional<Variant>>([&]<class T>(T& self) -> std::optional<Variant> {
        if constexpr (std::is_same_v<T, function>) {
            auto result = self(arguments[0]);
            if (arguments.size() == 1)
                return result;
            return result.call(arguments.subspan(1));
        }
        return std::nullopt;
    }, data).value();
}

bool Variant::operator==(const Variant &other) const {
    return double_visit([]<class T1, class T2>(const T1& first, const T2& second) -> bool {
        if constexpr (std::is_same_v<T1, function> && std::is_same_v<T2, function>)
            return &first == &second;
        else if constexpr ((std::is_integral_v<T1> || std::is_floating_point_v<T1>)
            && (std::is_integral_v<T2> || std::is_floating_point_v<T2>)
            || std::is_same_v<T1, T2>)
            return first == second;
        return false;
    }, data, other.data);
}


static Variant pop(std::vector<Variant>& stack) {
    const auto last = stack.back();
    stack.pop_back();
    return last;
}

size_t Program::Instruction::stack_arguments() const {
    switch (type) {
        case PUSH_CONST:
        case PUSH_STACK:
        case POP:
        case SWAP:
        case PUSH_IMMEDIATE:
        case DELETE:
            return 0;
        case PUSH_GLOBAL:
        case JUMP_IF_POSITIVE:
            return 1;
        case OVERFLOW_ADD:
        case OVERFLOW_SUB:
        case OVERFLOW_MUL:
        case OVERFLOW_DIV:
        case OVERFLOW_MOD:
        case OR_ELSE:
        case AND_ELSE:
        case EQUAL:
        case GET:
            return 2;
        case SET:
            return 3;
        case CALL:
            return argument + 1;
        default:
            throw std::logic_error("Unhandled type");
    }
}

int Program::Instruction::stack_increment() const {
    switch (type) {
        case POP:
            return -static_cast<int>(argument);
        case SWAP:
        case DELETE:
        case JUMP_IF_POSITIVE:
            return 0;
        default:
            return 1;
    }
}

void Program::execute(std::vector<Variant>& stack) const {
    size_t instruction_index = 0;
    while (instruction_index < instructions.size()) {
        std::cerr << '\n';
        instruction_index = execute_instruction(stack, instruction_index);
    }
}

std::vector<Variant> Program::execute() const {
    std::vector<Variant> stack;
    execute(stack);
    return stack;
}

size_t Program::execute_instruction(std::vector<Variant> &stack, const size_t instruction) const {
    switch (auto [type, argument] = instructions.at(instruction); type) {
        BINARY_OPERATION(OVERFLOW_ADD, [](const auto &a, const auto &b) { return a.overflow_add(b).value(); });
        BINARY_OPERATION(OVERFLOW_SUB,  [](const auto &a, const auto &b) { return a.overflow_sub(b).value(); });
        BINARY_OPERATION(OVERFLOW_MUL, [](const auto &a, const auto &b) { return a.overflow_mul(b).value(); });
        BINARY_OPERATION(OVERFLOW_DIV, [](const auto &a, const auto &b) { return a.overflow_div(b).value(); });
        BINARY_OPERATION(OVERFLOW_MOD, [](const auto &a, const auto &b) { return a.overflow_mod(b).value(); });
        BINARY_OPERATION(OR_ELSE, [](const auto &a, const auto &b) { return a.or_else(b); });
        BINARY_OPERATION(AND_ELSE, [](const auto &a, const auto &b) { return a.and_else(b); });
        case POP: {
            const auto amount = argument;
            stack.erase(stack.end() - amount, stack.end());
        } break;
        case SWAP: {
            const size_t index = argument;
            stack.at(stack.size() - 1).swap(stack.at(stack.size() - 1 - index));
        } break;
        case PUSH_CONST: {
            const size_t index = argument;
            if (index >= constants.size())
                throw std::out_of_range("Program::push_const: index out of range");
            stack.push_back(constants.at(index));
        } break;
        case PUSH_STACK: {
            const size_t index = argument;
            if (index >= stack.size())
                throw std::out_of_range("Program::push_global: index out of range");
            stack.push_back(stack.at(stack.size() - index - 1));
        } break;
        case PUSH_GLOBAL: {
            const size_t index = pop(stack).try_to_index().value();
            if (index >= stack.size())
                throw std::out_of_range("Program::push_global: index out of range");
            stack.push_back(stack.at(stack.size() - index - 1));
        } break;
        case DELETE: {
            const size_t index = argument;
            stack.at(stack.size() - index - 1).unchecked_delete();
        } break;
        case PUSH_IMMEDIATE: {
            stack.push_back(Variant::integer(argument));
        } break;
        case CALL: {
            Variant callable {pop(stack)};
            auto argument_count = argument;
            if (argument_count >= constants.size())
                throw std::out_of_range("Program::curry: argument count out of range");
            auto result = callable.call({
                stack.end() - argument_count,
                argument_count
            });
            stack.erase(stack.end() - argument_count, stack.end());
            stack.push_back(result);
        } break;
        case JUMP_IF_POSITIVE: {
            const size_t jump = argument;
            if (Variant condition {pop(stack)}; condition.jumps_on_jump_if_positive())
                return jump;
        } break;
        case SET: {
            const Variant value = pop(stack);
            const Variant index = pop(stack);
            if (stack.empty())
                throw std::out_of_range("Program::set: stack is empty");
            if (stack.back().set(index, value) == false)
                throw ProjectError("cannot set index to value");
        } break;
        case GET: {
            const Variant index = pop(stack);
            const Variant value = pop(stack);
            const auto result = value.get(index);
            if (result.has_value() == false)
                throw ProjectError("cannot get index");
            stack.push_back(result.value());
        } break;
        default:
            throw std::runtime_error("Program::execute(): Unknown instruction");
    }
    return instruction + 1;
}


std::ostream & project::operator<<(std::ostream &stream, const Variant &variant) {
    std::visit([&]<class T>(const T &self) {
        if constexpr (std::is_same_v<T, Variant::map>) {
            stream << '{';
            bool first = true;
            for (const auto &pair : self) {
                if (first) {
                    stream << pair.first << ": " << pair.second;
                    first = false;
                }
                else
                    stream << ", " << pair.first << ": " << pair.second;
            }
            stream << '}';
        } else if constexpr (std::is_same_v<T, Variant::function>) {
            stream << "<function>";
        } else {
            stream << self;
        }
    }, variant.data);
    return stream;
}
