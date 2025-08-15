#ifndef HEAP_H
#define HEAP_H

#include "core.h"

namespace traf {

    template <size_t SIZE>
    struct heap_memory {
        static constexpr size_t size = SIZE;

        union word {
            word *pointer;
            intptr_t integer;
            uintptr_t address;

            explicit operator word *() const { return pointer; }
            explicit operator intptr_t() const { return integer; }
            explicit operator uintptr_t() const { return address; }
        };

        word& operator[](size_t index) { return data[index]; }
        word operator[](size_t index) const { return data[index]; }

        void free(void *pointer, size_t size) {

        };


        word free;
        word data[SIZE];
    };

}

#endif //HEAP_H
