#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

#define NUM_BASEBALL_PLAYERS 36
#define NUM_FOOTBALL_PLAYERS 44
#define NUM_RUGBY_PLAYERS 60

// Player types
typedef enum { BASEBALL, FOOTBALL, RUGBY, NONE } SportType;

// Player structure
typedef struct {
    int id;
    SportType sport;
} Player;

// Arrays to hold players for each sport
Player baseballPlayers[NUM_BASEBALL_PLAYERS];
Player footballPlayers[NUM_FOOTBALL_PLAYERS];
Player rugbyPlayers[NUM_RUGBY_PLAYERS];

// Global variables for field state
SportType currentSport = NONE; // Initially, no sport is using the field
pthread_mutex_t fieldMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t fieldCond = PTHREAD_COND_INITIALIZER;

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
        default:
            return; // Invalid sport
    }

    // Shuffle players randomly
    shufflePlayers(players, totalPlayers);

    // Ensure exactly the required number of players for the game
    printf("[%s: %d] Game <<STARTED>>\n", 
           (sport == BASEBALL) ? "Baseball" : (sport == FOOTBALL) ? "Football" : "Rugby", 
           numPlayersRequired);
    
    for (int i = 0; i < numPlayersRequired; i++) {
        printf("[%s: %d] Playing at position %d\n", 
               (sport == BASEBALL) ? "Baseball" : (sport == FOOTBALL) ? "Football" : "Rugby", 
               players[i].id, 
               i + 1);
        usleep(100000); // Simulate game play with a delay
    }
    
    printf("[%s: %d] Game <<ENDED>>\n", 
           (sport == BASEBALL) ? "Baseball" : (sport == FOOTBALL) ? "Football" : "Rugby", 
           numPlayersRequired);
}

// Function to manage the field and execute games for baseball, football, and rugby
void* manageField(void* arg) {
    SportType sport = *(SportType*)arg;

    pthread_mutex_lock(&fieldMutex);

    // Wait until the field is free for the current sport
    while (currentSport != NONE && currentSport != sport) {
        pthread_cond_wait(&fieldCond, &fieldMutex);
    }

    // Set the field to the current sport
    currentSport = sport;

    pthread_mutex_unlock(&fieldMutex);

    // Play the game
    switch (sport) {
        case BASEBALL:
            if (NUM_BASEBALL_PLAYERS >= 18) {
                playGame(BASEBALL, 18);
            }
            break;
        case FOOTBALL:
            if (NUM_FOOTBALL_PLAYERS >= 22) {
                playGame(FOOTBALL, 22);
            }
            break;
        case RUGBY:
            if (NUM_RUGBY_PLAYERS >= 2) {
                int rugbyPlayersToPlay = (NUM_RUGBY_PLAYERS > 30) ? 30 : NUM_RUGBY_PLAYERS;
                if (rugbyPlayersToPlay % 2 != 0) rugbyPlayersToPlay--; // Ensure it's even
                playGame(RUGBY, rugbyPlayersToPlay);
            }
            break;
        default:
            break;
    }

    // Release the field
    pthread_mutex_lock(&fieldMutex);
    currentSport = NONE;
    pthread_cond_broadcast(&fieldCond); // Notify all waiting threads
    pthread_mutex_unlock(&fieldMutex);

    return NULL;
}

int main() {
    // Seed the random number generator
    srand(time(NULL));

    // Initialize all players
    initializePlayers();

    // Create threads for each sport
    pthread_t baseballThread, footballThread, rugbyThread;
    SportType baseball = BASEBALL, football = FOOTBALL, rugby = RUGBY;

    pthread_create(&baseballThread, NULL, manageField, &baseball);
    pthread_create(&footballThread, NULL, manageField, &football);
    pthread_create(&rugbyThread, NULL, manageField, &rugby);

    // Wait for all threads to finish
    pthread_join(baseballThread, NULL);
    pthread_join(footballThread, NULL);
    pthread_join(rugbyThread, NULL);

    return 0;
}