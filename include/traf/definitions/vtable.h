#ifndef VTABLE_H
#define VTABLE_H

#include "core.h"
#include "runtime_basics.h"

namespace traf {

    alignas(index)
    struct vtable {
        enum indices {
            WRITABLE_INDICES_SIZE,

            // last
            WRITABLE_INDICES_START
        };

        index *const table;

        explicit vtable(const index table) : table{static_cast<index *>(table.c_ptr)} {}
        explicit vtable(index *const table) : table{table} {}
        vtable(const vtable& table) = default;
        vtable(vtable&& table) = delete;

        [[nodiscard]] size_t start() const { return WRITABLE_INDICES_START; }
        [[nodiscard]] size_t end() const { return static_cast<size_t>(retrieve_index(WRITABLE_INDICES_SIZE)); }

        [[nodiscard]] index retrieve_index(const size_t i) const { return this->table[i]; }
        [[nodiscard]] index retrieve_index(const index i) const { return this->table[i.c_int]; }
        void set_index_at(const size_t i, const index value) { this->table[i] = value; }
        void set_index_at(const index i, const index value) { this->table[i.c_int] = value; }

        template<class T>
        T* retrieve(const size_t i) const { return static_cast<T*>(retrieve_index(i).c_ptr); }
        template<class T>
        T* retrieve(const index i) const { return static_cast<T*>(retrieve_index(i).c_ptr); }

        template<class T>
        void set_at(const size_t i, T* value) { set_index_at(i, index(value)); }
        template<class T>
        void set_at(const index i, T* value) {  set_index_at(i, index(value)); }

    };

}

#endif //VTABLE_H
