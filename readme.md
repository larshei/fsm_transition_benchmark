# Benchmark for mechanisms of matching state/event pairs

## Background

After contributing to [FSMlib][1], a very easy to use, lightweight, header-only
finite state machine library, and using said library in a couple of
projects, I decided to give finite state machines some more attention. Partly
as a challenge and partly because there are some things in FSMlib that I think
are nice, while others could be improved.

## What you find in this repository

Before writing a new state machine library some preparation work was done,
including everything you are finding here.

Any state machine matches states and events that are provided in e.g. a
transition table. Different strategies to find the entry for a given
state/event pair in a transition table will lead to different execution times.

Three different approaches are tested here:
- A for-loop, running through the whole transition table. For each entry, the
  table entries state is compared to the current state of the state machine. If
  these two match, we check if the table entry is meant for the current event
  and either found our match or continue the search.
- A Lookup approach, that reserves memory for states times events pointers.
  Each pointer points directly to the corresponing state/event table entry.
- An intermediate approach, where the state table entries are sorted by either
  event or state. Assuming we sorted by state, all entries for a given state
  will be placed adjacent to each other and we only need to search the block
  of entries for the current state. We can use a lookup table to jump to the
  first of the adjacent elements and then a for (or while) loop to serch
  through the block.
- Do you have more ideas? feel free to add them!

## Usage

Simply run `make` and everything will be done.

You may configure the tests in **common.h**.


[1]: https://github.com/bheemann/fsmlib
