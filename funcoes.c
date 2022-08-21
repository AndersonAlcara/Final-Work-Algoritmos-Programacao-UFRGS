#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#define ARESTA 20
#define LARGURA 540
#define ALTURA 260
void descobrePosI();
void desenhaMapaEstatico();
void desenhaJogo();
int podeMover();
void atualizaJogo();

void initJogo(int* posicaoIpersX, int* posicaoIpersY, char mapa[][28], int* jogadorX, int* jogadorY, int* qntdW, int xw[], int yw[])
{
    int x = 0, y = 0;
    int aux = 0;

    for(int i = 0; i < 11; i++){//percorre a matriz mapa e identifica a posicao inicial do jogador, onde h� paredes indestrutiveis e quantas sao elas
        for(int j = 0; j < 28; j++){
            if(mapa[i][j]=='J'){
                *posicaoIpersX = x;
                *posicaoIpersY = y;
            }
            if(mapa[i][j]=='W'){
                xw[aux] = x;
                yw[aux] = y;
                aux++;
            }

            x = x + ARESTA;

        }
        y = y + ARESTA;
        x = 0;

    }
    *jogadorX = *posicaoIpersX;//repassa a posicao inicial do jogador para uma posi��o din�mica, que ser� atualizada por cada acao de movimento do jogador
    *jogadorY = *posicaoIpersY;
    *qntdW = aux;//quantas paredes indestrutiveis
}



void desenhaMapaEstatico(char mapa[][28])
{
    int x = 0, y = 0;
    //desenha graficamente o mapa est�tico, que n�o ter� atualiza��es, ou seja, as paredes indestrutiveis
     for(int i = 0; i < 11; i++){
        for(int j = 0; j < 28; j++){
            if(mapa[i][j]=='W'){
                DrawRectangle(x, y, ARESTA, ARESTA, BLUE);
            }

            x = x + ARESTA; //posi��o no eixo horizontal do pr�ximo elemento
        }
        y = y + ARESTA;
        x = 0;//desce uma linha e volta para a coluna '0'.
    }
}


void desenhaJogo(char mapa[11][28], int xw[], int yw[], int*posicaoatualX, int*posicaoatualY, int menu)
{
    //faz qualquer atualiza��o grafica
    BeginDrawing();
    ClearBackground(RAYWHITE);
    desenhaMapaEstatico(mapa);
    DrawRectangle(*posicaoatualX, *posicaoatualY, ARESTA, ARESTA, ORANGE);
    if(menu){
        DrawText("MENU", 210, 50, 30, BLACK);
        DrawText("(N)Novo Jogo", 210, 100, 20, BLACK);
        DrawText("(C)Carregar Jogo", 210, 125, 20, BLACK);
        DrawText("(S)Salvar Jogo", 210, 150, 20, BLACK);
        DrawText("(Q)Sair do Jogo", 210, 175, 20, BLACK);
        DrawText("(V)Voltar", 210, 200, 20, BLACK);
    }
    EndDrawing();
}

int podeMover(int posicaoatualX, int posicaoatualY, int persdx, int persdy, int xw[], int yw[], int qntdW)
{
    //ve se o personagem consegue se mover, ou seja, se n�o vai ocupar o mesmo espa�o que outra parede indestrutivel
    int colidiu=0;
    for(int i = 0; i < qntdW; i++){
        if(((posicaoatualX + persdx)==xw[i])&&((posicaoatualY + persdy)==yw[i])){
            return 1;
        }
    }
    return 0;

}

void atualizaJogo(int *posicaoatualX, int *posicaoatualY, int xw[], int yw[], int qntdW)
{
    //l� as movimenta��es do jogador
    int persdx = 0, persdy = 0;

    if(IsKeyPressed(KEY_RIGHT)){
    persdx = ARESTA;

    }
    if(IsKeyPressed(KEY_LEFT)){
    persdx =- ARESTA;

    }
    if(IsKeyPressed(KEY_UP)){
    persdy =- ARESTA;

    }
    if(IsKeyPressed(KEY_DOWN)){
    persdy = ARESTA;

    }
    if(podeMover(*posicaoatualX, *posicaoatualY, persdx, persdy, xw, yw, qntdW)==0){
        *posicaoatualX+=persdx;
        *posicaoatualY+=persdy;
    }

}








