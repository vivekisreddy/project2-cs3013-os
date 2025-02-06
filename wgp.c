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
    printf("[%s: %d] Game <<STARTED>>\n", (sport == BASEBALL) ? "Baseball" : (sport == FOOTBALL) ? "Football" : "Rugby", numPlayersRequired);
    
    for (int i = 0; i < numPlayersRequired; i++) {
        printf("[%s: %d] Playing at position %d\n", 
               (sport == BASEBALL) ? "Baseball" : (sport == FOOTBALL) ? "Football" : "Rugby", 
               players[i].id, 
               i + 1);
    }
    
    printf("[%s: %d] Game <<ENDED>>\n", (sport == BASEBALL) ? "Baseball" : (sport == FOOTBALL) ? "Football" : "Rugby", numPlayersRequired);
}

// Function to handle a player choosing a sport and playing
void* playerThread(void* arg) {
    Player* player = (Player*)arg;

    // Loop to keep playing games indefinitely
    while (1) {
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
