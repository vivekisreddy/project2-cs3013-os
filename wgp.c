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

#define MAX_TURNS 3 // Maximum number of turns a sport can monopolize the field

pthread_mutex_t field_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t field_cond = PTHREAD_COND_INITIALIZER;

// Struct for representing each sport in the queue
typedef struct SportNode {
    int sport_id;
    int turns_remaining;
    struct SportNode* next;
} SportNode;

SportNode* queue_head = NULL;  // Head of the queue
SportNode* queue_tail = NULL;  // Tail of the queue

// Queue functions
void enqueue(int sport_id) {
    SportNode* new_node = (SportNode*)malloc(sizeof(SportNode));
    new_node->sport_id = sport_id;
    new_node->turns_remaining = MAX_TURNS;  // Initialize turns for the sport
    new_node->next = NULL;
    
    if (queue_tail == NULL) {
        queue_head = new_node;
        queue_tail = new_node;
    } else {
        queue_tail->next = new_node;
        queue_tail = new_node;
    }
}

int dequeue() {
    if (queue_head == NULL) {
        return -1;  // Empty queue
    }
    
    SportNode* temp = queue_head;
    int sport_id = temp->sport_id;
    queue_head = queue_head->next;
    
    if (queue_head == NULL) {
        queue_tail = NULL;
    }
    
    free(temp);
    return sport_id;
}

// Game struct
typedef struct {
    int sport_id;         // Use an integer identifier for the sport
    int required_players;
    int max_players;
    int* player_list;
} Game;

void* play_game(void* arg) {
    Game* game = (Game*)arg;

    pthread_mutex_lock(&field_mutex);

    // Enqueue the current sport to the queue
    enqueue(game->sport_id);

    // Wait until it's this sport's turn
    while (queue_head->sport_id != game->sport_id) {
        pthread_cond_wait(&field_cond, &field_mutex);
    }

    // Once it's the sport's turn, proceed with playing
    if (queue_head->turns_remaining <= 0) {
        // Skip the sport if it has reached the maximum turns
        dequeue();  // Remove the sport from the queue
        pthread_cond_broadcast(&field_cond);  // Allow the next sport to play
        pthread_mutex_unlock(&field_mutex);
        return NULL;
    }

    // Decrease remaining turns for the current sport
    queue_head->turns_remaining--;

    // Play the game depending on the sport
    if (game->sport_id == 3) {  // Rugby: Maximize player pairing
        int players_on_field = 0;

        for (int i = 0; i < game->required_players; i += 2) {
            if (players_on_field + 2 <= game->max_players) {
                printf("[Sport %d] Pairing players %d and %d\n", game->sport_id, game->player_list[i], game->player_list[i+1]);
                sleep(0.5);
                players_on_field += 2;
            } else {
                break;
            }
        }

        printf("[Sport %d] Game ENDED with %d players\n", game->sport_id, players_on_field);
    } else {  // Baseball and Football: Exact number of players
        for (int i = 0; i < game->required_players; i++) {
            printf("[Sport %d: %d] Playing at Position %d\n", game->sport_id, game->player_list[i], i + 1);
            sleep(0.5);
        }
        printf("[Sport %d] Game Ended\n", game->sport_id);
    }

    // Finish the game and allow the field to be used by another sport
    dequeue();  // Remove this sport from the queue

    // Notify the next sport to play
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

    // Create threads for sports
    pthread_t baseball_thread, football_thread, rugby_thread;

    // Enqueue all the sports initially
    enqueue(1);  // Baseball
    enqueue(2);  // Football
    enqueue(3);  // Rugby

    pthread_create(&baseball_thread, NULL, play_game, (void*)&baseball);
    pthread_create(&football_thread, NULL, play_game, (void*)&football);
    pthread_create(&rugby_thread, NULL, play_game, (void*)&rugby);

    // Wait for threads to complete
    pthread_join(baseball_thread, NULL);
    pthread_join(football_thread, NULL);
    pthread_join(rugby_thread, NULL);

    return 0;
}
