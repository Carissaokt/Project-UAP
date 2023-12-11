#include <iostream>
#include <ncurses/ncurses.h>
#include <cstdlib>
#include <cstring>
#include <windows.h>
#include <ctime>
#include <mmsystem.h>

#define WIN_WIDTH 70
#define SCREEN_WIDTH 90
#define SCREEN_HEIGHT 26
#define GAP_SIZE 7

using namespace std;

void showLoading();
void play();
void instructions();
void drawBorder();
void genPipe(int ind);
void updateScore();
void drawBunny();
void drawPipe(int ind);
int collision();
void gameover();
void eraseBunny();
void erasePipe(int ind);
void setcursor(bool visible);
void gotoxy(int x, int y);
void drawCarrot(int x, int y);
void eraseCarrot(int x, int y);
void generateCarrot();

int pipePos[3];
int gapPos[3];
int pipeFlag[3];

char bunny[3][5] = {
        {'/', '_', '\\', ' ', '/'},
        {'(', ' ', 'o', '.', 'o'},
        {' ', '>', '^', '<', ' '}
    };

int bunnyPos = 6;
int score = 0;

int main() {
	
    initscr(); 
    setcursor(0);
    
    start_color();
    init_pair(1, COLOR_YELLOW, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);   
	init_pair(3, COLOR_RED, COLOR_BLACK);
	init_pair(4, COLOR_CYAN, COLOR_BLACK);
	init_pair(5, COLOR_MAGENTA, COLOR_BLACK);
	
    srand(static_cast<unsigned>(time(nullptr)));

    do {
        clear(); 
        
        mvprintw(5, 10, " -------------------------- ");
        mvprintw(6, 10, " |        Bunny Run       | ");
        mvprintw(7, 10, " --------------------------");

        mvprintw(9, 10, "1. Start Game");
        mvprintw(10, 10, "2. Instructions");
        mvprintw(11, 10, "3. Quit");

        mvprintw(13, 10, "Select option: ");
        char op = getch();

        if (op == '1') showLoading(), play();
        else if (op == '2') instructions();
        else if (op == '3') {
            endwin(); 
            exit(0);
        }

    } while (true);

    endwin(); 
    return 0;
}


void drawCarrot(int x, int y) {
	attron(COLOR_PAIR(1)); 
    mvprintw(y, x, "@");
    attroff(COLOR_PAIR(1));
}


void eraseCarrot(int x, int y) {
    mvprintw(y, x, " ");
}


void showLoading(){
    clear();
    attron(COLOR_PAIR(4));
    for(int i=0;i<2;i++){
		mvprintw(2,10,"Loading......");
		mvprintw(3,10,"~~~~~~~~~~~~~~~~~~~~~~~~~");
		mvprintw(4,10,"|                       |");
		mvprintw(5,10,"~~~~~~~~~~~~~~~~~~~~~~~~~");
		for(int g=1;g<23;g++){
			mvprintw(4,10+g,">>");
			refresh();
			Sleep(100);
	    }
	}
	attroff(COLOR_PAIR(4));
	 endwin();
}


void play() {
    bool crossedCarrot = false; 
    bunnyPos = 6;
    score = 0;
    pipeFlag[0] = 1;
    pipeFlag[1] = 0;
    pipePos[0] = pipePos[1] = 4;
    bool gameOver = false;

    clear();
    drawBorder();
    genPipe(0);
    updateScore();

    mvprintw(2, WIN_WIDTH + 5, "BUNNY RUN");
    mvprintw(4, WIN_WIDTH + 6, "----------");
    mvprintw(6, WIN_WIDTH + 6, "----------");
    mvprintw(12, WIN_WIDTH + 7, "Control ");
    mvprintw(13, WIN_WIDTH + 7, "-------- ");
    mvprintw(14, WIN_WIDTH + 2, " Spacebar = jump");

    mvprintw(5, 10, "Press any key to start");
    getch();
    mvprintw(5, 10, "                      ");

    PlaySound(TEXT("Run-Amok.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP); 

    timeout(0);

    while (!gameOver) {
        int ch = getch();
        if (ch == 32) {
            if (bunnyPos > 3) {
                bunnyPos -= 3;
            }
        }
        if (ch == 27) {
            break;
        }

        drawBunny();
        drawPipe(0);
        drawPipe(1);

        if (pipeFlag[0] == 1) {
            drawCarrot(WIN_WIDTH - pipePos[0] - 1, gapPos[0] + GAP_SIZE / 2);
            crossedCarrot = true;
        }

        if (collision() == 1) {
            PlaySound(0, 0, 0);
            gameover();
            gameOver = true;
        }

        refresh();
        napms(100);

        eraseBunny();
        erasePipe(0);
        erasePipe(1);
        eraseCarrot(WIN_WIDTH - pipePos[0] - 1, gapPos[0] + GAP_SIZE / 2);
        bunnyPos += 1;

        if (bunnyPos > SCREEN_HEIGHT - 2) {
            gameover();
            gameOver = true;
        }

        if (pipeFlag[0] == 1)
            pipePos[0] += 2;

        if (pipeFlag[1] == 1)
            pipePos[1] += 2;

        if (pipePos[0] >= 40 && pipePos[0] < 42) {
            if (crossedCarrot && bunnyPos == gapPos[0] + GAP_SIZE / 2) {
                score += 1; 
                crossedCarrot = false; 
            }
            
            pipeFlag[1] = 1;
            pipePos[1] = 4;
            genPipe(1);
        }

        if (pipePos[0] > 68) {
        	score++;
        	updateScore();
            pipeFlag[1] = 0;
            pipePos[0] = pipePos[1];
            gapPos[0] = gapPos[1];
        }
    }

    timeout(-1);
}


void drawBorder() {
		attron(COLOR_PAIR(5));
    for (int i = 0; i < SCREEN_WIDTH; i++) {
        mvprintw(0, i, "|");
        mvprintw(SCREEN_HEIGHT, i, "|");
    }

    for (int i = 0; i < SCREEN_HEIGHT; i++) {
        mvprintw(i, 0, "|");
        mvprintw(i, SCREEN_WIDTH, "|");
    }

    for (int i = 0; i < SCREEN_HEIGHT; i++) {
        mvprintw(i, WIN_WIDTH, "|");
    }
    attroff(COLOR_PAIR(5));
}


void genPipe(int ind) {
    gapPos[ind] = 3 + rand() % 14;
}


void updateScore() {
    mvprintw(5, WIN_WIDTH + 7, "Score: %d", score);
}


void drawBunny() {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 5; j++) {
            mvprintw(i + bunnyPos, j + 2, "%c", bunny[i][j]);
        }
    }
}


void drawPipe(int ind) {
    if (pipeFlag[ind] == 1) {
    	attron(COLOR_PAIR(2));
        
        for (int i = 0; i < gapPos[ind]; i++) {
            mvprintw(i + 1, WIN_WIDTH - pipePos[ind], "**");
		}

        for (int i = gapPos[ind] + 1; i <= gapPos[ind] + GAP_SIZE; i++) {
            mvprintw(i, WIN_WIDTH - pipePos[ind], "  ");
        }

        for (int i = gapPos[ind] + GAP_SIZE + 1; i < SCREEN_HEIGHT - 1; i++) {
            if (i > gapPos[ind] + GAP_SIZE + 1) {
                mvprintw(i, WIN_WIDTH - pipePos[ind], "**");
            } else {
                mvprintw(i, WIN_WIDTH - pipePos[ind], "  ");
            }
        }
        attroff(COLOR_PAIR(2));
    }
}

int collision() {
    if (pipePos[0] >= 61) {
        if (bunnyPos < gapPos[0] || bunnyPos > gapPos[0] + GAP_SIZE) {
            return 1;
        }
    }
    return 0;
}


void gameover() {
	PlaySound(0, 0, 0); 
	attron(COLOR_PAIR(3));
	
    clear();
    mvprintw(5, 10, "--------------------------");
    mvprintw(6, 10, "-------- Game Over -------");
    mvprintw(7, 10, "--------------------------\n\n");
    mvprintw(9, 10, "Your score: %d\n\n", score);
    mvprintw(11, 10, "Press ENTER to go back to menu.");
    
    refresh();
    
	int ch;
    while (true) {
        ch = getch();
        if (ch == '\n' || ch == '\r') {
            break;
        }
    }
    attroff(COLOR_PAIR(3));
}


void eraseBunny() {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 5; j++) {
            mvprintw(i + bunnyPos, j + 2, " ");
        }
    }
}


void erasePipe(int ind) {
    if (pipeFlag[ind] == 1) {
        for (int i = 0; i < gapPos[ind]; i++) {
            mvprintw(i + 1, WIN_WIDTH - pipePos[ind], "  ");
        }
        for (int i = gapPos[ind] + GAP_SIZE; i < SCREEN_HEIGHT - 1; i++) {
            mvprintw(i, WIN_WIDTH - pipePos[ind], "  ");
        }
    }
}


void instructions(){
	clear();
	printw("Instructions");
    printw("\n----------------");
    printw("\n\nPress spacebar to make bunny jump");
    printw("\n\nPress ENTER to go back to menu");
	getch();
}


void setcursor(bool visible) 
{
	if(visible){
		curs_set(1);
	}else{
		curs_set(0);
	}
		
	CONSOLE_CURSOR_INFO lpCursor;	
	lpCursor.bVisible = visible;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &lpCursor);
}


HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);


void gotoxy(int x, int y)
{

  	COORD CursorPosition;
	CursorPosition.X = x;
	CursorPosition.Y = y;
	SetConsoleCursorPosition(console, CursorPosition);
}
