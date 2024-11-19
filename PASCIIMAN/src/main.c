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
int numGhosts = 4;
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

// Labirinto
char levels[MAX_LEVELS][ROWS][COLS] = {
    { 
      "___________________________________",
     "|###################################|",
     "|P.............#....................|",
     "|.#######.######.###########.######.|",
     "|.#...............................#.|",
     "|.#.########.###########.########.#.|",
     "|.#........#.#...........#........#.|",
     "|.#######...#..#########..#.#######.|",
     "|............#.....##....#..........|",
     "|..##............######..#......###.|",
     "|#.#####.....#.....##..#......#..###|",
     "|#.####..#####.........########.###.|",
     "|..........########....#..##......##|",
     "|.#.#......#.............#.......#.#|",
     "|.#.##..####.#########.#.#######.#.#|",
     "|.#........#.#...........#.......#.#|",
     "|___________________________________|"
    }
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
        } while (maze[posY][posX] != '.'); // Garantir que os fantasmas comecem apenas em posições válidas
        ghosts[i] = (Ghost){posX, posY, 0, 0}; // Inicializar com direção nula
    }
}

// Desenha os fantasmas na tela
void drawGhosts() {
    for (int i = 0; i < numGhosts; i++) {
        screenGotoxy(offsetX + ghosts[i].x, offsetY + ghosts[i].y);
        printf("\033[1;31mG\033[0m"); // ANSI para texto vermelho brilhante
    }
}

void moveGhosts() {
    static clock_t lastMoveTime = 0;
    clock_t currentTime = clock();

    // Atualizar fantasmas a cada 500ms
    if (((currentTime - lastMoveTime) / CLOCKS_PER_SEC) < 0.5) return;
    lastMoveTime = currentTime;

    for (int i = 0; i < numGhosts; i++) {
        int newX = ghosts[i].x + ghosts[i].dirX;
        int newY = ghosts[i].y + ghosts[i].dirY;

        // Verificar se a nova posição é válida
        if (newX >= 0 && newX < COLS && newY >= 0 && newY < ROWS && maze[newY][newX] == '.') {
            // Apagar posição antiga
            screenGotoxy(offsetX + ghosts[i].x, offsetY + ghosts[i].y);
            printf(".");

            // Atualizar posição
            ghosts[i].x = newX;
            ghosts[i].y = newY;

            // Colisão com Pac-Man
            if (ghosts[i].x == x && ghosts[i].y == y) {
                showGameOverScreen(0);
            }
        } else {
            // Selecionar nova direção
            int directions[4][2] = {{0, 1}, {0, -1}, {1, 0}, {-1, 0}};
            int validDir = 0;
            for (int d = 0; d < 4; d++) {
                int testX = ghosts[i].x + directions[d][0];
                int testY = ghosts[i].y + directions[d][1];
                if (testX >= 0 && testX < COLS && testY >= 0 && testY < ROWS && maze[testY][testX] == '.') {
                    ghosts[i].dirX = directions[d][0];
                    ghosts[i].dirY = directions[d][1];
                    validDir = 1;
                    break;
                }
            }
            if (!validDir) {
                // Inverter direção se nenhuma direção válida for encontrada
                ghosts[i].dirX *= -1;
                ghosts[i].dirY *= -1;
            }
        }
    }
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
    screenGotoxy(offsetX, offsetY + ROWS + 1); // Posição abaixo do labirinto
    printf("Pontuação: %d   ", score);        // Exibe a pontuação
    fflush(stdout);                          // Garante a atualização imediata
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
    drawGhosts();
}
// Tela inicial
void showStartScreen() {
    screenClear();
    screenGotoxy(SCREEN_WIDTH / 2 - 10, SCREEN_HEIGHT / 2 - 2);
    printf("\033[1;34m===== PASCIIMAN =====\033[0m"); // Azul brilhante
    screenGotoxy(SCREEN_WIDTH / 2 - 10, SCREEN_HEIGHT / 2);
    printf("\033[1;32mPressione qualquer tecla para começar...\033[0m"); // Verde brilhante
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
        screenClear();
        resetLevel();
    } else {
        exit(0);
    }

}

// Incremento de Pontuação
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
        printf("\033[1;33mP\033[0m");

        if (maze[newY][newX] == '.') {
            score += 1;
            maze[newY][newX] = ' ';
            drawScore();
        }
    }
}

void respawnPontos() {
    static clock_t lastRespawnTime = 0;
    clock_t currentTime = clock();

    // Reaparecer bolinhas a cada 3 segundos
    if (((currentTime - lastRespawnTime) / CLOCKS_PER_SEC) < 3) return;
    lastRespawnTime = currentTime;

    // Escolher uma posição aleatória válida para adicionar uma bolinha
    int posX, posY;
    do {
        posX = rand() % (COLS - 2) + 1;
        posY = rand() % (ROWS - 2) + 1;
    } while (maze[posY][posX] != ' '); // Certificar-se de que a posição está vazia

    maze[posY][posX] = '.'; // Adicionar uma bolinha
    screenGotoxy(offsetX + posX, offsetY + posY);
    printf("\033[1;34m.\033[0m"); // Azul brilhante
}

int main() {
    int ch = 0;

    // Inicializar bibliotecas
    screenInit(1);
    keyboardInit();
    calculateOffsets();

    // Exibir tela inicial
    showStartScreen();

    // Carrega o labirinto
    loadLevel();
    initGhosts();
    drawMaze();

   while (1) {
     if (keyhit()) {
         ch = readch();
         movePacman(ch);
        }

      moveGhosts();
      respawnPontos();

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
