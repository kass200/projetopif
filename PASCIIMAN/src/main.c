#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "screen.h"
#include "keyboard.h"
#include "timer.h"

#define ROWS 20
#define COLS 40
#define MAX_LEVELS 5
#define MAX_GHOSTS 5

// Funções declaradas
void showGameOverScreen(int);
void showStartScreen();
void drawMaze();
void drawScore();
void initGhosts();
void resetLevel();
void loadLevel();
int isWin();
void movePacman(char);

// Variáveis globais
int x = 1, y = 1;
int score = 0;
int currentLevel = 0;
int numGhosts = 2;
int speedMultiplier = 1;

typedef struct {
    int x, y;
    int dirX, dirY;
} Ghost;

Ghost ghosts[MAX_GHOSTS];
char maze[ROWS][COLS];

// Definição dos cinco labirintos
char levels[MAX_LEVELS][ROWS][COLS] = {
    { // Nível 1 (Fácil)
        "#######################################",
        "#P.............#.....................#",
        "#.#######.######.###########.#######.#",
        "#.#...............................#.#",
        "#.#.########.###########.########.#.#",
        "#.#........#.#...........#........#.#",
        "#.#######.#.#.###########.#.#######.#",
        "#.........#...#...........#.........#",
        "#######################################"
    },
    { // Nível 2 (Moderado)
        "#######################################",
        "#P..#..............#.................#",
        "#.#.#.#######.###########.#######.#.#",
        "#.#.#......#.............#.......#.#.#",
        "#.#.########.###########.#######.#.#",
        "#.#........#.#...........#.......#.#",
        "#.#######.#.#.###########.#######.#",
        "#.........#...#...........#.......#",
        "#######################################"
    },
    { // Nível 3 (Desafiador)
        "#######################################",
        "#P.....#.............................#",
        "#.#####.######.###########.#######.#.#",
        "#.#...............................#.#",
        "#.#.########.#######.#######.#####.#",
        "#.#........#.#.....#...#..........#.#",
        "#.#######.#.#.###.#####.#######.###.#",
        "#.........#...#...........#.......#.#",
        "#######################################"
    },
    { // Nível 4 (Difícil)
        "#######################################",
        "#P.............................#.....#",
        "#.#######.######.###########.#######.#",
        "#.#......#.............#...........#.#",
        "#.#.#############.###############.#.#",
        "#.#....#..........#...............#.#",
        "#.###########.#.#######.#########.#.#",
        "#.............#.....#.............#..#",
        "#######################################"
    },
    { // Nível 5 (Muito Difícil)
        "#######################################",
        "#P.....##########...##########........#",
        "#.######....#............#....######.#",
        "#.#######.##.###########.###.#######.#",
        "#.....#...#.................#........#",
        "#.###.#.#############.####.###########",
        "#.#.....#.............#..............#",
        "#.#######.######################.###.#",
        "#######################################"
    }
};

// Função para reinicializar o nível
void resetLevel() {
    x = 1; y = 1;
    score = 0;
    loadLevel();
    initGhosts();
    drawMaze();
    drawScore();
}

// Inicializa os fantasmas
void initGhosts() {
    srand(time(0));
    for (int i = 0; i < numGhosts; i++) {
        int posX, posY;
        do {
            posX = rand() % (COLS - 2) + 1;
            posY = rand() % (ROWS - 2) + 1;
        } while (maze[posY][posX] == '#');
        ghosts[i] = (Ghost){posX, posY, rand() % 2 ? 1 : -1, rand() % 2 ? 1 : -1};
    }
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

void drawScore() {
    screenGotoxy(0, ROWS);
    printf("Pontuação: %d   ", score);
}

void drawMaze() {
    for (int i = 0; i < ROWS; i++) {
        screenGotoxy(0, i);
        printf("%s", maze[i]);
    }
    drawScore();
}

void showStartScreen() {
    screenClear();
    screenGotoxy(10, 5);
    printf("===== PASCIIMAN =====");
    screenGotoxy(10, 7);
    printf("Pressione qualquer tecla para começar...");
    while (!keyhit());
    readch();
}

void showGameOverScreen(int won) {
    screenClear();
    if (won) {
        printf("Parabéns! Você venceu todos os níveis!\n");
    } else {
        printf("Game Over! Você foi pego por um fantasma!\n");
    }
    printf("Pontuação final: %d\n", score);
    printf("Pressione 'R' para reiniciar ou qualquer outra tecla para sair...");
    while (!keyhit());
    char choice = readch();
    if (choice == 'r' || choice == 'R') {
        resetLevel();
    } else {
        exit(0);
    }
}

void movePacman(char direction) {
    int newX = x, newY = y;

    switch (direction) {
        case 'w': newY--; break;
        case 's': newY++; break;
        case 'a': newX--; break;
        case 'd': newX++; break;
        default: return;
    }

    if (newX >= 0 && newX < COLS && newY >= 0 && newY < ROWS && maze[newY][newX] != '#') {
        screenGotoxy(x, y);
        printf(" ");
        x = newX;
        y = newY;
        screenGotoxy(x, y);
        printf("P");

        if (maze[newY][newX] == '.') {
            score += speedMultiplier;
            maze[newY][newX] = ' ';
            drawScore();
        }
    }
}

int main() {
    int ch = 0;
    screenInit(1);
    keyboardInit();

    showStartScreen();
    loadLevel();
    initGhosts();
    drawMaze();

    while (1) {
        if (keyhit()) {
            ch = readch();
            movePacman(ch);
        }

        if (isWin()) {
            showGameOverScreen(1);
            break;
        }

        screenUpdate();
    }

    showGameOverScreen(0);
    keyboardDestroy();
    screenDestroy();
    return 0;
}
