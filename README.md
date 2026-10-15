# Lab 3 (Part 2): Processes and Shared Memory

A parent ("Dear Old Dad") and a child ("Poor Student") process share two integers
in System V shared memory: `BankAccount` and `Turn` (both start at 0). They take
turns using Tanenbaum's strict alternation, 25 times each:

- **Dad** (`Turn == 0`): if the account is <= $100 he picks a random amount 0–100;
  even amounts are deposited, odd means he has nothing to give. Over $100 he thinks
  the student has enough. Then `Turn = 1`.
- **Student** (`Turn == 1`): needs a random amount 0–50 and withdraws it if the
  account covers it, otherwise reports not enough cash. Then `Turn = 0`.

Each process sleeps a random 0–5 seconds before each turn.

## Build and run

```
make
./shm_proc
```
