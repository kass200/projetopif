#include <string.h>
#include <stdlib.h>
#include "screen.h"
#include "keyboard.h"
#include "timer.h"

#define ROWS 20
#define COLS 40
#define MAX_LEVELS 5
#define MAX_GHOSTS 5

// Declarações de funções para evitar implicit declaration           // Declaração para a função initGhosts
void showGameOverScreen(int); // Declaração para showGameOverScreen
void drawMaze();              // Declaração para drawMaze

int x = 1, y = 1; // Posição inicial do Pacman
int score = 0;
int currentLevel = 0;
int numGhosts = 2;
int speedMultiplier = 1; // Multiplicador de velocidade

typedef struct {
    int x, y;
    int dirX, dirY;
} Ghost;

Ghost ghosts[MAX_GHOSTS];

char levels[MAX_LEVELS][ROWS][COLS] = {
    { "#######################################", 
      "#P.............#.....................#", 
      "#.#######.######.###########.#######.#",
      "#.#...............................#.#", 
      "#.#.########.###########.########.#.#", 
      "#.#........#.#...........#........#.#",
      "#.#######.#.#.###########.#.#######.#", 
      "#.........#...#...........#.........#", 
      "#######################################" 
    },

    { "#######################################", 
      "#P..#..............#.................#", 
      "#.#.#.#######.###########.#######.#.#",
      "#.#.#...........................#.#.#", 
      "#.#.########.###########.#######.#.#", 
      "#.#........#.#...........#.......#.#",
      "#.#######.#.#.###########.#######.#", 
      "#.........#...#...........#.......#", 
      "#######################################" },

    { "#######################################", 
      "#P.....#.............................#", 
      "#.#####.######.###########.#######.#.#",
      "#.#...............................#.#", 
      "#.#.########.#######.#######.#####.#", 
      "#.#........#.#.........#..........#.#",
      "#.#######.#.#.###########.#######.#", 
      "#.........#...#...........#.......#", 
      "#######################################"
    },

    { "#######################################", 
      "#P...................................#", 
      "#.#######.######.###########.#######.#",
      "#.#.............#.................#.#", 
      "#.#.############.###############.#.#", 
      "#.#..............#...............#.#",
      "#.###########.#.###############.#.#", 
      "#.............#.................#..#", 
      "#######################################" 
    },

    { "#######################################", 
      "#P.....##########...##########........#", 
      "#.######.............#........######.#",
      "#.#######.###########.###########.###", 
      "#........#.................#........#", 
      "#.#######.###############.###########",
      "#.......................#............", 
      "#######################################" 
    }
};

char maze[ROWS][COLS];

void initGhosts() {
    for (int i = 0; i < numGhosts; i++) {
        ghosts[i] = (Ghost){5 + i, 5, 1, 0};
    }
}

void resetLevel() {
    x = 1; y = 1;
    initGhosts();
}

void loadLevel() {
    memcpy(maze, levels[currentLevel], sizeof(levels[currentLevel]));
    numGhosts = 2 + currentLevel;
    speedMultiplier = currentLevel + 1;
    timerInit(50 / speedMultiplier);
}

int isWin() {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            if (maze[i][j] == '.') return 0;
        }
    }
    return 1;
}

void moveGhosts() {
    for (int i = 0; i < numGhosts; i++) {
        int newX = ghosts[i].x + ghosts[i].dirX;
        int newY = ghosts[i].y + ghosts[i].dirY;

        if (maze[newY][newX] == '#') {
            ghosts[i].dirX = -ghosts[i].dirX;
            ghosts[i].dirY = -ghosts[i].dirY;
        } else {
            screenGotoxy(ghosts[i].x, ghosts[i].y);
            printf(" ");
            
            ghosts[i].x = newX;
            ghosts[i].y = newY;

            screenGotoxy(ghosts[i].x, ghosts[i].y);
            printf("G");
        }

        if (ghosts[i].x == x && ghosts[i].y == y) {
            showGameOverScreen(0);
        }
    }
}

void movePacman(char direction) {
    int newX = x, newY = y;

    switch (direction) {
        case 'w': newX--; break;
        case 's': newX++; break;
        case 'a': newY--; break;
        case 'd': newY++; break;
        default: return;
    }

    if (maze[newY][newX] != '#') {
        screenGotoxy(x, y);
        printf(" ");

        x = newX;
        y = newY;

        screenGotoxy(x, y);
        printf("P");

        if (maze[newY][newX] == '.'){
            score += speedMultiplier;
            maze[newY][newX] = ' ';
            screenGotoxy(0, ROWS);
            printf("Pontuacao: %d", score);
        }
    }
}

void showLevelTransition() {
    screenClear();
    screenGotoxy(10, 5);
    printf("Nível %d Concluído!", currentLevel);
    screenGotoxy(10, 7);
    printf("Prepare-se para o próximo nível...");
    getchar();
}

void showGameOverScreen(int isWin) {
    screenClear();
    if (isWin) {
        printf("\n\n");
        printf("******************************************\n");
        printf("*              PARABÉNS!                *\n");
        printf("*       Você venceu todos os níveis!    *\n");
        printf("******************************************\n\n");
    } else {
        printf("\n\n");
        printf("******************************************\n");
        printf("*               GAME OVER               *\n");
        printf("*      Você foi pego por um fantasma!   *\n");
        printf("******************************************\n\n");
    }
    printf("Pressione qualquer tecla para sair...");
    getchar(); // Espera o usuário pressionar uma tecla
}

void drawMaze() {
    // Desenha o labirinto
    for (int i = 0; i < ROWS; i++) {
        screenGotoxy(0, i);
        printf("%s", maze[i]);
    }

    screenGotoxy(0, ROWS);
    printf("Pontuacao: %d", score);
}


int main() {
    int ch = 0;
    screenInit(1);
    keyboardInit();

    loadLevel();
    initGhosts();
    drawMaze();

    while (1) {

        if (keyhit()) {
            ch = readch();
            movePacman(ch);
        }
        moveGhosts();

        if (isWin()) {
            showLevelTransition();
            currentLevel++;
            if (currentLevel >= MAX_LEVELS) {
                showGameOverScreen(1);
                break;
            } else {
                loadLevel();
                initGhosts();
            }
        }

        screenUpdate();
    }

    keyboardDestroy();
    screenDestroy();
    return 0;
}