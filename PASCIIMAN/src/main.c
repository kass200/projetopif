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
#define SCREEN_WIDTH 80  // Largura padrão do terminal
#define SCREEN_HEIGHT 24 // Altura padrão do terminal

// Declarações de funções
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
int offsetX = 0; // Deslocamento horizontal para centralizar o labirinto
int offsetY = 0; // Deslocamento vertical para centralizar o labirinto

typedef struct {
    int x, y;
    int dirX, dirY;
} Ghost;

Ghost ghosts[MAX_GHOSTS];
char maze[ROWS][COLS];

// Definição dos cinco labirintos
char levels[MAX_LEVELS][ROWS][COLS] = {
    { // Nível 1 (Fácil)
      "___________________________________",
     "|###################################|",
     "|P.............#....................|",
     "|.#######.######.###########.######.|",
     "|.#...............................#.|",
     "|.#.########.###########.########.#.|",
     "|.#........#.#...........#........#.|",
     "|.#######...#..#########..#.#######.|",
     "|............#...........#..........|",
     "|___________________________________|",
    },
    { // Nível 2 (Moderado)
      "_____________________________________",
     "|#####################################|",
     "|P..#..............#..................|",
     "|.#.#.#######.###########.#######.#...|",
     "|.#.#......#.............#.......#.#.#|",
     "|.#.########.###########.#######.#.#..|",
     "|.#........#.#...........#.......#.#.#|",
     "|.#######.#.#.###########.#######.#...|",
     "|.........#...#...........#...........|",
     "|_____________________________________|"
    },
    { // Nível 3 (Desafiador)
        " _____________________________________",
        "|#####################################|",
        "|P.....#.............................#|",
        "|.#####.######.###########.#######....|",
        "|.#...............................#.#.|",
        "|.#.########.#######.#######.#####....|",
        "|.#........#.#.....#...#..........#.##|",
        "|.#######.#.#.###.#####.#######.###...|",
        "|.........#...#...........#.......#.##|",
        "|_____________________________________|"
    },
    { // Nível 4 (Difícil)
        " _____________________________________",
        "|#####################################|",
        "|P.............................#.....#|",
        "|.#######.######.###########.#######.#|",
        "|.#......#.............#...........#.#|",
        "|.#.#############.###############.#.##|",
        "|.#....#..........#...............#.#.|",
        "|.###########.#.#######.#########.#...|",
        "|.............#.....#.............#...|",
        "|_____________________________________|"
    },
    { // Nível 5 (Muito Difícil)
        " ____________________________________",
        "|####################################|",
        "|P.....##########...##########.......|",
        "|.######....#............#....######.|",
        "|.#######.##.###########.###.########|",
        "|.....#...#.................#........|",
        "|.###.#.#############.####.##########|",
        "|.#.....#.............#..............|",
        "|.#######.######################.###.|",
        "|____________________________________|"
    }
};

// Função para calcular deslocamentos
void calculateOffsets() {
    offsetX = (SCREEN_WIDTH - COLS) / 2;  // Centraliza horizontalmente
    offsetY = (SCREEN_HEIGHT - ROWS) / 2; // Centraliza verticalmente
}

// Inicializa os fantasmas
void initGhosts() {
    srand(time(0));
    for (int i = 0; i < numGhosts; i++) {
        int posX, posY;
        do {
            posX = rand() % (COLS - 2) + 1;
            posY = rand() % (ROWS - 2) + 1;
        } while (maze[posY][posX] == '#' || maze[posY][posX] == '|' || maze[posY][posX] == '_');
        ghosts[i] = (Ghost){posX, posY, rand() % 2 ? 1 : -1, rand() % 2 ? 1 : -1};

        // Desenhar o fantasma com cor vermelha
        screenGotoxy(offsetX + posX, offsetY + posY);
        printf("\033[1;31mG\033[0m"); // ANSI para texto vermelho brilhante
    }
}


// Carrega o nível atual
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

// Desenha o placar
void drawScore() {
    screenGotoxy(offsetX, offsetY + ROWS + 1);
    printf("Pontuação: %d   ", score);
}
// Desenha o labirinto na tela
void drawMaze() {
    for (int i = 0; i < ROWS; i++) {
        screenGotoxy(offsetX, offsetY + i);
        for (int j = 0; j < COLS; j++) {
            if (maze[i][j] == '#') {
                printf("\033[1;34m#\033[0m"); // Azul brilhante para paredes
            } else if (maze[i][j] == '.') {
                printf("\033[1;34m.\033[0m"); // Azul brilhante para pontos
            } else if (maze[i][j] == 'P') {
                printf("\033[1;33mP\033[0m"); // Amarelo brilhante para Pac-Man
            } else if (maze[i][j] == '_') {
                printf("\033[1;34m_\033[0m"); // Azul brilhante para o limite inferior
            } else if (maze[i][j] == '|') {
                printf("\033[1;34m|\033[0m"); // Azul brilhante para bordas verticais
            } else {
                printf(" "); // Espaço vazio
            }
        }
    }
    drawScore();
}
// Tela inicial
void showStartScreen() {
    screenClear();
    screenGotoxy(SCREEN_WIDTH / 2 - 10, SCREEN_HEIGHT / 2 - 2);
    printf("\033[1;34m===== PASCIIMAN =====\033[0m"); // Azul brilhante
    screenGotoxy(SCREEN_WIDTH / 2 - 10, SCREEN_HEIGHT / 2);
    printf("\033[1;32mPressione qualquer tecla para começar...\033[0m"); // Verde brilhante
    while (!keyhit());
    readch();
}

// Função para reiniciar o nível
void resetLevel() {
    x = 1; y = 1;
    score = 0;
    loadLevel();
    initGhosts();
    drawMaze();
}

// Tela de Game Over
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

// Movimenta o Pac-Man
void movePacman(char direction) {
    int newX = x, newY = y;

    switch (direction) {
        case 'w': newY--; break;
        case 's': newY++; break;
        case 'a': newX--; break;
        case 'd': newX++; break;
        default: return;
    }

    // Verifica se o novo movimento está dentro dos limites do labirinto
    if (newX >= 0 && newX < COLS && newY >= 0 && newY < ROWS) {
        // Verifica se a nova posição não é uma parede (nem #, nem |, nem _)
        if (maze[newY][newX] != '#' && maze[newY][newX] != '|' && maze[newY][newX] != '_') {
            if (maze[newY][newX] == '.') {
                score++;  // Coleta o ponto
                maze[newY][newX] = ' ';  // Apaga o ponto do labirinto
            }
            // Atualiza a posição do Pac-Man
            x = newX;
            y = newY;

            // Desenha o Pac-Man na nova posição
            screenGotoxy(offsetX + x, offsetY + y);
            printf("\033[1;33mP\033[0m");
        if (maze[newY][newX] == '.') {
            score += speedMultiplier;
            maze[newY][newX] = ' ';
            drawScore();
        }
    }
}}

int main() {
    int ch = 0;
    screenInit(1);
    keyboardInit();
    calculateOffsets();

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
