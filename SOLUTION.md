# Dynamic bitset

`dynamic_bitset.hpp` is the complete submission, using only the five allowed
headers. Each uint64_t holds 64 consecutive bits. Unused high bits are always
zero. AND masks the final overlapping word to preserve the nonoverlapping tail.
Shifts traverse in the direction that prevents overwriting unread source words.

Indexing and size take constant time. Individual set takes constant time once
storage exists; the first write of one to an implicit zero bitset allocates
and initializes its packed words. Push takes amortized constant time after
this allocation. Bulk operations take linear time in the number of affected
64-bit words, plus any initial allocation; string construction takes linear
time in the string length. Size-only construction takes constant time.
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

## Submission history

- 879456: Compile Error, 0/100. The judge requires `sjtu::dynamic_bitset`,
  although the published interface omits the namespace. Added the namespace
  and retained a global alias for compatibility with the stated interface.
- 879458: 90/100. All groups passed except the constructor group's 32 MiB
  memory limit. Added noexcept move operations to transfer storage without
  deep copies, including during vector reallocation.
- 879459: 90/100, with unchanged constructor memory usage. Added an implicit
  all-zero representation: size-only construction and copies need no packed
  allocation until a bit becomes one. Packed operations support both forms.
- 879462: Accepted, 100/100, all ten test points passed. Constructor test
  memory fell to 3,956,736 bytes. Four of five submission attempts used.
  Final implementation passed differential tests with AddressSanitizer and
  UndefinedBehaviorSanitizer enabled.
