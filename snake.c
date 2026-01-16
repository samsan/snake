#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>
#include <time.h>

/*
 * This is a simple exercise to consolidate my basic C knowledge, 
 * while stretching my mind and stengthening my focus capabilities. 
 * This is Snake: the game everyone knows about.
 * */

#define SNAKE_TILE '*'
#define BOARD_TILE '.'
#define BOARD_HEIGHT 15
#define BOARD_WIDTH 15
#define BOARD_TILES (BOARD_HEIGHT * BOARD_WIDTH)
#define APPLE_TILE '@'

typedef enum {
	UP,
	DOWN,
	RIGHT,
	LEFT
} Direction;

/* Snake is a linked list of (x,y) values that map the snake state on a board */
struct snake {
	int x;
	int y;
	struct snake *prev;
};

struct apple {
	int x;
	int y;
};

/* Checks if the snake eat itselfi. */
bool snake_eat_itself(struct snake *s){
    struct snake *head = s;
    if (s->prev == NULL) return false;
    struct snake *body_part = s->prev;
    
    while(1){ 
        if ((body_part->x == head->x) && (body_part->y == head->y)) return true;
        if (body_part->prev == NULL) return false;
	body_part = body_part->prev;
    }
}

/* Game quits showing the total points. */
void quit_game(int *player_points){
	nodelay(stdscr, FALSE); // Stop the game from running in the background
        erase();
        if(*player_points == BOARD_TILES){
            printw("YOU WON!!! ALL APPLES EATEN!");
        } else {
            printw("You're dead! TOTAL APPLES: %d\n", *player_points);
            printw("Press any key to exit...");
        }
        refresh();
        getch(); // Wait for user to see the score
        endwin();
        exit(0);
}

/* Moves the snake to the next tile with a specific direction */ 
void move_snake_head(struct snake *s, Direction d){
	switch(d){
		case UP: s->y = s->y == 0 ? BOARD_HEIGHT - 1 : s-> y - 1; break; 
		case DOWN: s->y = s->y == BOARD_HEIGHT - 1? 0 : s-> y + 1; break; 
		case LEFT: s->x = s->x == 0 ? BOARD_WIDTH -1 : s->x - 1; break; 
		case RIGHT: s->x = s->x == BOARD_WIDTH - 1 ? 0 : s->x + 1; break; 
	}
}

/* Traslates the snake pieces to  the head direction. */
void move_snake_body(struct snake *s){
	if (s->prev == NULL) return;
	move_snake_body(s->prev);
	s->prev->x = s->x;
	s->prev->y = s->y;
	return;
}

/* Enlarges snake adding a new head (the new piece) */
struct snake* enlarge_snake(struct snake *s, Direction d){
	struct snake *new_head = malloc(sizeof(*new_head));
	new_head->x = s->x;
	new_head->y = s->y;
 	new_head->prev = s;	
	move_snake_head(new_head, d);
	return new_head;
}

/* Updates the snake posiiion: moves the head depending on the snake direction.
the other snake pieces are traslated: the prev piece takes the actual piece's coordinates */
void move_snake(struct snake *s, Direction d){		
	move_snake_body(s);
	move_snake_head(s,d);
}

/* Checks if a specific tile is covered by the snake */
bool is_snake_on_tile(struct snake *s, int tile){
	while(1){
		int snake_flattened_position = s->x + (s->y * BOARD_WIDTH);
		if (tile == snake_flattened_position) return true;
		if (s->prev == NULL) return false;
		s = s->prev;
	}
}

/* Sets the board's state, for all its tiles, given the snake positions 
and the apple position. 
*/
void set_board_state(char *board, struct snake *s, int *apple_position){
	for (int i = 0; i< BOARD_TILES; i++){
		if(is_snake_on_tile(s,i)){
			board[i] = SNAKE_TILE;
		} else if (i == *apple_position){
			board[i] = APPLE_TILE;
		} else{
			board[i] = BOARD_TILE;
		}
	}	
}

/* Prints the board's state  */
void print_board(char *board, int *player_points){
	erase();
	printw("TOTAL APPLES %d\n", *player_points);
	for(int i=0; i<BOARD_TILES;i++){
		printw("%c",board[i]);
		if((i+1) % BOARD_WIDTH == 0){
			printw("\n");
		}
	}
	refresh();
}

/* Returns a new random position of the apple, inside the board */
int new_apple(struct snake *s){
	while(1){
		int new_apple_position = rand() % (BOARD_TILES-1);
		if(!is_snake_on_tile(s,new_apple_position)) return new_apple_position;
	}
}	


int main(int argc, char **argv){
	initscr();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);
	nodelay(stdscr, TRUE);

	char board[BOARD_TILES];
	int player_points = 0;

	struct snake *s = malloc(sizeof(s));
	s->x = 1;
	s->y = 1;
	s->prev = NULL;

	Direction snake_direction = RIGHT;
	int apple_position = new_apple(s);	
	srand(time(NULL));

	int ch;

	while((ch = getch()) != 'q'){
		switch(ch){
			case KEY_UP: snake_direction = UP; break; 
			case KEY_DOWN: snake_direction = DOWN; break; 
			case KEY_RIGHT: snake_direction = RIGHT; break; 
			case KEY_LEFT: snake_direction = LEFT; break; 
		}
		move_snake(s, snake_direction);

		if(snake_eat_itself(s)) quit_game(&player_points);
		
		if(is_snake_on_tile(s,apple_position)) {
			player_points++;
			s = enlarge_snake(s, snake_direction);
			apple_position = new_apple(s);
		}

		set_board_state(board,s, &apple_position);
		print_board(board, &player_points);
		usleep(100000 - player_points * 1000);
	}
	return 0;
}
