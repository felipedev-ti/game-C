#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

#define LARGURA 15
#define ALTURA 10

// Estrutura do Jogador
typedef struct {
    int x;
    int y;
    int hp;
    int moedas;
} Jogador;

// Legenda do Mapa:
// 0 = Chão (.)
// 1 = Parede (#)
// 2 = Moeda ($)
// 3 = Monstro (M)
int mapa[ALTURA][LARGURA] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,2,0,0,1,0,0,3,0,0,2,0,1},
    {1,0,1,1,0,0,1,0,1,1,1,1,0,0,1},
    {1,0,1,0,0,3,0,0,0,0,0,1,0,0,1},
    {1,0,1,0,1,1,1,1,1,0,0,1,2,0,1},
    {1,0,2,0,1,0,0,2,1,0,0,0,0,0,1},
    {1,0,1,0,1,0,0,0,1,0,1,1,1,0,1},
    {1,0,1,0,0,0,1,0,0,3,0,0,1,0,1},
    {1,0,0,0,3,0,1,0,0,0,0,2,0,0,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

int totalMoedasNoMapa = 5;

void desenharMapa(Jogador p) {
    system("cls");
    
    printf("======================================\n");
    printf("         DUNGEON CRAWLER C - FASE 2   \n");
    printf("======================================\n");
    printf("Controles: [W] Cima | [S] Baixo | [A] Esquerda | [D] Direita | [Q] Sair\n");
    printf("HP: %d/100  |  Moedas Coletadas: %d/%d\n\n", p.hp, p.moedas, totalMoedasNoMapa);

    int i, j;
    for (i = 0; i < ALTURA; i++) {
        for (j = 0; j < LARGURA; j++) {
            if (i == p.y && j == p.x) {
                printf("@ "); // Herói
            } 
            else if (mapa[i][j] == 1) {
                printf("# "); // Parede
            } 
            else if (mapa[i][j] == 2) {
                printf("$ "); // Moeda
            } 
            else if (mapa[i][j] == 3) {
                printf("M "); // Monstro
            } 
            else {
                printf(". "); // Chão
            }
        }
        printf("\n");
    }
}

int main() {
    Jogador heroi = {1, 1, 100, 0};
    char tecla;
    char mensagem[100] = "Explore a masmorra com cuidado!";

    while (1) {
        desenharMapa(heroi);
        printf("\nStatus: %s\n", mensagem);

        // Checa Condição de Derrota
        if (heroi.hp <= 0) {
            printf("\n======================================\n");
            printf("  GAME OVER! O heroi foi derrotado... \n");
            printf("======================================\n");
            break;
        }

        // Checa Condição de Vitória
        if (heroi.moedas == totalMoedasNoMapa) {
            printf("\n======================================\n");
            printf("  PARABENS! Voce coletou todo o ouro! \n");
            printf("======================================\n");
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
            printf("\nVoce abandonou a busca!\n");
            break;
        }

        // 1. Colisão com Parede
        if (mapa[novoY][novoX] == 1) {
            sprintf(mensagem, "Voce bateu na parede!");
        } 
        // 2. Coleta de Moeda
        else if (mapa[novoY][novoX] == 2) {
            heroi.moedas++;
            mapa[novoY][novoX] = 0; // Transforma a moeda em chão comum (.)
            heroi.x = novoX;
            heroi.y = novoY;
            sprintf(mensagem, "Voce encontrou uma moeda de ouro ($)!");
        } 
        // 3. Combate com Monstro
        else if (mapa[novoY][novoX] == 3) {
            heroi.hp -= 25; // Toma 25 de dano
            mapa[novoY][novoX] = 0; // O monstro morre após o combate
            heroi.x = novoX;
            heroi.y = novoY;
            sprintf(mensagem, "CUIDADO! Um monstro (M) te atacou! (-25 HP)");
        } 
        // 4. Movimento Normal
        else {
            heroi.x = novoX;
            heroi.y = novoY;
            sprintf(mensagem, "Caminhando pela masmorra...");
        }
    }

    return 0;
}
