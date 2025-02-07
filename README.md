## Decentralized Nature of the Program:

My program is a decentralized simulation of multiple sports games (Baseball, Football, and Rugby)
using multi-threading in C with the pthread library. The decentralization comes from how players (threads)
operate independently without a central controller dictating when and how games are played. Instead, the
threads coordinate using mutex locks associated with each sport, ensuring only valid games are initiated while
still maintaining concurrency. Each thread autonomously selects a sport, waits for its turn, and plays when enough players
are available.

## How the Program Works:
Player Initialization:
Players are categorized into three sports: Baseball, Football, and Rugby.
Each player is assigned an ID and a reference to the mutex associated with their sport.
The players are stored in a single array for easy shuffling and management.

## Randomized Player Shuffling:

Players are shuffled to ensure randomness in their selection for each game.
The random seed is read from a file (seed.txt) if available; otherwise, the current time is used as a seed.


## Multi-threaded Execution:

Each player runs in a separate thread (playerThread function).
Players attempt to participate in a game based on their sport's required number of participants.
The mutex for the respective sport ensures only one game per sport runs at a time.

## Game Simulation:
A game starts when enough players for a particular sport are available.
For Baseball, 18 players are required.
For Football, 22 players are required.
For Rugby, up to 30 players can participate in pairs.
Players are assigned to game positions, and a brief delay (sleep) simulates game duration.
The game ends after the simulation, and the mutex is released for another batch of players to begin.

## Interpreting the Output:

The sample output demonstrates how different games start and finish, showing which players participate and their positions:

Baseball Game Start:
[Baseball: 18] Game <<STARTED>>
[Baseball: 41] Playing at position 1
[Baseball: 120] Playing at position 2
...
[Baseball: 81] Playing at position 17

The game starts with 18 randomly selected players.

Each player's ID and assigned position are printed.

Game Duration Simulation:

Game played for X seconds

Simulates the time taken for a game before ending.

Game End Notice:

[Baseball: 18] Game <<ENDED>>

Marks the end of a game so another one can start.

Other Sports Execution:

Similar messages appear for Football and Rugby, following the same decentralized approach.

Decentralization Summary:

No central thread dictates when games happen.

Each player operates independently, waiting for their turn.

Mutex locks ensure sports are played correctly without interfering with others.

The system dynamically organizes games based on available players, making it an autonomous and scalable approach to managing multiple sports simulations.