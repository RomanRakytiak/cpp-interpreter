#ifndef VM_H
#define VM_H


#include "runtime_basics.h"

namespace traf {

    using instructions = std::integer_sequence<word>;

    class TrafVM {

    public:
        bool release_thread(TrafThread &thread);
    };

}

#endif //VM_H
