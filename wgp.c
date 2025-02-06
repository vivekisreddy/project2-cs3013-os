#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define NUM_BASEBALL_PLAYERS 36
#define NUM_FOOTBALL_PLAYERS 44
#define NUM_RUGBY_PLAYERS 60

// Player types
typedef enum { BASEBALL, FOOTBALL, RUGBY } SportType;

// Player structure
typedef struct {
    int id;
    SportType sport;
} Player;

// Arrays to hold players for each sport
Player baseballPlayers[NUM_BASEBALL_PLAYERS];
Player footballPlayers[NUM_FOOTBALL_PLAYERS];
Player rugbyPlayers[NUM_RUGBY_PLAYERS];

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
    // Initialize baseball players
    for (int i = 0; i < NUM_BASEBALL_PLAYERS; i++) {
        baseballPlayers[i].id = i + 1;
        baseballPlayers[i].sport = BASEBALL;
    }

    // Initialize football players
    for (int i = 0; i < NUM_FOOTBALL_PLAYERS; i++) {
        footballPlayers[i].id = i + 1;
        footballPlayers[i].sport = FOOTBALL;
    }

    // Initialize rugby players
    for (int i = 0; i < NUM_RUGBY_PLAYERS; i++) {
        rugbyPlayers[i].id = i + 1;
        rugbyPlayers[i].sport = RUGBY;
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

// Function to play a game for baseball, football, or rugby
void playGame(SportType sport, int numPlayersRequired) {
    Player *players;
    int totalPlayers;

    // Select appropriate sport and number of players
    switch (sport) {
        case BASEBALL:
            players = baseballPlayers;
            totalPlayers = NUM_BASEBALL_PLAYERS;
            break;
        case FOOTBALL:
            players = footballPlayers;
            totalPlayers = NUM_FOOTBALL_PLAYERS;
            break;
        case RUGBY:
            players = rugbyPlayers;
            totalPlayers = NUM_RUGBY_PLAYERS;
            break;
    }

    // Shuffle players randomly
    shufflePlayers(players, totalPlayers);

    // Ensure exactly the required number of players for the game
    printf("[%s: %d] Game <<STARTED>>\n", 
           (sport == BASEBALL) ? "Baseball" : (sport == FOOTBALL) ? "Football" : "Rugby", numPlayersRequired);

    if (sport == RUGBY) {
        // Rugby players must be paired and played sequentially
        int pairCount = numPlayersRequired / 2; // Number of pairs

        for (int i = 0; i < pairCount; i++) {
            int player1 = i * 2;
            int player2 = i * 2 + 1;
            
            // Print when a pair is ready
            printf("[%s: %d] Pair ready\n", "Rugby", players[player1].id);

            // Print when players are playing at a position
            printf("[%s: %d] Playing at position %d\n", "Rugby", players[player1].id, (i * 2) + 1);
            printf("[%s: %d] Playing at position %d\n", "Rugby", players[player2].id, (i * 2) + 2);
        }
    } else {
        // For Baseball and Football, print players as usual
        for (int i = 0; i < numPlayersRequired; i++) {
            printf("[%s: %d] Playing at position %d\n", 
                   (sport == BASEBALL) ? "Baseball" : (sport == FOOTBALL) ? "Football" : "Rugby", 
                   players[i].id, 
                   i + 1);
        }
    }

    printf("[%s: %d] Game <<ENDED>>\n", 
           (sport == BASEBALL) ? "Baseball" : (sport == FOOTBALL) ? "Football" : "Rugby", numPlayersRequired);
}

// Function to handle a player choosing a sport and playing
void* playerThread(void* arg) {
    Player* player = (Player*)arg;

    // Loop to keep playing games indefinitely
    while (1) {
        pthread_mutex_lock(&fairnessMutex); // Lock fairness mutex to enforce turn-based play

        // Wait for the player's sport to be the one that gets to play
        while (player->sport != nextSportToPlay) {
            pthread_mutex_unlock(&fairnessMutex); // Release the fairness mutex so other threads can run
            sched_yield(); // Yield to allow other threads to progress
            pthread_mutex_lock(&fairnessMutex); // Re-acquire fairness lock
        }

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

    // Create threads for all players
    pthread_t playerThreads[NUM_BASEBALL_PLAYERS + NUM_FOOTBALL_PLAYERS + NUM_RUGBY_PLAYERS];
    int threadIndex = 0;

    // Create threads for baseball players
    for (int i = 0; i < NUM_BASEBALL_PLAYERS; i++) {
        pthread_create(&playerThreads[threadIndex++], NULL, playerThread, &baseballPlayers[i]);
    }

    // Create threads for football players
    for (int i = 0; i < NUM_FOOTBALL_PLAYERS; i++) {
        pthread_create(&playerThreads[threadIndex++], NULL, playerThread, &footballPlayers[i]);
    }

    // Create threads for rugby players
    for (int i = 0; i < NUM_RUGBY_PLAYERS; i++) {
        pthread_create(&playerThreads[threadIndex++], NULL, playerThread, &rugbyPlayers[i]);
    }

    // Wait for all threads to finish (note: threads will run indefinitely)
    for (int i = 0; i < NUM_BASEBALL_PLAYERS + NUM_FOOTBALL_PLAYERS + NUM_RUGBY_PLAYERS; i++) {
        pthread_join(playerThreads[i], NULL);
    }

    return 0;
}
