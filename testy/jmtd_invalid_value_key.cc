#include "../encstrset.h"

#ifdef NDEBUG
#undef NDEBUG
#endif

#include <cassert>

using namespace ::jnp1;

#define NOT !

int main() {
    const unsigned int id = encstrset_new();

    assert(NOT encstrset_remove(id, nullptr, nullptr));
    assert(NOT encstrset_test(id, nullptr, nullptr));
    assert(NOT encstrset_insert(id, nullptr, nullptr));

    assert(NOT encstrset_remove(id, nullptr, ""));
    assert(NOT encstrset_test(id, nullptr, ""));
    assert(NOT encstrset_insert(id, nullptr, ""));

    assert(NOT encstrset_remove(id, "", nullptr));
    assert(NOT encstrset_test(id, "", nullptr));
    assert(encstrset_insert(id, "", nullptr));
    assert(encstrset_test(id, "", nullptr));
    assert(encstrset_test(id, "", ""));
    assert(encstrset_test(id, "", "aaa"));

    encstrset_delete(id);
}
