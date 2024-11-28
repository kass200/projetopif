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
int x = 1, y = 2;
int score = 0;
int currentLevel = 0;
int numGhosts = 4;
int speedMultiplier = 1;
int offsetX = 0; // Deslocamento horizontal para centralizar o labirinto
int offsetY = 0; // Deslocamento vertical para centralizar o labirinto
int ghostMoveCounter = 0;
int highScore = 0;
typedef struct {
    int x, y;
    int dirX, dirY;
} Ghost;

Ghost ghosts[MAX_GHOSTS];
char maze[ROWS][COLS];

// Labirinto
char levels[MAX_LEVELS][ROWS][COLS] = {
    { 
     "_____________________________________",
     "|###################################|",
     "|O.............#....................|",
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
        } while (maze[posY][posX] == '#' || maze[posY][posX] == '|' || maze[posY][posX] == '_');
        ghosts[i] = (Ghost){posX, posY, rand() % 2 ? 1 : -1, rand() % 2 ? 1 : -1};

        drawGhosts();
    }
}

// Desenha os fantasmas na tela
void drawGhosts() {
    for (int i = 0; i < numGhosts; i++) {
        screenGotoxy(offsetX + ghosts[i].x, offsetY + ghosts[i].y);
        printf("\033[1;31m👻\033[0m"); // ANSI para texto vermelho brilhante
    }
}

void moveGhosts() {
    ghostMoveCounter++;
    if (ghostMoveCounter % 170000 != 0) return; // Reduz a frequência de movimento dos fantasmas

    for (int i = 0; i < numGhosts; i++) {
        int newX = ghosts[i].x + ghosts[i].dirX;
        int newY = ghosts[i].y + ghosts[i].dirY;

        // Verificar se a nova posição é válida (não é uma parede e está dentro dos limites)
        if (newX >= 0 && newX < COLS && newY >= 0 && newY < ROWS && maze[newY][newX] != '#' && maze[newY][newX] != '_' && maze[newY][newX] != '|') {
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

// Desenha o placar
void drawScore() {
    screenGotoxy(offsetX, offsetY + ROWS + 1);
    printf("Pontuação: %d   ", score);
    if (score > 0 && score % 10 == 0) {
        screenGotoxy(offsetX, offsetY + ROWS + 2);
        printf("Bom trabalho! Continue assim!");
    } else {
        screenGotoxy(offsetX, offsetY + ROWS + 2);
        printf("                             "); // Apaga mensagem anterior
    }
    fflush(stdout);
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
            } else if (maze[i][j] == 'O') {
                printf("\033[1;33m●\033[0m"); // Amarelo brilhante para Pac-Man
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

void showStartScreen() {
    screenClear(); 
    printf("\n\n");
    printf("  #############################################\n");
    printf("  #                                           #\n");
    printf("  #              PASCIIMAN GAME               #\n");
    printf("  #                                           #\n");
    printf("  #          Pressione qualquer tecla         #\n");
    printf("  #               para começar!               #\n");
    printf("  #############################################\n");
    printf("\n");
    printf("  Bem-vindo ao Pasciiman! Aqui estão as instruções:\n\n");
    printf("  Movimentação:\n");
    printf("    - Use as teclas W, A, S, D para mover o Pac-Man:\n");
    printf("        W: Cima\n");
    printf("        A: Esquerda\n");
    printf("        S: Baixo\n");
    printf("        D: Direita\n");
    printf("\n");
    printf("  Regras:\n");
    printf("    - Colete o máximo de pontos (.) Possíveis.\n");
    printf("    - Evite os fantasmas (G), ou será derrotado.\n");
    printf("    - Pontos reaparecem periodicamente.\n");
    printf("    - Use a estratégia para não ser pego!\n");

    while (!keyhit()) { } 
    readch(); 
    screenClear(); 
}


// Tela de Game Over
void showGameOverScreen(int won) {
    screenClear();
    if (won) {
        printf("Parabéns! Você venceu!\n");
    } else {
        printf("Game Over! Você foi pego por um fantasma!\n");
    }
    if (score > highScore) highScore = score;
    printf("Pontuação: %d   Maior Pontuação: %d\n", score, highScore);
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
     // Verifica se o novo movimento está dentro dos limites do labirinto
    if (newX >= 0 && newX < COLS && newY >= 0 && newY < ROWS) {
        // Verifica se a nova posição não é uma parede (nem #, nem |, nem )
        if (maze[newY][newX] != '#' && maze[newY][newX] != '|' && maze[newY][newX] != '_') {
            // Apaga a posição anterior do Pac-Man
            screenGotoxy(offsetX + x, offsetY + y);
            printf(" ");  // Deixa a posição em branco

            // Atualiza a posição do Pac-Man
            x = newX;
            y = newY;

            // Desenha o Pac-Man na nova posição
            screenGotoxy(offsetX + x, offsetY + y);
            printf("\033[1;33m●\033[0m");

            if (maze[newY][newX] == '.') {
                score += 1;
                maze[newY][newX] = ' ';
            }

            drawScore();
        }

    }
}

void respawnPontos() {
    static clock_t lastRespawnTime = 0;
    clock_t currentTime = clock();
    if (((currentTime - lastRespawnTime) / CLOCKS_PER_SEC) < 3) return;
    lastRespawnTime = currentTime;

    int posX, posY, attempts = 0;
    int isNearGhost = 0;
    
    do {
        posX = rand() % (COLS - 2) + 1;
        posY = rand() % (ROWS - 2) + 1;

        for (int i = 0; i < numGhosts; i++) {
            if (ghosts[i].x == posX && ghosts[i].y == posY) {
                isNearGhost = 1;
                break;
            }
        }
        attempts++;
        if (attempts > 100) break; // Evita loop infinito
    } while (maze[posY][posX] != ' ' || (abs(posX - x) < 3 && abs(posY - y) < 3) || isNearGhost);

    if (maze[posY][posX] == ' ') {
        maze[posY][posX] = '.';
        screenGotoxy(offsetX + posX, offsetY + posY);
        printf("\033[1;34m.\033[0m");
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
    
    // Carrega o labirinto
    loadLevel();
    initGhosts();
    drawMaze();

    while (1) {
        // Verifica entrada do teclado
        if (keyhit()) {
            ch = readch();
            movePacman(ch); // Move Pac-Man
        }

        // Atualiza fantasmas
        moveGhosts();

        // Respawna pontos após um intervalo
        respawnPontos();

        // Atualiza a tela
        screenUpdate();
    }
}