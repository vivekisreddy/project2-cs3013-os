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

// Function to manage the field and execute games for baseball, football, and rugby
void manageField() {
    // Play baseball game if there are enough players
    if (NUM_BASEBALL_PLAYERS >= 18) {
        playGame(BASEBALL, 18);
    }

    // Play football game if there are enough players
    if (NUM_FOOTBALL_PLAYERS >= 22) {
        playGame(FOOTBALL, 22);
    }

    // Play rugby game if there are enough players
    if (NUM_RUGBY_PLAYERS >= 2) {
        // Rugby allows even numbers, up to 30 players
        int rugbyPlayersToPlay = (NUM_RUGBY_PLAYERS > 30) ? 30 : NUM_RUGBY_PLAYERS;
        if (rugbyPlayersToPlay % 2 != 0) rugbyPlayersToPlay--; // Ensure it's even
        playGame(RUGBY, rugbyPlayersToPlay);
    }
}

int main() {
    // Initialize all players
    initializePlayers();

    // Manage the field and execute games
    manageField();

    return 0;
}
