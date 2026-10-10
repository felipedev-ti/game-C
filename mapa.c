#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <math.h>

#define LARGURA 22
#define ALTURA 12
#define QTD_MONSTROS 4

// Estrutura do Jogador
typedef struct {
    int x;
    int y;
    int hp;
    int moedas;
} Jogador;

// Estrutura dos Monstros (Fantasmas)
typedef struct {
    int x;
    int y;
    int ativo; // 1 = Vivo, 0 = Derrotado
} Monstro;

// Mapa Expandido: 1 = Parede (#), 2 = Moeda ($), 0 = Chao (.)
int mapa[ALTURA][LARGURA] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,2,0,0,1,0,2,0,0,0,0,1,0,0,0,2,0,0,2,0,1},
    {1,0,1,1,0,1,0,1,1,1,1,0,1,0,1,1,1,1,1,1,0,1},
    {1,0,1,0,0,0,0,0,2,0,1,0,0,0,0,0,0,2,0,1,0,1},
    {1,0,1,0,1,1,1,1,1,0,1,1,1,1,1,0,1,1,0,1,0,1},
    {1,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,1},
    {1,0,1,0,1,0,1,1,1,0,1,1,1,0,1,0,1,0,1,1,0,1},
    {1,0,1,0,0,0,1,2,0,0,0,2,1,0,0,0,0,0,0,1,0,1},
    {1,0,1,1,1,0,1,0,1,1,1,0,1,0,1,1,1,1,0,1,0,1},
    {1,0,2,0,0,0,0,0,0,2,0,0,0,0,0,0,2,0,0,0,2,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

int totalMoedasNoMapa = 11;

// 1. FUNÇÃO: Renderiza o mapa e todas as entidades dinâmicas
void desenharJogo(Jogador p, Monstro monstros[]) {
    system("cls");
    
    printf("===================================================\n");
    printf("           PAC-CRAWLER: PAC-MAN EM C               \n");
    printf("===================================================\n");
    printf("Controles: [W] Cima | [S] Baixo | [A] Esquerda | [D] Direita | [Q] Sair\n");
    printf("HP: %d/100  |  Moedas: %d/%d\n\n", p.hp, p.moedas, totalMoedasNoMapa);

    int i, j, k;
    for (i = 0; i < ALTURA; i++) {
        for (j = 0; j < LARGURA; j++) {
            // Desenha o Herói
            if (i == p.y && j == p.x) {
                printf("@ ");
                continue;
            }

            // Checa se há algum monstro nesta posição
            int ehMonstro = 0;
            for (k = 0; k < QTD_MONSTROS; k++) {
                if (monstros[k].ativo && monstros[k].x == j && monstros[k].y == i) {
                    printf("M ");
                    ehMonstro = 1;
                    break;
                }
            }

            if (ehMonstro) continue;

            // Desenha elementos estáticos do mapa
            if (mapa[i][j] == 1) {
                printf("# ");
            } else if (mapa[i][j] == 2) {
                printf("$ ");
            } else {
                printf(". ");
            }
        }
        printf("\n");
    }
}

// 2. FUNÇÃO: Lógica de IA para movimentação dos monstros
void moverMonstros(Monstro monstros[], Jogador *p, char *msg) {
    int k;
    for (k = 0; k < QTD_MONSTROS; k++) {
        if (!monstros[k].ativo) continue;

        int novoX = monstros[k].x;
        int novoY = monstros[k].y;

        // IA Simples: O monstro tenta se aproximar do jogador
        int dx = p->x - monstros[k].x;
        int dy = p->y - monstros[k].y;

        if (abs(dx) > abs(dy)) {
            if (dx > 0) novoX++;
            else if (dx < 0) novoX--;
        } else {
            if (dy > 0) novoY++;
            else if (dy < 0) novoY--;
        }

        // Move o monstro apenas se a nova posição NÃO for parede
        if (mapa[novoY][novoX] != 1) {
            monstros[k].x = novoX;
            monstros[k].y = novoY;
        }

        // Se o monstro alcançar o jogador
        if (monstros[k].x == p->x && monstros[k].y == p->y) {
            p->hp -= 20;
            sprintf(msg, "CUIDADO! Um fantasma te alcançou e causou 20 de dano!");
        }
    }
}

// 3. FUNÇÃO: Processa o movimento e ações do jogador
void processarJogador(Jogador *p, Monstro monstros[], int novoX, int novoY, char *msg) {
    // Colisão com parede
    if (mapa[novoY][novoX] == 1) {
        sprintf(msg, "Voce bateu na parede!");
        return;
    }

    p->x = novoX;
    p->y = novoY;

    // Coleta de moeda
    if (mapa[novoY][novoX] == 2) {
        p->moedas++;
        mapa[novoY][novoX] = 0;
        sprintf(msg, "Voce coletou uma moeda de ouro ($)!");
    } else {
        sprintf(msg, "Caminhando pelo labirinto...");
    }

    // Combate: se o jogador andar diretamente para cima de um monstro
    int k;
    for (k = 0; k < QTD_MONSTROS; k++) {
        if (monstros[k].ativo && monstros[k].x == p->x && monstros[k].y == p->y) {
            p->hp -= 15;
            monstros[k].ativo = 0; // O fantasma é eliminado
            sprintf(msg, "Voce combateu e eliminou um fantasma!");
        }
    }
}

int main() {
    Jogador heroi = {1, 1, 100, 0};

    // Inicialização dos 4 Monstros em posições diferentes do mapa
    Monstro monstros[QTD_MONSTROS] = {
        {10, 1, 1},
        {20, 5, 1},
        {5, 9, 1},
        {18, 9, 1}
    };

    char tecla;
    char mensagem[100] = "Explore o labirinto e fuja dos fantasmas!";

    while (1) {
        desenharJogo(heroi, monstros);
        printf("\nStatus: %s\n", mensagem);

        if (heroi.hp <= 0) {
            printf("\n===================================================\n");
            printf("  GAME OVER! Os fantasmas te pegaram...           \n");
            printf("===================================================\n");
            break;
        }

        if (heroi.moedas == totalMoedasNoMapa) {
            printf("\n===================================================\n");
            printf("  PARABENS! Voce limpou o labirinto de ouro!      \n");
            printf("===================================================\n");
            break;
        }

        tecla = getch();

        int novoX = heroi.x;
        int novoY = heroi.y;

        if (tecla == 'w' || tecla == 'W') novoY--;
        if (tecla == 's' || tecla == 'S') novoY++;
        if (tecla == 'a' || tecla == 'A') novoX--;
        if (tecla == 'd' || tecla == 'D') novoX++;
        if (tecla == 'q' || tecla == 'Q') {
            printf("\nJogo encerrado.\n");
            break;
        }

        // Executa a rodada: move o jogador e depois atualiza a IA dos monstros
        processarJogador(&heroi, monstros, novoX, novoY, mensagem);
        moverMonstros(monstros, &heroi, mensagem);
    }

    return 0;
}
