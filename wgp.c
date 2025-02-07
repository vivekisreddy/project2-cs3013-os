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
    pthread_mutex_t *sportMutex;  // Mutex specific to the sport
} Player;

// Arrays to hold all players
Player allPlayers[TOTAL_PLAYERS];

// Mutexes for each sport
pthread_mutex_t baseballMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t footballMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t rugbyMutex = PTHREAD_MUTEX_INITIALIZER;

// Function to initialize the players for each sport
void initializePlayers() {
    int playerIndex = 0;
   
    // Initialize baseball players
    for (int i = 0; i < NUM_BASEBALL_PLAYERS; i++) {
        allPlayers[playerIndex].id = 0; // Placeholder for now, will shuffle later
        allPlayers[playerIndex].sport = BASEBALL;
        allPlayers[playerIndex].sportMutex = &baseballMutex;
        playerIndex++;
    }

    // Initialize football players
    for (int i = 0; i < NUM_FOOTBALL_PLAYERS; i++) {
        allPlayers[playerIndex].id = 0; // Placeholder for now, will shuffle later
        allPlayers[playerIndex].sport = FOOTBALL;
        allPlayers[playerIndex].sportMutex = &footballMutex;
        playerIndex++;
    }

    // Initialize rugby players
    for (int i = 0; i < NUM_RUGBY_PLAYERS; i++) {
        allPlayers[playerIndex].id = 0; // Placeholder for now, will shuffle later
        allPlayers[playerIndex].sport = RUGBY;
        allPlayers[playerIndex].sportMutex = &rugbyMutex;
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
    int gameTime = rand() % 3 + 1;
    sleep(gameTime);  // Simulate the game duration
    printf("Game played for %d seconds\n", gameTime);
}

// Function to simulate game time for rugby players
void simulateRugbyGameTime() {
    int gameTime = rand() % 3 + 1;
    sleep(gameTime);  // Simulate the game duration
    printf("Pair played for %d seconds\n", gameTime);
}

// Function to play a game for rugby with pairs
void playRugbyGame(Player* players, int numPlayersRequired) {
    int pairCount = numPlayersRequired / 2;

    for (int i = 0; i < pairCount; i++) {
        int player1 = i * 2;
        int player2 = i * 2 + 1;

        printf("[Rugby: %d] Pair ready\n", players[player1].id);

        printf("[Rugby: %d] Playing at position %d\n", players[player1].id, (i * 2) + 1);
        printf("[Rugby: %d] Playing at position %d\n", players[player2].id, (i * 2) + 2);

        simulateRugbyGameTime();
    }
}

// Function to play a game for any sport
void playGame(SportType sport, int numPlayersRequired) {
    Player *players;
    int totalPlayers;

    switch (sport) {
        case BASEBALL:
            players = allPlayers;
            totalPlayers = NUM_BASEBALL_PLAYERS;
            break;
        case FOOTBALL:
            players = allPlayers + NUM_BASEBALL_PLAYERS;
            totalPlayers = NUM_FOOTBALL_PLAYERS;
            break;
        case RUGBY:
            players = allPlayers + NUM_BASEBALL_PLAYERS + NUM_FOOTBALL_PLAYERS;
            totalPlayers = NUM_RUGBY_PLAYERS;
            break;
    }

    shufflePlayers(players, totalPlayers);

    printf("[%s: %d] Game <<STARTED>>\n",
           (sport == BASEBALL) ? "Baseball" : (sport == FOOTBALL) ? "Football" : "Rugby", numPlayersRequired);

    if (sport == RUGBY) {
        int rugbyPlayersToPlay = (NUM_RUGBY_PLAYERS > 30) ? 30 : NUM_RUGBY_PLAYERS;
        if (rugbyPlayersToPlay % 2 != 0) rugbyPlayersToPlay--;
        if (rugbyPlayersToPlay >= 2) {
            playRugbyGame(players, rugbyPlayersToPlay);
        }
    } else {
        for (int i = 0; i < numPlayersRequired; i++) {
            printf("[%s: %d] Playing at position %d\n",
                   (sport == BASEBALL) ? "Baseball" : (sport == FOOTBALL) ? "Football" : "Rugby",
                   players[i].id, i + 1);
        }

        simulateGameTime();
    }

    printf("[%s: %d] Game <<ENDED>>\n",
           (sport == BASEBALL) ? "Baseball" : (sport == FOOTBALL) ? "Football" : "Rugby", numPlayersRequired);
}

// Function to handle a player choosing a sport and playing
void* playerThread(void* arg) {
    Player* player = (Player*)arg;

    int seed = readSeedFromFile("seed.txt");
    srand(seed);

    while (1) {
        // Wait for this sport's turn
        pthread_mutex_lock(player->sportMutex); // Lock the sport's mutex to simulate that it's this sport's turn
        int delayTime = rand() % 1000000;
        usleep(delayTime); // Simulate randomness

        int isPlaying = 0;

        // Play the game for the current sport
        switch (player->sport) {
            case BASEBALL:
                if (NUM_BASEBALL_PLAYERS >= 18) {
                    playGame(BASEBALL, 18); // Baseball needs 18 players
                    isPlaying = 1;
                }
                break;
            case FOOTBALL:
                if (NUM_FOOTBALL_PLAYERS >= 22) {
                    playGame(FOOTBALL, 22); // Football needs 22 players
                    isPlaying = 1;
                }
                break;
            case RUGBY:
                int rugbyPlayersToPlay = (NUM_RUGBY_PLAYERS > 30) ? 30 : NUM_RUGBY_PLAYERS;
                if (rugbyPlayersToPlay % 2 != 0) rugbyPlayersToPlay--;
                if (rugbyPlayersToPlay >= 2) {
                    playGame(RUGBY, rugbyPlayersToPlay); // Rugby needs at
                    isPlaying = 1;
                }

                break;
        }
        if (!isPlaying) {
            int sleepTime = rand() % 3 + 1; // Sleep for 1 to 3 seconds
            sleep(sleepTime);
        }

        // Release the sport mutex so other threads of this sport can play next
        pthread_mutex_unlock(player->sportMutex);
    }

    return NULL;
}

int main() {
    // Initialize players
    initializePlayers();

    // Shuffle all players and assign IDs
    int allPlayerIndex = 0;
    for (int i = 0; i < NUM_BASEBALL_PLAYERS; i++) {
        allPlayers[allPlayerIndex].id = i + 1;
        allPlayerIndex++;
    }
    for (int i = 0; i < NUM_FOOTBALL_PLAYERS; i++) {
        allPlayers[allPlayerIndex].id = i + 37;
        allPlayerIndex++;
    }
    for (int i = 0; i < NUM_RUGBY_PLAYERS; i++) {
        allPlayers[allPlayerIndex].id = i + 81;
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