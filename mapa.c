#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
    #include <conio.h>
    #include <windows.h>
    #define LIMPAR_TELA() system("cls")
#else
    #include <termios.h>
    #include <unistd.h>
    #define LIMPAR_TELA() printf("\033[H\033[2J")
#endif

#define LARGURA 22
#define ALTURA 12
#define QTD_MONSTROS 4
#define MSG_TAM 100
#define HP_MAX 100
#define DANO_FANTASMA 20
#define DANO_COMBATE 15
#define TURNOS_POR_PASSO_MONSTRO 2   

typedef struct {
    int x, y;
    int hp;
    int moedas;
} Jogador;

typedef struct {
    int x, y;
    int inicioX, inicioY;   
    int ativo;              
} Monstro;


static int mapa[ALTURA][LARGURA] = {
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

static int totalMoedasNoMapa = 0;

static int contarMoedas(void) {
    int total = 0;
    for (int i = 0; i < ALTURA; i++){
        for (int j = 0; j < LARGURA; j++)
            if (mapa[i][j] == 2) total++;
        }
    return total;
}


static int lerTecla(void) {
#ifdef _WIN32
    return _getch();
#else
    struct termios antigo, novo;
    tcgetattr(STDIN_FILENO, &antigo);
    novo = antigo;
    novo.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &novo);
    int c = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &antigo);
    return c;
#endif
}

static void desenharJogo(const Jogador *p, const Monstro monstros[], const char *msg) {
    LIMPAR_TELA();

    printf("===================================================\n");
    printf("                  PAC-CRAWLER                      \n");
    printf("===================================================\n");
    printf("[W] Cima | [S] Baixo | [A] Esquerda | [D] Direita | [Q] Sair\n");
    printf("HP: %d/%d  |  Moedas: %d/%d\n\n", p->hp, HP_MAX, p->moedas, totalMoedasNoMapa);

    for (int i = 0; i < ALTURA; i++) {
        for (int j = 0; j < LARGURA; j++) {
            if (i == p->y && j == p->x) {
                printf("@ ");
                continue;
            }

            int ehMonstro = 0;
            for (int k = 0; k < QTD_MONSTROS; k++) {
                if (monstros[k].ativo && monstros[k].x == j && monstros[k].y == i) {
                    ehMonstro = 1;
                    break;
                }
            }
            if (ehMonstro) { printf("M "); continue; }

            if (mapa[i][j] == 1)      printf("# ");
            else if (mapa[i][j] == 2) printf("$ ");
            else                      printf(". ");
        }
        printf("\n");
    }
    printf("\nStatus: %s\n", msg);
}

static int celulaLivreParaMonstro(const Monstro monstros[], int x, int y) {
    if (mapa[y][x] == 1) return 0;
    for (int k = 0; k < QTD_MONSTROS; k++)
        if (monstros[k].ativo && monstros[k].x == x && monstros[k].y == y)
            return 0;
    return 1;
}

static void moverMonstros(Monstro monstros[], Jogador *p, char *msg) {
    for (int k = 0; k < QTD_MONSTROS; k++) {
        Monstro *m = &monstros[k];
        if (!m->ativo) continue;

        int dx = p->x - m->x;
        int dy = p->y - m->y;
        int passoX = (dx > 0) - (dx < 0);
        int passoY = (dy > 0) - (dy < 0);

        int tent[2][2];
        if (abs(dx) >= abs(dy)) {
            tent[0][0] = passoX; tent[0][1] = 0;
            tent[1][0] = 0;      tent[1][1] = passoY;
        } else {
            tent[0][0] = 0;      tent[0][1] = passoY;
            tent[1][0] = passoX; tent[1][1] = 0;
        }

        for (int t = 0; t < 2; t++) {
            if (tent[t][0] == 0 && tent[t][1] == 0) continue;
            int nx = m->x + tent[t][0];
            int ny = m->y + tent[t][1];
            if (celulaLivreParaMonstro(monstros, nx, ny)) {
                m->x = nx;
                m->y = ny;
                break;
            }
        }

        if (m->x == p->x && m->y == p->y) {
            p->hp -= DANO_FANTASMA;
            if (p->hp < 0) p->hp = 0;
            m->x = m->inicioX;   
            m->y = m->inicioY;
            snprintf(msg, MSG_TAM, "CUIDADO! Um fantasma te alcançou e causou %d de dano!", DANO_FANTASMA);
        }
    }
}

static int processarJogador(Jogador *p, Monstro monstros[], int novoX, int novoY, char *msg) {
    if (mapa[novoY][novoX] == 1) {
        snprintf(msg, MSG_TAM, "Você bateu na parede!");
        return 0;
    }

    p->x = novoX;
    p->y = novoY;

    if (mapa[novoY][novoX] == 2) {
        p->moedas++;
        mapa[novoY][novoX] = 0;
        snprintf(msg, MSG_TAM, "Você coletou uma moeda de ouro ($)!");
    } else {
        snprintf(msg, MSG_TAM, "Caminhando pelo labirinto...");
    }

    for (int k = 0; k < QTD_MONSTROS; k++) {
        if (monstros[k].ativo && monstros[k].x == p->x && monstros[k].y == p->y) {
            p->hp -= DANO_COMBATE;
            if (p->hp < 0) p->hp = 0;
            monstros[k].ativo = 0;
            snprintf(msg, MSG_TAM, "Você combateu e eliminou um fantasma! (-%d HP)", DANO_COMBATE);
        }
    }
    return 1;
}

int main(void) {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif

    totalMoedasNoMapa = contarMoedas();

    Jogador heroi = {1, 1, HP_MAX, 0};
    Monstro monstros[QTD_MONSTROS] = {
        {10, 1, 10, 1, 1},
        {20, 5, 20, 5, 1},
        {5, 9, 5, 9, 1},
        {18, 9, 18, 9, 1}
    };

    char mensagem[MSG_TAM] = "Explore o labirinto e fuja dos fantasmas!";
    int turno = 0;

    while (1) {
        desenharJogo(&heroi, monstros, mensagem);

        if (heroi.moedas == totalMoedasNoMapa) {
            printf("\n===================================================\n");
            printf("  PARABÉNS! Você limpou o labirinto de ouro!\n");
            printf("===================================================\n");
            break;
        }
        if (heroi.hp <= 0) {
            printf("\n===================================================\n");
            printf("  GAME OVER! Os fantasmas te pegaram...\n");
            printf("===================================================\n");
            break;
        }

        int tecla = lerTecla();
        int novoX = heroi.x, novoY = heroi.y, sair = 0;

        switch (tecla) {
            case 'w': case 'W': novoY--; break;
            case 's': case 'S': novoY++; break;
            case 'a': case 'A': novoX--; break;
            case 'd': case 'D': novoX++; break;
            case 'q': case 'Q': sair = 1; break;
            default: continue;   
        }
        if (sair) {
            printf("\nJogo encerrado.\n");
            break;
        }

        if (!processarJogador(&heroi, monstros, novoX, novoY, mensagem))
            continue;            

        turno++;
        if (turno % TURNOS_POR_PASSO_MONSTRO == 0)
            moverMonstros(monstros, &heroi, mensagem);
    }
    return 0;
}
