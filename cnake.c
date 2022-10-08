// ASCII snake in the terminal using ncurses.

// Includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include <ncurses.h>

// Constants
#define WIDTH 40
#define HEIGHT 20

// Structs
struct part{
	int x;
	int y;
	int id;
};

struct plyr{
	int x;
	int y;
	int grounded;
	int health;
	int points;
	int direction;
	double speed; // Between 0 and 1
	char sprite;
	char trail;
	struct part** tail; 
};

struct item{
	int x;
	int y;
	char sprite;
};

// Variables
char* VIDEO_BUFFER;
int BUFFER_SIZE = WIDTH*HEIGHT+HEIGHT;
struct part* initTail;
struct plyr player = {5, 5, 1, 100, 0, 1, 0.90, '#', '#', &initTail};
struct item food = {1, 1, '@'};
struct timeval start, stop;
char background = ' ';
double delta = 0;
double step = 0;
int stepSize = 1;
int quit = 0;

// Forward declarations
void update();
void editPixel(int x, int y, char val);
char getPixel(int x, int y);
void clearScreen();
void moveFood();
void gameOver();
void resetGame();
void moveTail();
void addTail();

// Main function
int main(int argc, char* argv[]){
	// Setup Buffer Memory
	clearScreen();
	gettimeofday(&start, NULL);
	VIDEO_BUFFER = malloc(BUFFER_SIZE*sizeof(char));
	initTail = malloc(100*sizeof(struct part));
	int col = 0;
	for(int i = 0; i < BUFFER_SIZE; i++){
		if(col == WIDTH){
			*(VIDEO_BUFFER+i) = '\0';
			col = 0;	
		}else{
			*(VIDEO_BUFFER+i) = background;
			col++;
		}
	}

	// Add Border
	for(int i = 0; i < HEIGHT; i++){
		editPixel(0, i, '|');
		editPixel(WIDTH-1, i, '|');
	}
	for(int i = 0; i < WIDTH; i++){
		editPixel(i, 0, '-');
		editPixel(i, HEIGHT-1, '-');
	}

	// Spawn Player and Food
	editPixel(player.x, player.y, player.sprite);
	moveFood();

	// Render
	while (quit != 1){
		// Time
		gettimeofday(&stop, NULL);
		delta = (double)(stop.tv_usec - start.tv_usec) / 1000000 + (double)(stop.tv_sec - start.tv_sec);
		gettimeofday(&start, NULL);
		step += delta;
		
		if(player.health > 0){
			// Frame
			for(int i = 0; i < HEIGHT; i++){
				char* line = (VIDEO_BUFFER+(i*(WIDTH+1)));
				mvprintw( i, 0, line);
			}
		
			// Score
			char score[12];
			snprintf(score, 12, "SCORE: %d", player.points);
			mvprintw(HEIGHT+1, 0, score);

			// Movement
		 	update();
		}else{
			char key = getch();
			if(key == 'R'){
				resetGame();
			}else if(key == 'Q'){
				quit = 1;
			}
		}
		// Wait
		sleep(0.01);
		refresh();
	}
	echo();
	curs_set(1);
	endwin();
	return 0;
}

void resetGame(){
	player.health = 100;
	player.x = WIDTH/2;
	player.y = HEIGHT/2;
	player.points = 0;
	player.direction = 0;
	initTail = malloc(sizeof(struct part)*100);
	player.tail = &initTail;

	// Setup Buffer Memory
        clearScreen();
        gettimeofday(&start, NULL);
        int col = 0;
        for(int i = 0; i < BUFFER_SIZE; i++){
                if(col == WIDTH){
                        *(VIDEO_BUFFER+i) = '\0';
                        col = 0;
                }else{
                        *(VIDEO_BUFFER+i) = background;
                        col++;
                }
        }

        // Add Border
        for(int i = 0; i < HEIGHT; i++){
                editPixel(0, i, '|');
                editPixel(WIDTH-1, i, '|');
        }
        for(int i = 0; i < WIDTH; i++){
                editPixel(i, 0, '-');
                editPixel(i, HEIGHT-1, '-');
        }

        // Spawn Player and Food
        editPixel(player.x, player.y, player.sprite);
        moveFood();

	return;
}

void update(){
	// Keyboard input
	char key = getch();
	if(key == 'Q'){
		quit = 1;
	}

	if(key == 'a'){
		player.direction = 3;
	}

	if(key == 'd'){
		player.direction = 1;
	}

	if(key == 's'){
		player.direction = 2;
	}

	if(key == 'w'){
		player.direction = 0;
	}

	if(key == ' '){
		player.points++;
	}

	// Collisions
	if(0){
		gameOver();
		return;
	}

	// Move Player
	if(step >= (1-player.speed)){
		if(player.direction == 1){
			if(getPixel(player.x+1, player.y) == '@'){
				player.points++;
				addTail();
				moveFood();
			}
			// Collisions
			if(player.x+1 >= WIDTH-1 || getPixel(player.x+1, player.y) == player.trail){
				gameOver();
				return;
			}
			// Move
			editPixel(player.x, player.y, background);
			moveTail();
			player.x+=stepSize;
			editPixel(player.x, player.y, player.sprite);
			step = 0;
		}else if(player.direction == 3){
			if(getPixel(player.x-1, player.y) == '@'){
                                player.points++;
				addTail();
				moveFood();
                        }
			// Collisions
			if(player.x-1 <= 0 || getPixel(player.x-1, player.y) == player.trail){
				gameOver();
				return;
			}
			// Move
			editPixel(player.x, player.y, background);
             		moveTail();
			player.x-=stepSize;
              		editPixel(player.x, player.y, player.sprite);
			step = 0;
		}else if(player.direction == 2 & step >= ((1-player.speed)*2)){
			if(getPixel(player.x, player.y+1) == '@'){
                                player.points++;
				addTail();
				moveFood();
                        }
			// Collisions
			if(player.y+1 >= HEIGHT-1 || getPixel(player.x, player.y+1) == player.trail){
				gameOver();
				return;
			}
			// Move
			editPixel(player.x, player.y, background);
                	moveTail();
			player.y+=stepSize;
                	editPixel(player.x, player.y, player.sprite);
			step = 0;
		}else if(player.direction == 0 & step >= ((1-player.speed)*2)){
			if(getPixel(player.x, player.y-1) == '@'){
                                player.points++;
				addTail();
				moveFood();
                        }
			// Collisions
			if(player.y-1 <= 0 | getPixel(player.x, player.y-1) == player.trail){
				gameOver();
				return;
			}
			// Move
			editPixel(player.x, player.y, background);
                	moveTail();
			player.y-=stepSize;
               		editPixel(player.x, player.y, player.sprite);
			step = 0;
		}
	}
	return;
}

void addTail(){
	((*player.tail)+player.points)->x = 999;
	((*player.tail)+player.points)->y = 999;
	((*player.tail)+player.points)->id = player.points;
	return;
}

void moveTail(){
	// Derender Tail
	for(int i = 0; i < player.points; i++){
        	if(((*player.tail)+i)->x != 999){
                	editPixel(((*player.tail)+i)->x, ((*player.tail)+i)->y, background);
                }
        }

	// Update Positions
	int prev_x = 0;
	int prev_y = 0;
	int curr_x = (*player.tail)->x;
	int curr_y = (*player.tail)->y;
	(*player.tail)->x = player.x;
	(*player.tail)->y = player.y;

	for(int i = 1; i < player.points; i++){
		prev_x = ((*player.tail)+i)->x;
		prev_y = ((*player.tail)+i)->y;

		((*player.tail)+i)->x = curr_x;
		((*player.tail)+i)->y = curr_y;

		curr_x = prev_x;
		curr_y = prev_y;
	}

	// Render Tail
	for(int i = 0; i < player.points; i++){
       		if(((*player.tail)+i)->x != 999){
               		editPixel(((*player.tail)+i)->x, ((*player.tail)+i)->y, player.trail);
        	}
        }
	return;
}

void editPixel(int x, int y, char val){
	int offset = x + (y*(WIDTH+1));
	*(VIDEO_BUFFER+offset) = val;
	return;
}

char getPixel(int x, int y){
	return *(VIDEO_BUFFER+(x+(y*(WIDTH+1))));
}

void moveFood(){
	srand(time(NULL));
	int x = (rand() % (WIDTH - 3 + 2)) + 2;
	int y = (rand() % (HEIGHT - 3 + 2)) + 2;
	if(x <= 0){
		x = 1;
	}else if(x >= WIDTH-1){
		x = WIDTH - 2;
	}

	if(y <= 0){
		y = 1;
	}else if(y >= HEIGHT-1){
		y = HEIGHT - 2;
	}
	editPixel(x, y, food.sprite);
}

void gameOver(){
	// Kill Player
	player.health = 0;
	// Clear Buffer
	int col = 0;
        for(int i = 0; i < BUFFER_SIZE; i++){
                if(col == WIDTH){
                        *(VIDEO_BUFFER+i) = '\0';
                        col = 0;
                }else{
                        *(VIDEO_BUFFER+i) = ' ';
                        col++;
                }
        }
	// Print Buffer
	for(int i = 0; i < HEIGHT; i++){
        	char* line = (VIDEO_BUFFER+(i*(WIDTH+1)));
		mvprintw( i, 0, line);
        }
	// Print Text
	char score[12];
        snprintf(score, 12, "SCORE: %d", player.points);
        mvprintw(HEIGHT/2, WIDTH/2-6, ".:Game Over:.");
	mvprintw(HEIGHT/2+1, WIDTH/2-4, score);
	mvprintw(HEIGHT/2+3, WIDTH/2-7, "Shift+R = Reset");
	mvprintw(HEIGHT/2+4, WIDTH/2-7, "Shift+Q = Quit");
	return;
}

void clearScreen(){
	initscr();
	erase();
	noecho();
	raw();
	timeout(0.1);
	move(0, 0);
	curs_set(0);
	refresh();
	return;
}
