#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#define ARESTA   20
#define LARGURA  540
#define ALTURA   260


void initJogo();
void desenhaMapaEstatico();
void desenhaJogo();
int  podeMover();
void atualizaJogo();
int  moveParaPocao();
void funcaoPontua();


int main()
{
    bool menu = false;
    int* pos_dinamicaPersX = 0;
    int* pos_dinamicaPersY = 0;
    char mapa[11][28] =  {"WWWWWWWWWWWWWWWWWWWWWWWWWWW",
                          "WJPWWWWWW  PPPPPWWWWWWWWPPW",
                          "W WW      WWWWWW   PPWW  PW",
                          "W                       WWW",
                          "W     PWWWWWWWWWWWWW     PW",
                          "W   PPWWWWWWWWWWWWWWW  WWWW",
                          "W    WWWWWWWWWWWWWWWWW   PW",
                          "W      PPPP WWWWWPPWWW    W",
                          "W WWWWWWW      WWW W      W",
                          "WPWW      PP           PP W",
                          "WWWWWWWWWWWWWWWWWWWWWWWWWWW"};
    int posicoes_Xw[308];//vetor para as coordenadas X das paredes indestrutiveis 'W'// 308 caso tivessemos apenas paredes indestrutiveis em todo o mapa
    int posicoes_Yw[308];//vetor para as coordenadas Y das paredes indestrutiveis 'W'
    int *qntdW = 0;//conta quantas paredes indestrutiveis existem no mapa
    int posicoes_Xp[308];//vetor para as coordenadas X das poçoes 'P'//308 caso tivessemos apenas poçoes em todo o mapa
    int posicoes_Yp[308];//vetor para as coordenadas Y das pocoes 'P'
    int *qntdP = 0;//conta quantas pocoes existem no mapa
    int *pontuacao = 0;//calcula o SCORE do player

    InitWindow(LARGURA, ALTURA, "O jogo");
    initJogo(&pos_dinamicaPersX, &pos_dinamicaPersY, mapa, &qntdW, posicoes_Xw, posicoes_Yw, &qntdP, posicoes_Xp, posicoes_Yp);//faz uma leitura do mapa e devolve a posicao inicial do jogador
    SetTargetFPS(60);


    //main game loop
    while (!WindowShouldClose())
    {
        if(IsKeyPressed(KEY_TAB))
            menu = !menu;//se TAB for pressionado abre um menu

        if(menu){
        //menu aberto que não atualiza mais as ações do jogo. AINDA NAO CONFIGURADO

        }else{
            atualizaJogo(&pos_dinamicaPersX, &pos_dinamicaPersY, posicoes_Xw, posicoes_Yw, &qntdW, &pontuacao, posicoes_Xp, posicoes_Yp, &qntdP, mapa);//atualiza o jogo como, por enquanto, a movimentacao do jogador
        }

        desenhaJogo(mapa, posicoes_Xw, posicoes_Yw, pos_dinamicaPersX, pos_dinamicaPersY, menu, pontuacao);//reproduz o grafico

    }

    CloseWindow();

    return 0;

}
