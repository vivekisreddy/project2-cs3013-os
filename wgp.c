#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

// Define constants for sports and player counts 
#define BASEBALL_PLAYERS 36 
#define FOOTBALL_PLAYERS 44
#define RUGBY_PLAYERS 60 

// Define the number of teams for each sport 
#define BASEBALL_TEAMS 4 
#define FOOTBALL_TEAMS 4 
#define RUGBY_TEAMS 4 

// Define team sizes 
#define BASEBALL_TEAM_SIZE 9 
#define FOOTBALL_TEAM_SIZE 11 
#define RUGBY_TEAM_SIZE 15 

// Global variable to track the ongoing sport and synchronization primitives 
pthread_mutex_t field_lock; // Mutex to control access to the field 
sem_t field_semaphore; // Semaphore to control player entry 

// Variable to track the currently ongoing sport
char* current_sport = NULL;  // Could be "Baseball", "Football", or "Rugby"

// Function to initialize synchronization 
void init_sync() {
    pthread_mutex_init(&field_lock, NULL);
    sem_init(&field_semaphore, 0, 1);  // Only one sport can be played at a time
}

// Function to clean up synchronization
void destroy_sync() {
    pthread_mutex_destroy(&field_lock);
    sem_destroy(&field_semaphore);
}

// Function to simulate player action
void* player_action(void* arg) {
    // Simulate the random wait time for a player arriving at the park
    srand(time(NULL));
    int wait_time = rand() % 5 + 1;  // Wait between 1 to 5 seconds
    sleep(wait_time);

    // Cast the argument to a player type and sport
    char* player_type = (char*)arg;

    pthread_mutex_lock(&field_lock);

    // Check if the field is occupied by a different sport
    while (current_sport != NULL && strcmp(current_sport, player_type) != 0) {
        // If the sport is different, the player must wait for the field to clear
        pthread_mutex_unlock(&field_lock);
        sem_wait(&field_semaphore);  // Wait for field to become available
        pthread_mutex_lock(&field_lock);
    }

    // Now the player can play if it's their sport's turn
    if (current_sport == NULL) {
        current_sport = player_type;  // Set the field for this sport
    }

    // Simulate the player playing on the field
    printf("%s player enters the field.\n", player_type);
    int game_time = rand() % 3 + 1;  // Random game time (1 to 3 seconds)
    sleep(game_time);  // Simulate playing time
    
    // After playing, leave the field
    printf("%s player leaves the field.\n", player_type);

    // If all players of this sport leave, reset the sport to NULL
    current_sport = NULL;

    // Signal that the field is available for others
    sem_post(&field_semaphore);

    pthread_mutex_unlock(&field_lock);

    return NULL;
}

int main() {
    // Initialize synchronization mechanisms
    init_sync();

    // Simulate creating player threads for each sport
    pthread_t baseball_players[BASEBALL_PLAYERS];
    pthread_t football_players[FOOTBALL_PLAYERS];
    pthread_t rugby_players[RUGBY_PLAYERS];

    // Create baseball players
    for (int i = 0; i < BASEBALL_PLAYERS; i++) {
        char* player_type = "Baseball";
        pthread_create(&baseball_players[i], NULL, player_action, (void*)player_type);
    }

    // Create football players
    for (int i = 0; i < FOOTBALL_PLAYERS; i++) {
        char* player_type = "Football";
        pthread_create(&football_players[i], NULL, player_action, (void*)player_type);
    }

    // Create rugby players
    for (int i = 0; i < RUGBY_PLAYERS; i++) {
        char* player_type = "Rugby";
        pthread_create(&rugby_players[i], NULL, player_action, (void*)player_type);
    }

    // Join player threads
    for (int i = 0; i < BASEBALL_PLAYERS; i++) {
        pthread_join(baseball_players[i], NULL);
    }
    for (int i = 0; i < FOOTBALL_PLAYERS; i++) {
        pthread_join(football_players[i], NULL);
    }
    for (int i = 0; i < RUGBY_PLAYERS; i++) {
        pthread_join(rugby_players[i], NULL);
    }

    // Clean up synchronization before exiting
    destroy_sync();

    return 0;
}
