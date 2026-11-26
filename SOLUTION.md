# Dynamic bitset

`dynamic_bitset.hpp` is the complete submission, using only the five allowed
headers. Each uint64_t holds 64 consecutive bits. Unused high bits are always
zero. AND masks the final overlapping word to preserve the nonoverlapping tail.
Shifts traverse in the direction that prevents overwriting unread source words.

Indexing, individual set, and size take constant time. Push takes amortized
constant time. Bulk operations take linear time in the number of affected
64-bit words; string construction takes linear time in the string length.
Storage is proportional to the number of packed words (vector capacity is
retained after shrinking).

The live problem API accepts `cpp` only and expects a class implementation,
so submit the header directly through the client. The generic Git executable
instructions in README do not match this problem's interface. The local CMake
target `code` runs differential tests rather than inventing an input format.

Local validation:

```sh
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

Tests cover all pairs of lengths up to 260 for bitwise operations, shifts
from 0 through 260 for each length, and 400,000 random state transitions.
The supplied data directory is absent in this environment.
