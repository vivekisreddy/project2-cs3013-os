## How your solution avoids depriving the different sports layer types of the field (avoiding thread starving)

- Randomized execution order using shufflePlayers()
- We used usleep to acquire a lock and play, each player thread sleeps for a random amount of time
- releasing mutexes after a game completes
- Fair scheduling via thread join

## the kind of test cases would be most effective for identifying potential synchronization issues? Please share at least two cases and share the output of your program.

1. Deadlock Detection

Scenario: Multiple players attempt to lock their respective sport’s mutex simultaneously, leading to a potential deadlock.
Expected Behavior of the output: The program should ensure that players don’t indefinitely hold locks, leading to deadlocks.


2. Race Condition Detection

Scenario: Multiple threads simultaneously update shared variables (NUM_BASEBALL_PLAYERS, NUM_FOOTBALL_PLAYERS, NUM_RUGBY_PLAYERS) without proper synchronization.
Expected Behavior of the output: Ensure mutual exclusion to prevent data corruption.
