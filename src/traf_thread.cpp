#include "traf_thread.h"
#include "traf_vm.h"

using namespace traf;

TrafThread::TrafThread()
    : uid(0), vm(nullptr), stack(nullptr), stack_size(0), program(nullptr), program_size(0) {
}

TrafThread::TrafThread(
    const size_t uid,
    TrafVM &vm,
    word *stack, const size_t stack_size,
    const instruction *program, const size_t program_size
)
    : uid(uid), vm(&vm), stack(stack), stack_size(stack_size), program(program), program_size(program_size) {}

TrafThread::TrafThread(TrafThread &&other) noexcept
    : uid(other.uid), vm(other.vm)
    , stack(other.stack), stack_size(other.stack_size)
    , program(other.program), program_size(other.program_size), program_counter(other.program_counter) {
    clean();
}

TrafThread & TrafThread::operator=(TrafThread &&other) noexcept {
    uid = other.uid;
    vm = other.vm;
    stack = other.stack;
    stack_size = other.stack_size;
    program = other.program;
    program_size = other.program_size;
    program_counter = other.program_counter;
    other.clean();
    return *this;
}

bool TrafThread::set_program_counter(const intptr_t program_counter) {
    if (program_counter >= program_size || program_counter < 0)
        return false;
    this->program_counter = program_counter;
    return true;
}

bool TrafThread::set_stack_pointer(const intptr_t stack_pointer) {
    if (stack_pointer >= stack_size || stack_pointer < 0)
        return false;
    this->stack_pointer = stack_pointer;
    return true;
}


index TrafThread::stack_top() const {
    return stack[stack_pointer];
}

intptr_t TrafThread::get_stack_pointer() const {
    return stack_pointer;
}

index * TrafThread::get_index(const intptr_t offset) const {
    return stack + offset;
}

bool TrafThread::run(const instruction code) {
    return template_find<instructions>([this, code]<word INSTR>() {
        if (code.type == INSTR) {
            instruction_traits<INSTR>::run(*this, code);
            return true;
        }
        return false;
    });
}

bool TrafThread::run_one_instruction() {
    return run(program[program_counter]);
}

bool TrafThread::has_next_instruction() {
    return is_active() && program_counter < program_size;
}

bool TrafThread::halt_program() {
    program_counter = program_size;
}

bool TrafThread::release() {

}

bool TrafThread::is_active() const {
    return vm != nullptr;
}

void TrafThread::clean() {
    stack = nullptr;
    stack_size = 0;
    vm = nullptr;
    program = nullptr;
    uid = 0;
    program_size = 0;
    program_counter = 0;
}
