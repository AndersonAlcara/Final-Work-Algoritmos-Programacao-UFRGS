#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#define ARESTA   20
#define LARGURA  540
#define ALTURA   260

typedef struct{
    int posicoes_Xw[308];//vetor para as coordenadas X das paredes indestrutiveis 'W'// 308 caso tivessemos apenas paredes indestrutiveis em todo o mapa
    int posicoes_Yw[308];//vetor para as coordenadas Y das paredes indestrutiveis 'W'
    int qntdW;//conta quantas paredes indestrutiveis existem no mapa
}PAREDES;

typedef struct{
    int pos_dinamicaPersX;
    int pos_dinamicaPersY;
    int persdx;
    int persdy;
}PERSONAGEM;

typedef struct{
    int posicoes_Xp[308];//vetor para as coordenadas X das poçoes 'P'//308 caso tivessemos apenas poçoes em todo o mapa
    int posicoes_Yp[308];//vetor para as coordenadas Y das pocoes 'P'
    int qntdP;//conta quantas pocoes existem no mapa
}CONSUMIVEL;

typedef struct{
    int pontuacao;//calcula o SCORE do player
    int vidas;
    int nivel;
    int bombas;
}CONTADORES;

void posicaoJogador();
void initJogo();
void desenhaMapaEstatico();
void desenhaJogo();
int  podeMover();
int  moveParaPocao();

int main()
{
    //paredes indestrutiveis
    PAREDES indestrutiveis;//cria a variavel para as paredes indestrutiveis
    indestrutiveis.qntdW = 0; //inicia a contagem das paredes em 0
    //jogador
    PERSONAGEM jogador;
    //pocao
    CONSUMIVEL pocao;
    pocao.qntdP = 0;
    //menu
    bool menu = false;//comeca com o menu fechado
    //mapa
    char mapa[11][28] =  {"WWWWWWWWWWWWWWWWWWWWWWWWWWW",
                          "WJ P  WWWW PPPPP WWWW  P  W",
                          "W          WWWWW          W",
                          "W                         W",
                          "W  P  WWWWWWWWWWWWWWW  P  W",
                          "W                         W",
                          "W  P  WWWWWWWWWWWWWWW  P  W",
                          "W                         W",
                          "WWWW                   WWWW",
                          "WWWWWPP  WWWWWWWWW  PPWWWWW",
                          "WWWWWWWWWWWWWWWWWWWWWWWWWWW"};
    //atualiza contadores
    CONTADORES info;
    info.bombas = 3;
    info.nivel = 10; //??
    info.pontuacao = 0;
    info.vidas = 3;

    InitWindow(LARGURA, ALTURA, "O jogo");
    posicaoJogador(mapa, &(jogador.pos_dinamicaPersX), &(jogador.pos_dinamicaPersY));
    initJogo(mapa, &indestrutiveis, &pocao);//faz uma leitura do mapa e devolve a posicao inicial do jogador
    SetTargetFPS(60);

    //main game loop
    while (!WindowShouldClose())
    {
        if(IsKeyPressed(KEY_TAB))
            menu = !menu;//se TAB for pressionado abre um menu

        if(menu){
        //menu aberto que não atualiza mais as ações do jogo. AINDA NAO CONFIGURADO

        }else{
            jogador.persdx = 0;
            jogador.persdy = 0;//zera os deslocamentos

            if(IsKeyPressed(KEY_RIGHT))jogador.persdx = ARESTA;
            if(IsKeyPressed(KEY_LEFT))jogador.persdx =- ARESTA;
            if(IsKeyPressed(KEY_UP))jogador.persdy =- ARESTA;
            if(IsKeyPressed(KEY_DOWN))jogador.persdy = ARESTA;//analisa o deslocamento

            if(podeMover(jogador, indestrutiveis)== 0){//se puder mover
                if(moveParaPocao(jogador, &pocao)== 0){//se for para cima de uma pocao
                    colhePocao(&jogador, mapa, &(info.pontuacao));//realiza o processo de captura da pocao
                    pocao.qntdP -= 1;
                    initJogo(mapa, &indestrutiveis, &pocao);
                }else{
                    jogador.pos_dinamicaPersX += jogador.persdx;
                    jogador.pos_dinamicaPersY += jogador.persdy;//caso contrario, só desloca o personagem
                }
            }

        }

        desenhaJogo(mapa, jogador.pos_dinamicaPersX, jogador.pos_dinamicaPersY, menu, info);//reproduz o grafico

    }

    CloseWindow();

    return 0;
}
