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

void initJogo(int* pos_dinamicaPersX, int* pos_dinamicaPersY, char mapa[][28], int* qntdW, int posicoes_Xw[], int posicoes_Yw[])
{
    int x = 0, y = 0;
    int aux = 0;
    int i, j;

    for(i = 0; i < 11; i++){//percorre a matriz mapa e identifica a posicao inicial do jogador, onde há paredes indestrutiveis e quantas sao elas
        for(j = 0; j < 28; j++){
            if(mapa[i][j] == 'J'){
                *pos_dinamicaPersX = x;
                *pos_dinamicaPersY = y;//guarda a posicao inicial do jogador
            }
            if(mapa[i][j] == 'W'){
                posicoes_Xw[aux] = x;
                posicoes_Yw[aux] = y;//guarda as coordenadas das paredes indestrutiveis
                aux++;
            }
            x = x + ARESTA;
        }
        y = y + ARESTA;
        x = 0;
    }
    *qntdW = aux;//quantas paredes indestrutiveis
}



void desenhaMapaEstatico(char mapa[][28])
{
    int x = 0, y = 0;
    int i, j;
    //desenha graficamente o mapa estático, que não terá atualizações, ou seja, as paredes indestrutiveis
     for(i = 0; i < 11; i++){
        for(j = 0; j < 28; j++){
            if(mapa[i][j] == 'W'){
                DrawRectangle(x, y, ARESTA, ARESTA, BLUE);//desenha um cubo azul onde há parede indestruitivel
            }
            x = x + ARESTA; //posição no eixo horizontal do próximo elemento
        }
        y = y + ARESTA;
        x = 0;//desce uma linha e volta para a coluna '0'.
    }
}


void desenhaJogo(char mapa[11][28], int posicoes_Xw[], int posicoes_Yw[], int pos_dinamicaPersX, int pos_dinamicaPersY, int menu)
{
    //faz qualquer atualização grafica
    BeginDrawing();
    ClearBackground(RAYWHITE);
    desenhaMapaEstatico(mapa);//desenha o mapa
    DrawRectangle(pos_dinamicaPersX, pos_dinamicaPersY, ARESTA, ARESTA, ORANGE);//desenha o personagem

    if(menu){//por enquanto só exibe, as opcoes nao sao funcionais ainda
        DrawText("MENU", 210, 50, 30, BLACK);
        DrawText("(N)Novo Jogo", 210, 100, 20, BLACK);
        DrawText("(C)Carregar Jogo", 210, 125, 20, BLACK);
        DrawText("(S)Salvar Jogo", 210, 150, 20, BLACK);
        DrawText("(Q)Sair do Jogo", 210, 175, 20, BLACK);
        DrawText("(V)Voltar", 210, 200, 20, BLACK);
    }

    EndDrawing();
}

int podeMover(int pos_dinamicaPersX, int pos_dinamicaPersY, int persdx, int persdy, int posicoes_Xw[], int posicoes_Yw[], int qntdW)
{
    //ve se o personagem consegue se mover, ou seja, se não vai ocupar o mesmo espaço que outra parede indestrutivel
    int colidiu = 0; //usar a propria colidiu
    int i;

    for(i = 0; i < qntdW; i++){
        if(((pos_dinamicaPersX + persdx)== posicoes_Xw[i])&&((pos_dinamicaPersY + persdy)== posicoes_Yw[i])){
            colidiu = 1; //nao seria possivel trocar por uma variavel?
        }
    }
    return colidiu;

}

void atualizaJogo(int* pos_dinamicaPersX, int* pos_dinamicaPersY, int posicoes_Xw[], int posicoes_Yw[], int qntdW)
{
    //deslocamentos do personagem de x e y em escala de 20 a 20
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
    // verifica se é possivel mover o personagem
    if(podeMover(*pos_dinamicaPersX, *pos_dinamicaPersY, persdx, persdy, posicoes_Xw, posicoes_Yw, qntdW)== 0){
        *pos_dinamicaPersX += persdx;
        *pos_dinamicaPersY += persdy;
    }

}








