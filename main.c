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
    int* pos_dinamicaPersX = 0, *pos_dinamicaPersY = 0;
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
    int posicoes_Xw[308];//vetor para as coordenadas X das paredes indestrutiveis 'W'// 308 caso tivessemos apenas paredes indestrutiveis em todo o mapa
    int posicoes_Yw[308];//vetor para as coordenadas Y das paredes indestrutiveis 'W'
    int *qntdW = 0;//conta quantas paredes indestrutiveis existem no mapa


    InitWindow(LARGURA, ALTURA, "O jogo");
    initJogo(&pos_dinamicaPersX, &pos_dinamicaPersY, mapa, &qntdW, posicoes_Xw, posicoes_Yw);//faz uma leitura do mapa e devolve a posicao inicial do jogador
    SetTargetFPS(60);


    //main game loop
    while (!WindowShouldClose())
    {
        if(IsKeyPressed(KEY_TAB))
            menu = !menu;//se TAB for pressionado abre um menu
                        // o booleano inicial do menu é invertido
        if(menu){
        //menu aberto que não atualiza mais as ações do jogo. AINDA NAO CONFIGURADO

        }else
        {
            atualizaJogo(&pos_dinamicaPersX, &pos_dinamicaPersY, posicoes_Xw, posicoes_Yw, qntdW);//atualiza o jogo como, por enquanto, a movimentacao do jogador
        }
        desenhaJogo(mapa, posicoes_Xw, posicoes_Yw, pos_dinamicaPersX, pos_dinamicaPersY, menu);//reproduz o grafico

    }

    CloseWindow();

    return 0;

}
