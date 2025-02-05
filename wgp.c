#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BASEBALL_PLAYERS 36
#define FOOTBALL_PLAYERS 44
#define RUGBY_PLAYERS 60

#define BASEBALL_FIELD 18
#define FOOTBALL_FIELD 22
#define RUGBY_MAX_PLAYERS 30

// Struct to represent each player
typedef struct {
    int player_id;
    int sport_id;  // Sport the player is participating in
} Player;

// Mutex for the field to ensure only one player from any sport plays at any given time
pthread_mutex_t field_mutex = PTHREAD_MUTEX_INITIALIZER;

// Array of sport names and number of players for each sport
const char* sport_names[] = {"Baseball", "Football", "Rugby"};
int player_count[] = {BASEBALL_PLAYERS, FOOTBALL_PLAYERS, RUGBY_PLAYERS};
int total_sports = 3;

// Mutex lock and thread control
void* play_game(void* arg) {
    Player* player = (Player*)arg;

    while (1) {
        // Each player tries to lock the field mutex to access the field and play
        if (pthread_mutex_trylock(&field_mutex) == 0) {
            // Successfully acquired the field (playing the game)
            printf("[Player %d from Sport %d] Ready to play\n", player->player_id, player->sport_id);
            
            // Simulate the game, player-specific task
            if (player->sport_id == 3) {  // Rugby: Maximize player pairing
                printf("[Player %d from Rugby] Playing with a partner\n", player->player_id);
                sleep(1);
            } else {  // Baseball and Football: Exact number of players
                printf("[Player %d from Sport %d] Playing at position\n", player->player_id, player->sport_id);
                sleep(1);
            }

            // Game ends for this player
            printf("[Player %d from Sport %d] Finished\n", player->player_id, player->sport_id);

            // Release the field for other players
            pthread_mutex_unlock(&field_mutex);
            break;
        } else {
            // If the field is occupied, the player waits for a short random time and tries again
            usleep(rand() % 100000);
        }
    }

    return NULL;
}

int main() {
    FILE* seed_file = fopen("seed.txt", "r");
    if (seed_file == NULL) {
        perror("Unable to open seed.txt");
        return 1;
    }

    unsigned int seed;
    if (fscanf(seed_file, "%u", &seed) != 1) {
        perror("Invalid seed value in seed.txt");
        fclose(seed_file);
        return 1;
    }
    fclose(seed_file);

    // Seed the random number generator
    srand(seed);

    // Set up the players for each sport
    Player baseball_players[BASEBALL_PLAYERS];
    Player football_players[FOOTBALL_PLAYERS];
    Player rugby_players[RUGBY_PLAYERS];

    // Fill player details and assign them to respective sports
    for (int i = 0; i < BASEBALL_PLAYERS; i++) baseball_players[i] = (Player){i + 1, 1};
    for (int i = 0; i < FOOTBALL_PLAYERS; i++) football_players[i] = (Player){i + 1, 2};
    for (int i = 0; i < RUGBY_PLAYERS; i++) rugby_players[i] = (Player){i + 1, 3};

    // Print sport and number of players
    for (int i = 0; i < total_sports; i++) {
        printf("[%s: %d] Pair ready\n", sport_names[i], player_count[i]);
        printf("[%s: %d] Game <<STARTED>>\n", sport_names[i], player_count[i]);

        // Loop through each player and print actions
        for (int j = 0; j < player_count[i]; j++) {
            printf("[%s: %d] Playing at position %d\n", sport_names[i], j+1, j+1);
        }

        // Optionally, end the game for the sport
        printf("[%s: %d] Game <<ENDED>>\n", sport_names[i], player_count[i]);
    }

    // Create player threads for each sport
    pthread_t baseball_threads[BASEBALL_PLAYERS], football_threads[FOOTBALL_PLAYERS], rugby_threads[RUGBY_PLAYERS];

    // Create player threads
    for (int i = 0; i < BASEBALL_PLAYERS; i++) {
        pthread_create(&baseball_threads[i], NULL, play_game, (void*)&baseball_players[i]);
    }
    for (int i = 0; i < FOOTBALL_PLAYERS; i++) {
        pthread_create(&football_threads[i], NULL, play_game, (void*)&football_players[i]);
    }
    for (int i = 0; i < RUGBY_PLAYERS; i++) {
        pthread_create(&rugby_threads[i], NULL, play_game, (void*)&rugby_players[i]);
    }

    // Wait for all player threads to complete
    for (int i = 0; i < BASEBALL_PLAYERS; i++) {
        pthread_join(baseball_threads[i], NULL);
    }
    for (int i = 0; i < FOOTBALL_PLAYERS; i++) {
        pthread_join(football_threads[i], NULL);
    }
    for (int i = 0; i < RUGBY_PLAYERS; i++) {
        pthread_join(rugby_threads[i], NULL);
    }

    return 0;
}
