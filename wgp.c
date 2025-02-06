#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>  

#define NUM_BASEBALL_PLAYERS 36
#define NUM_FOOTBALL_PLAYERS 44
#define NUM_RUGBY_PLAYERS 60
#define TOTAL_PLAYERS (NUM_BASEBALL_PLAYERS + NUM_FOOTBALL_PLAYERS + NUM_RUGBY_PLAYERS)

// Player types
typedef enum { BASEBALL, FOOTBALL, RUGBY } SportType;

// Player structure
typedef struct {
    int id;
    SportType sport;
} Player;

// Arrays to hold all players
Player allPlayers[TOTAL_PLAYERS];

// Mutexes for each sport
pthread_mutex_t baseballMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t footballMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t rugbyMutex = PTHREAD_MUTEX_INITIALIZER;

// Mutex to enforce fairness (round-robin scheduling)
pthread_mutex_t fairnessMutex = PTHREAD_MUTEX_INITIALIZER;

// Global variable to track the turn of the next sport
int nextSportToPlay = BASEBALL;

// Function to initialize the players for each sport
void initializePlayers() {
    int playerIndex = 0;
    
    // Initialize baseball players
    for (int i = 0; i < NUM_BASEBALL_PLAYERS; i++) {
        allPlayers[playerIndex].id = 0; // Placeholder for now, will shuffle later
        allPlayers[playerIndex].sport = BASEBALL;
        playerIndex++;
    }

    // Initialize football players
    for (int i = 0; i < NUM_FOOTBALL_PLAYERS; i++) {
        allPlayers[playerIndex].id = 0; // Placeholder for now, will shuffle later
        allPlayers[playerIndex].sport = FOOTBALL;
        playerIndex++;
    }

    // Initialize rugby players
    for (int i = 0; i < NUM_RUGBY_PLAYERS; i++) {
        allPlayers[playerIndex].id = 0; // Placeholder for now, will shuffle later
        allPlayers[playerIndex].sport = RUGBY;
        playerIndex++;
    }
}

// Function to shuffle an array of players randomly
void shufflePlayers(Player players[], int numPlayers) {
    srand(time(NULL));
    for (int i = 0; i < numPlayers; i++) {
        int j = rand() % numPlayers;
        Player temp = players[i];
        players[i] = players[j];
        players[j] = temp;
    }
}

// Function to read the seed from a file
int readSeedFromFile(const char* filename) {
    FILE* file = fopen(filename, "r");
    int seed;
    if (file != NULL) {
        fscanf(file, "%d", &seed);
        fclose(file);
    } else {
        // If the file can't be opened, use a default seed value
        seed = time(NULL);
    }
    return seed;
}

// Function to simulate game time
void simulateGameTime() {
    // Random time between 1 and 5 seconds
    int gameTime = rand() % 3 + 1;
    sleep(gameTime);  // Simulate the game duration
    printf("Game played for %d seconds\n", gameTime);
}

// Function to simulate game time for rugby players
void simulateRugbyGameTime() {
    // Random time between 1 and 3 seconds for each pair
    int gameTime = rand() % 3 + 1;
    sleep(gameTime);  // Simulate the game duration
    printf("Pair played for %d seconds\n", gameTime);
}

// Function to play a game for rugby with pairs
void playRugbyGame(Player* players, int numPlayersRequired) {
    // Number of pairs
    int pairCount = numPlayersRequired / 2;

    // Loop through each pair
    for (int i = 0; i < pairCount; i++) {
        int player1 = i * 2;
        int player2 = i * 2 + 1;

        // Print when a pair is ready
        printf("[Rugby: %d] Pair ready\n", players[player1].id);

        // Print when players are playing at a position
        printf("[Rugby: %d] Playing at position %d\n", players[player1].id, (i * 2) + 1);
        printf("[Rugby: %d] Playing at position %d\n", players[player2].id, (i * 2) + 2);

        // Simulate the game time for the pair
        simulateRugbyGameTime();
    }
}


// Function to play a game for baseball, football, or rugby
void playGame(SportType sport, int numPlayersRequired) {
    Player *players;
    int totalPlayers;

    // Select appropriate sport and number of players
    switch (sport) {
        case BASEBALL:
            players = allPlayers;
            totalPlayers = NUM_BASEBALL_PLAYERS;
            break;
        case FOOTBALL:
            players = allPlayers + NUM_BASEBALL_PLAYERS; // Start from football players
            totalPlayers = NUM_FOOTBALL_PLAYERS;
            break;
        case RUGBY:
            players = allPlayers + NUM_BASEBALL_PLAYERS + NUM_FOOTBALL_PLAYERS; // Start from rugby players
            totalPlayers = NUM_RUGBY_PLAYERS;
            break;
    }

    // Shuffle players randomly
    shufflePlayers(players, totalPlayers);

    // Start the game for the sport
    printf("[%s: %d] Game <<STARTED>>\n", 
           (sport == BASEBALL) ? "Baseball" : (sport == FOOTBALL) ? "Football" : "Rugby", numPlayersRequired);

    if (sport == RUGBY) {
        // Rugby players must be paired and played sequentially
        int rugbyPlayersToPlay = (NUM_RUGBY_PLAYERS > 30) ? 30 : NUM_RUGBY_PLAYERS;
        if (rugbyPlayersToPlay % 2 != 0) rugbyPlayersToPlay--; // Ensure it's even
        if (rugbyPlayersToPlay >= 2) {
            playRugbyGame(players, rugbyPlayersToPlay); // Rugby needs at least 2 players, max 30
        }
    } else {
        // For Baseball and Football, print players as usual
        for (int i = 0; i < numPlayersRequired; i++) {
            printf("[%s: %d] Playing at position %d\n", 
                   (sport == BASEBALL) ? "Baseball" : (sport == FOOTBALL) ? "Football" : "Rugby", 
                   players[i].id, 
                   i + 1);
        }

        // Simulate game time for baseball and football
        simulateGameTime();
    }

    printf("[%s: %d] Game <<ENDED>>\n", 
           (sport == BASEBALL) ? "Baseball" : (sport == FOOTBALL) ? "Football" : "Rugby", numPlayersRequired);
}

// Function to handle a player choosing a sport and playing
void* playerThread(void* arg) {
    Player* player = (Player*)arg;

    // Read seed from file and seed the random number generator
    int seed = readSeedFromFile("seed.txt");
    srand(seed);

    // Loop to keep playing games indefinitely
    while (1) {
        pthread_mutex_lock(&fairnessMutex); // Lock fairness mutex to enforce turn-based play

        // Wait for the player's sport to be the one that gets to play
        while (player->sport != nextSportToPlay) {
            pthread_mutex_unlock(&fairnessMutex); // Release the fairness mutex so other threads can run
            sched_yield(); // Yield to allow other threads to progress
            pthread_mutex_lock(&fairnessMutex); // Re-acquire fairness lock
        }

        // Introduce a random delay to simulate randomness in scheduling
        int delayTime = rand() % 1000000; // Random delay time in microseconds
        usleep(delayTime); // Delay to simulate randomness

        // Start the game for the sport
        switch (player->sport) {
            case BASEBALL:
                pthread_mutex_lock(&baseballMutex);
                if (NUM_BASEBALL_PLAYERS >= 18) {
                    playGame(BASEBALL, 18); // Baseball needs 18 players
                }
                pthread_mutex_unlock(&baseballMutex);
                break;

            case FOOTBALL:
                pthread_mutex_lock(&footballMutex);
                if (NUM_FOOTBALL_PLAYERS >= 22) {
                    playGame(FOOTBALL, 22); // Football needs 22 players
                }
                pthread_mutex_unlock(&footballMutex);
                break;

            case RUGBY:
                pthread_mutex_lock(&rugbyMutex);
                int rugbyPlayersToPlay = (NUM_RUGBY_PLAYERS > 30) ? 30 : NUM_RUGBY_PLAYERS;
                if (rugbyPlayersToPlay % 2 != 0) rugbyPlayersToPlay--; // Ensure it's even
                if (rugbyPlayersToPlay >= 2) {
                    playGame(RUGBY, rugbyPlayersToPlay); // Rugby needs at least 2 players, max 30
                }
                pthread_mutex_unlock(&rugbyMutex);
                break;
        }

        // Update the next sport to play (round-robin scheduling)
        if (nextSportToPlay == BASEBALL) {
            nextSportToPlay = FOOTBALL;
        } else if (nextSportToPlay == FOOTBALL) {
            nextSportToPlay = RUGBY;
        } else {
            nextSportToPlay = BASEBALL;
        }

        pthread_mutex_unlock(&fairnessMutex); // Release fairness lock
    }

    return NULL;
}

int main() {
    // Initialize all players
    initializePlayers();

    // Shuffle all players to assign random IDs from 1-140
    int allPlayerIndex = 0;
    for (int i = 0; i < NUM_BASEBALL_PLAYERS; i++) {
        allPlayers[allPlayerIndex].id = i + 1; // Assign ID
        allPlayerIndex++;
    }
    for (int i = 0; i < NUM_FOOTBALL_PLAYERS; i++) {
        allPlayers[allPlayerIndex].id = i + 37; // Assign ID
        allPlayerIndex++;
    }
    for (int i = 0; i < NUM_RUGBY_PLAYERS; i++) {
        allPlayers[allPlayerIndex].id = i + 81; // Assign ID
        allPlayerIndex++;
    }

    shufflePlayers(allPlayers, TOTAL_PLAYERS);

    // Create player threads
    pthread_t threads[TOTAL_PLAYERS];
    for (int i = 0; i < TOTAL_PLAYERS; i++) {
        pthread_create(&threads[i], NULL, playerThread, (void*)&allPlayers[i]);
    }

    // Wait for all threads to finish
    for (int i = 0; i < TOTAL_PLAYERS; i++) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}
