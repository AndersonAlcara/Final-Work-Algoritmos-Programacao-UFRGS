#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#define ARESTA 20
#define LARGURA 540
#define ALTURA 260
void initJogo();
void desenhaMapaEstatico();
void desenhaJogo();
int podeMover();
void atualizaJogo();

int main()
{
    bool menu = false;
    int* jogadorX=0, *jogadorY=0;
    int* posIx=0, *posIy=0;
    char mapa[11][28] =  {"WWWWWWWWWWWWWWWWWWWWWWWWWWW",
                          "W     W                   W",
                          "W     W                   W",
                          "W     W          J    W   W",
                          "W     WWWWWWW             W",
                          "W           W           W W",
                          "W           W      W  WW  W",
                          "W       WWWWW     WW   WWWW",
                          "W       WWWWWWWWWWWW  WW  W",
                          "W WWWWWWWWWWWWWWWWWWWWWWWWW",
                          "WWWWWWWWWWWWWWWWWWWWWWWWWWW"};
    int xw[308];//vetor para as coordenadas X das paredes indestrutiveis 'W'
    int yw[308];//vetor para as coordenadas Y das paredes indestrutiveis 'W'
    int *qntdW = 0;//conta quantas paredes indestrutiveis existem no mapa


    InitWindow(LARGURA, ALTURA, "O jogo");
    initJogo(&posIx, &posIy, mapa, &jogadorX, &jogadorY, &qntdW, xw, yw);//faz uma leitura do mapa
    SetTargetFPS(60);


    //main game loop
    while (!WindowShouldClose())
    {
        if(IsKeyPressed(KEY_TAB))
            menu = !menu;//se TAB for pressionado abre um menu

        if(menu){
        //menu aberto que não atualiza mais as ações do jogo. AINDA NAO CONFIGURADO

        }else
        {
            atualizaJogo(&jogadorX, &jogadorY, xw, yw, qntdW);//atualiza o jogo como, por enquanto, a movimentacao do jogador
        }

        desenhaJogo(mapa, xw, yw, &jogadorX, &jogadorY, menu);//reproduz o grafico

    }

    CloseWindow();

    return 0;

}
