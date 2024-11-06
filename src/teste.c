#include <stdio.h>
#include <conio.h> // Para Windows; para Linux, substitua por <termios.h> e ajuste o código

#define WIDTH 10
#define HEIGHT 10

typedef struct {
    int x;
    int y;
} Player;

void initializeMaze(int maze[HEIGHT][WIDTH]) {
    // Exemplo simples de labirinto (0 = espaço vazio, 1 = parede)
    int temp[HEIGHT][WIDTH] = {
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 0, 0, 0, 1, 0, 0, 0, 0, 1},
        {1, 0, 1, 0, 1, 0, 1, 1, 0, 1},
        {1, 0, 1, 0, 0, 0, 0, 1, 0, 1},
        {1, 0, 1, 1, 1, 1, 0, 1, 0, 1},
        {1, 0, 0, 0, 0, 1, 0, 1, 0, 1},
        {1, 1, 1, 1, 0, 1, 0, 1, 0, 1},
        {1, 0, 0, 1, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 1, 1, 1, 0, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    };

    // Copia o labirinto temporário para o labirinto principal
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            maze[i][j] = temp[i][j];
        }
    }
}

void printMaze(int maze[HEIGHT][WIDTH], Player player) {
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            if (i == player.y && j == player.x)
                printf("P "); // Posiciona o jogador
            else if (maze[i][j] == 1)
                printf("# "); // Parede
            else
                printf(". "); // Espaço vazio
        }
        printf("\n");
    }
}

void movePlayer(int maze[HEIGHT][WIDTH], Player *player, char direction) {
    int newX = player->x;
    int newY = player->y;

    switch (direction) {
        case 'w': newY--; break; // Cima
        case 's': newY++; break; // Baixo
        case 'a': newX--; break; // Esquerda
        case 'd': newX++; break; // Direita
        default: return;
    }

    // Verifica se a nova posição é um espaço vazio
    if (maze[newY][newX] == 0) {
        player->x = newX;
        player->y = newY;
    }
}

int main() {
    int maze[HEIGHT][WIDTH];
    Player player = {1, 1}; // Posição inicial do jogador

    initializeMaze(maze);

    char input;
    while (1) {
        system("cls"); // Limpa a tela no Windows; para Linux, use "clear"
        printMaze(maze, player);

        printf("Use W, A, S, D para mover o jogador (P). Pressione Q para sair.\n");
        input = _getch(); // Captura a tecla pressionada

        if (input == 'q') break;

        movePlayer(maze, &player, input);
    }

    return 0;
}
