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
int moveParaPocao();
void funcaoPontua();



void initJogo(int* pos_dinamicaPersX, int* pos_dinamicaPersY, char mapa[][28], int* qntdW, int posicoes_Xw[], int posicoes_Yw[], int* qntdP, int posicoes_Xp[], int posicoes_Yp[])
{
    int x = 0, y = 0;
    int aux = 0, aux2 = 0;
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
             if(mapa[i][j] == 'P'){
                posicoes_Xp[aux2] = x;
                posicoes_Yp[aux2] = y;//guarda as coordenadas das pocoes
                aux2++;
            }
            x = x + ARESTA;
        }
        y = y + ARESTA;
        x = 0;
    }
    *qntdW = aux;//quantas paredes indestrutiveis
    *qntdP = aux2;//quantas pocoes
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
            if(mapa[i][j] == 'P'){
                DrawRectangleLines(x, y, ARESTA, ARESTA, PURPLE);//desenha as pocoes
            }
            x = x + ARESTA; //posição no eixo horizontal do próximo elemento
        }
        y = y + ARESTA;
        x = 0;//desce uma linha e volta para a coluna '0'.
    }
}




void desenhaJogo(char mapa[][28], int posicoes_Xw[], int posicoes_Yw[], int pos_dinamicaPersX, int pos_dinamicaPersY, int menu, int pontuacao)
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
    DrawText(TextFormat("Score: %i", pontuacao), 5, 220, 20, BLACK);//vai contando o SCORE do player
    EndDrawing();
}



int podeMover(int pos_dinamicaPersX, int pos_dinamicaPersY, int persdx, int persdy, int posicoes_Xw[], int posicoes_Yw[], int qntdW)
{
    //ve se o personagem consegue se mover, ou seja, se não vai ocupar o mesmo espaço que outra parede indestrutivel
    int colidiu = 0;
    int i;

    for(i = 0; i < qntdW; i++){
        if(((pos_dinamicaPersX + persdx)== posicoes_Xw[i])&&((pos_dinamicaPersY + persdy)== posicoes_Yw[i])){
            colidiu = 1;
        }
    }
    return colidiu;

}


int moveParaPocao(int* pos_dinamicaPersX, int* pos_dinamicaPersY, int persdx, int persdy, int posicoes_Xp[], int posicoes_Yp[], int* qntdP)
{
    //ve se o personagem vai ocupar as  mesmas coordenadas de uma pocao
    int colidiu = 0;
    int i;

    for(i = 0; i < *qntdP; i++){
        if(((*pos_dinamicaPersX + persdx)== posicoes_Xp[i])&&((*pos_dinamicaPersY + persdy)== posicoes_Yp[i])){
            return colidiu;//vai ocupar
        }
    }
    return colidiu = 1;

}


void atualizaJogo(int* pos_dinamicaPersX, int* pos_dinamicaPersY, int posicoes_Xw[], int posicoes_Yw[], int* qntdW, int* pontuacao, int posicoes_Xp[], int posicoes_Yp[], int* qntdP, char mapa[][28])
{
    //deslocamentos do personagem de x e y em escala de 20 a 20
    int persdx = 0, persdy = 0;
    int i, j;

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
    if(moveParaPocao(pos_dinamicaPersX, pos_dinamicaPersY, persdx, persdy, posicoes_Xp, posicoes_Yp, qntdP) == 0){
        *pos_dinamicaPersX += persdx;
        *pos_dinamicaPersY += persdy;//primeiro, atualiza a posicao do jogador
        j = *pos_dinamicaPersX/ARESTA;
        i = *pos_dinamicaPersY/ARESTA;//Se as posicoes dinamicas são o produto da aresta pela posicao na matriz caracter, logo as posicoes na matriz caracter serao a divisao das posiçoes dinamicas pela aresta.
        mapa[i][j] = ' ';//onde há pocao e o está em cima dessa posicao, nossa matriz caracter tera esse 'P' trocado por um ' ', ou seja, irá liberar um espaço de livre.
        *qntdP = *qntdP - 1;//diminui uma poção do nosso mapa
        *pontuacao = *pontuacao + 50;//contabiliza 50 pontos por pocao pega
        initJogo(&pos_dinamicaPersX, &pos_dinamicaPersY, mapa, &qntdW, posicoes_Xw, posicoes_Yw, &qntdP, posicoes_Xp, posicoes_Yp);//faz as atualizações na matriz caracter

    }else{

        if(podeMover(*pos_dinamicaPersX, *pos_dinamicaPersY, persdx, persdy, posicoes_Xw, posicoes_Yw, *qntdW)== 0){//colisao simples com paredes indestrutiveis
            *pos_dinamicaPersX += persdx;
            *pos_dinamicaPersY += persdy;//atualiza a posicao do jogador
        }

    }

}
