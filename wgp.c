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

pthread_mutex_t field_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t field_cond = PTHREAD_COND_INITIALIZER;
int active_sport = 0;  // 0: none, 1: baseball, 2: football, 3: rugby

typedef struct {
    int sport_id;         // Use an integer identifier for the sport
    int required_players;
    int max_players;
    int* player_list;
} Game;

void* play_game(void* arg) {
    Game* game = (Game*)arg;
    
    pthread_mutex_lock(&field_mutex);
    while (active_sport != 0 && active_sport != game->sport_id) {
        pthread_cond_wait(&field_cond, &field_mutex);
    }
    
    active_sport = game->sport_id;  // Set active sport using integer ID
    
    // Simulate game play
    for (int i = 0; i < game->required_players; i++) {
        printf("[Sport %d: %3d] Playing at Position %d\n", game->sport_id, game->player_list[i], i + 1);
        usleep(500000);  // Simulate time to play (half a second per position)
    }
    
    printf("[Sport %d: %3d] Game <<ENDED>>\n", game->sport_id, game->player_list[0]);
    
    // Finish the game and allow the field to be used by another sport
    active_sport = 0;
    pthread_cond_broadcast(&field_cond);
    
    pthread_mutex_unlock(&field_mutex);
    
    return NULL;
}

int main() {
    srand(time(NULL));

    // Set up the players for each sport
    int baseball_players[BASEBALL_PLAYERS];
    int football_players[FOOTBALL_PLAYERS];
    int rugby_players[RUGBY_PLAYERS];
    
    // Fill player IDs (just for simulation purposes)
    for (int i = 0; i < BASEBALL_PLAYERS; i++) baseball_players[i] = rand() % 100;
    for (int i = 0; i < FOOTBALL_PLAYERS; i++) football_players[i] = rand() % 100;
    for (int i = 0; i < RUGBY_PLAYERS; i++) rugby_players[i] = rand() % 100;

    // Create the sport games
    Game baseball = {1, BASEBALL_FIELD, BASEBALL_PLAYERS, baseball_players};  // Sport ID = 1 for Baseball
    Game football = {2, FOOTBALL_FIELD, FOOTBALL_PLAYERS, football_players};  // Sport ID = 2 for Football
    Game rugby = {3, RUGBY_MAX_PLAYERS, RUGBY_PLAYERS, rugby_players};         // Sport ID = 3 for Rugby
    
    // Create threads for sports (For simplicity, assuming we can start one sport at a time)
    pthread_t baseball_thread, football_thread, rugby_thread;
    
    pthread_create(&baseball_thread, NULL, play_game, (void*)&baseball);
    pthread_create(&football_thread, NULL, play_game, (void*)&football);
    pthread_create(&rugby_thread, NULL, play_game, (void*)&rugby);
    
    // Wait for threads to complete
    pthread_join(baseball_thread, NULL);
    pthread_join(football_thread, NULL);
    pthread_join(rugby_thread, NULL);
    
    return 0;
}
