#include <string.h>

#include "screen.h"
#include "keyboard.h"
#include "timer.h"

#define ROWS 20
#define COLS 40

int x = 34, y = 12;
int incX = 1, incY = 1;
int score = 0;

char maze[ROWS][COLS] = {
    "#######################################",
    "#..............#.....................#",
    "#.#######.######.###########.#######.#",
    "#.#...............................#.#",
    "#.#.########.###########.########.#.#",
    "#.#........#.#...........#........#.#",
    "#.#######.#.#.###########.#.#######.#",
    "#.........#...#...........#.........#",
    "#######################################"
};

int isWin() {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            if (maze[i][j] == '.') return 0; // Ainda há bolinhas
        }
    }
    return 1; // Todas as bolinhas foram coletadas
}

void drawMaze() {
    for (int i = 0; i < ROWS; i++) {
        screenGotoxy(0, i);
        printf("%s", maze[i]);
    }
}

void printHello(int nextX, int nextY)
{
    screenSetColor(CYAN, DARKGRAY);
    screenGotoxy(x, y);
    printf("           ");
    x = nextX;
    y = nextY;
    screenGotoxy(x, y);
    printf("Hello World");
}

void printKey(int ch)
{
    screenSetColor(YELLOW, DARKGRAY);
    screenGotoxy(35, 22);
    printf("Key code :");

    screenGotoxy(34, 23);
    printf("            ");
    
    if (ch == 27) screenGotoxy(36, 23);
    else screenGotoxy(39, 23);

    printf("%d ", ch);
    while (keyhit())
    {
        printf("%d ", readch());
    }
}

void displayScore() {
    screenGotoxy(0, ROWS + 1);
    printf("Score: %d", score);
}

int main() 
{
    static int ch = 0;

    drawMaze();
    displayScore();

    while (1) {
        drawMaze();
        displayScore();
    }

    screenInit(1);
    keyboardInit();
    timerInit(50);

    printHello(x, y);
    screenUpdate();

    while (ch != 10) //enter
    {
        // Handle user input
        if (keyhit()) 
        {
            ch = readch();
            printKey(ch);
            screenUpdate();
        }

        // Update game state (move elements, verify collision, etc)
        if (timerTimeOver() == 1)
        {
            int newX = x + incX;
            if (newX >= (MAXX -strlen("Hello World") -1) || newX <= MINX+1) incX = -incX;
            int newY = y + incY;
            if (newY >= MAXY-1 || newY <= MINY+1) incY = -incY;

            printKey(ch);
            printHello(newX, newY);

            screenUpdate();
        }

        if (maze[y][x] == '.') {
            maze[y][x] = ' '; // Remove the bolinha from the maze
            score++;          // Increment the player's score
        }

        if (isWin()) {
            screenGotoxy(0, ROWS + 2);
            printf("You Win! Final Score: %d", score);
            break;
        }

    }

    keyboardDestroy();
    screenDestroy();
    timerDestroy();

    return 0;
}
