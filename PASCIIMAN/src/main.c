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
void drawGhosts();
void moveGhosts();
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
int ghostMoveCounter = 0;
typedef struct {
    int x, y;
    int dirX, dirY;
} Ghost;

Ghost ghosts[MAX_GHOSTS];
char maze[ROWS][COLS];

// Labirintos
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
    
};

// Função para calcular deslocamentos
void calculateOffsets() {
    offsetX = (SCREEN_WIDTH - COLS) / 2;
    offsetY = (SCREEN_HEIGHT - ROWS) / 2;
}

// Inicializa os fantasmas
void initGhosts() {
    srand(time(0));
    for (int i = 0; i < numGhosts; i++) {
        int posX, posY;
        do {
            posX = rand() % (COLS - 2) + 1;
            posY = rand() % (ROWS - 2) + 1;
        } while (maze[posY][posX] == '#' || maze[posY][posX] == 'P');
        ghosts[i] = (Ghost){posX, posY, rand() % 2 ? 1 : -1, rand() % 2 ? 1 : -1};
    }
}

// Desenha os fantasmas na tela
void drawGhosts() {
    for (int i = 0; i < numGhosts; i++) {
        screenGotoxy(offsetX + ghosts[i].x, offsetY + ghosts[i].y);
        printf("G");
    }
}

void moveGhosts() {
    ghostMoveCounter++;
    if (ghostMoveCounter % 5 != 0) return; // Reduz a frequência de movimento dos fantasmas

    for (int i = 0; i < numGhosts; i++) {
        int newX = ghosts[i].x + ghosts[i].dirX;
        int newY = ghosts[i].y + ghosts[i].dirY;

        // Verificar se a nova posição é válida (não é uma parede e está dentro dos limites)
        if (newX >= 0 && newX < COLS && newY >= 0 && newY < ROWS && maze[newY][newX] != '#') {
            // Apagar a posição antiga
            screenGotoxy(offsetX + ghosts[i].x, offsetY + ghosts[i].y);
            printf(" ");

            // Atualizar a posição
            ghosts[i].x = newX;
            ghosts[i].y = newY;

            // Verificar colisão com Pac-Man
            if (ghosts[i].x == x && ghosts[i].y == y) {
                showGameOverScreen(0);
            }
        } else {
            // Se a direção está bloqueada, escolher uma nova direção aleatória válida
            int directions[4][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}}; // Direções possíveis (direita, esquerda, baixo, cima)
            int validDirectionFound = 0;

            for (int attempt = 0; attempt < 4; attempt++) {
                int randIndex = rand() % 4; // Escolher uma direção aleatória
                int testX = ghosts[i].x + directions[randIndex][0];
                int testY = ghosts[i].y + directions[randIndex][1];

                if (testX >= 0 && testX < COLS && testY >= 0 && testY < ROWS && maze[testY][testX] != '#') {
                    ghosts[i].dirX = directions[randIndex][0];
                    ghosts[i].dirY = directions[randIndex][1];
                    validDirectionFound = 1;
                    break;
                }
            }

            // Se nenhuma direção válida foi encontrada, inverter a direção atual
            if (!validDirectionFound) {
                ghosts[i].dirX = -ghosts[i].dirX;
                ghosts[i].dirY = -ghosts[i].dirY;
            }
        }
    }

    // Redesenhar os fantasmas em suas novas posições
    drawGhosts();
}



// Função para reiniciar o nível
void resetLevel() {
    x = 1; y = 1;
    score = 0;
    loadLevel();
    initGhosts();
    drawMaze();
    drawGhosts();
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

// Desenha o labirinto
void drawMaze() {
    for (int i = 0; i < ROWS; i++) {
        screenGotoxy(offsetX, offsetY + i);
        printf("%s", maze[i]);
    }
    drawScore();
    drawGhosts();
}

// Tela inicial
void showStartScreen() {
    screenClear();
    screenGotoxy(SCREEN_WIDTH / 2 - 10, SCREEN_HEIGHT / 2 - 2);
    printf("===== PASCIIMAN =====");
    screenGotoxy(SCREEN_WIDTH / 2 - 10, SCREEN_HEIGHT / 2);
    printf("Pressione qualquer tecla para começar...");
    fflush(stdout);
    while (!keyhit());
    readch();
    screenClear();
    screenUpdate();
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
    fflush(stdout);
    while (!keyhit());
    char choice = readch();
    if (choice == 'r' || choice == 'R') {
        resetLevel();
        screenClear();
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

    if (newX >= 0 && newX < COLS && newY >= 0 && newY < ROWS && maze[newY][newX] != '#') {
        screenGotoxy(offsetX + x, offsetY + y);
        printf(" ");
        x = newX;
        y = newY;
        screenGotoxy(offsetX + x, offsetY + y);
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

    // Inicializar bibliotecas
    screenInit(1);
    keyboardInit();
    calculateOffsets();

    // Exibir tela inicial
    showStartScreen();

    // Carregar o primeiro nível
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
            showGameOverScreen(1);
            break;
        }

        screenUpdate();
    }

    // Tela de fim de jogo
    showGameOverScreen(0);

    // Limpar recursos
    keyboardDestroy();
    screenDestroy();

    return 0;
}
