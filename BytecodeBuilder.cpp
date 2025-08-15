//
// Created by roman on 7.12.2024.
//

#include "BytecodeBuilder.h"

#include <stdexcept>

using namespace project;

ConstantAddress::ConstantAddress(const Program::word offset)
    : offset(offset)
{
    if (offset >= Program::max_constants_size)
        throw std::invalid_argument("offset is greater than Program::max_constants_size");
}

StackAddress::StackAddress(const size_t offset)
    : offset(offset)
{}

InstructionAddress::InstructionAddress(const Program::word offset)
    : offset(offset)
{
    if (offset >= Program::max_instructions_size)
        throw std::invalid_argument("offset is greater than Program::max_instructions_size");
}

void StackFrame::end_frame() {
    if (is_virtual)
        return;
    if (pop_variables)
        builder.pop_until(old_stack_pointer);
    else
        builder.return_until(old_stack_pointer);
    is_virtual = true;
}


ConstantAddress BytecodeBuilder::next_constant_address() const {
    return ConstantAddress(static_cast<Program::word>(constants.size()));
}

ConstantAddress BytecodeBuilder::last_constant_address() const {
    return ConstantAddress(static_cast<Program::word>(constants.size() - 1));
}

InstructionAddress BytecodeBuilder::next_instruction_address() const {
    return InstructionAddress(static_cast<Program::word>(instructions.size()));
}

InstructionAddress BytecodeBuilder::last_instruction_address() const {
    return InstructionAddress(static_cast<Program::word>(instructions.size() - 1));
}

ConstantAddress BytecodeBuilder::new_constant(const Variant &variant) {
    constants.push_back(variant);
    return last_constant_address();
}

StackAddress BytecodeBuilder::stack_top() const {
    assert(stack_pointer > 0);
    return StackAddress(stack_pointer - 1);
}

size_t BytecodeBuilder::real_address(const StackAddress ref) const {
    assert(ref.offset < stack_pointer);
    return stack_pointer - 1 - ref.offset;
}

StackAddress BytecodeBuilder::push(const InstructionAddress value) {
    return push(value.offset);
}

StackAddress BytecodeBuilder::push(const StackAddress value) {
    const size_t address = real_address(value);
    if (address < Program::max_word_limit) {
        command({
            Program::PUSH_STACK,
            static_cast<Program::word>(address)
        });
        return stack_top();
    }
    push(address);
    command(Program::PUSH_GLOBAL);
    return stack_top();
}

StackAddress BytecodeBuilder::push(const ConstantAddress value) {
    command({Program::PUSH_CONST, value.offset});
    return stack_top();
}

StackAddress BytecodeBuilder::push_unit() {
    return push(Variant());
}

StackAddress BytecodeBuilder::virtual_push() {
    stack_pointer++;
    return stack_top();
}

JumpAddress BytecodeBuilder::jump_if_stack_top_positive() {
    command(Program::JUMP_IF_POSITIVE);
    return JumpAddress(last_instruction_address());
}

JumpAddress BytecodeBuilder::unconditional_jump() {
    return jump_if_positive(1);
}

void BytecodeBuilder::update_jump_location(const JumpAddress value, const InstructionAddress location) {
    instructions[value.address.offset].argument = location.offset;
}

void BytecodeBuilder::pop(const size_t amount) {
    assert(stack_pointer >= amount);
    size_t remaining = amount;
    while (remaining > Program::max_word_limit) {
        remaining -= Program::max_word_limit;
        command({Program::POP, Program::max_word_limit});
    }
    command({Program::POP, static_cast<Program::word>(remaining)});
}

void BytecodeBuilder::pop_until(const size_t old_stack_pointer) {
    assert(old_stack_pointer <= stack_pointer);
    if (old_stack_pointer == stack_pointer)
        return;
    pop(stack_pointer - old_stack_pointer);
}


void BytecodeBuilder::swap_top_with(const uint16_t index) {
    command({Program::SWAP, index});
}

void BytecodeBuilder::swap_top_with(const StackAddress value) {
    const size_t address = real_address(value);
    assert(address < Program::max_word_limit);
    command({Program::SWAP, static_cast<Program::word>(address)});
}

void BytecodeBuilder::assign_from_top(const StackAddress value) {
    if (value == stack_top())
        return;
    swap_top_with(value);
    pop();
}

void BytecodeBuilder::swap(const StackAddress first, const StackAddress second) {
    swap_top_with(first);
    swap_top_with(second);
    swap_top_with(first);
}

void BytecodeBuilder::try_delete(const StackAddress value) {
    const auto address = real_address(value);
    assert(address < Program::max_word_limit);
    command({Program::DELETE, static_cast<Program::word>(address)});
}

void BytecodeBuilder::command(const Program::Instruction instruction) {
    instructions.push_back(instruction);
    stack_pointer -= instruction.stack_arguments();
    stack_pointer += instruction.stack_increment();
}

std::optional<BytecodeBuilder::Instruction> BytecodeBuilder::evaluate_operator(const char c) {
    switch (c) {
        case '+':
            return Program::OVERFLOW_ADD;
        case '-':
            return Program::OVERFLOW_SUB;
        case '*':
            return Program::OVERFLOW_MUL;
        case '/':
            return Program::OVERFLOW_DIV;
        case '%':
            return Program::OVERFLOW_MOD;
        case '|':
            return Program::OR_ELSE;
        case '&':
            return Program::AND_ELSE;
        default:
            return std::nullopt;
    }
}



