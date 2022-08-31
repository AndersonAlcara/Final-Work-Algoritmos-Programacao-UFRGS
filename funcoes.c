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
    int posicoes_Xp[308];//vetor para as coordenadas X das po�oes 'P'//308 caso tivessemos apenas po�oes em todo o mapa
    int posicoes_Yp[308];//vetor para as coordenadas Y das pocoes 'P'
    int qntdP;//conta quantas pocoes existem no mapa
}CONSUMIVEL;

typedef struct{
    int pontuacao;//calcula o SCORE do player
    int vidas;
    int nivel;
    int bombas;
}CONTADORES;

void posicaoJogador(char mapa[][28], int* pos_dinamicaPersX, int* pos_dinamicaPersY)
{
    int x = 0, y = 0;
    int i, j;

    for(i = 0; i < 11; i++){//percorre a matriz mapa e identifica a posicao inicial do jogador, onde h� paredes indestrutiveis e quantas sao elas
        for(j = 0; j < 28; j++){
            if(mapa[i][j] == 'J'){
                *pos_dinamicaPersX = x;
                *pos_dinamicaPersY = y;//guarda a posicao inicial do jogador
            }
            x = x + ARESTA;
        }
        y = y + ARESTA;
        x = 0;
    }
}

void initJogo(char mapa[][28], PAREDES *indestrutiveis, CONSUMIVEL *pocao)
{
    int x = 0, y = 0;
    int aux = 0, aux2 = 0;
    int i, j;

    for(i = 0; i < 11; i++){//percorre a matriz mapa e identifica a posicao inicial do jogador, onde h� paredes indestrutiveis e quantas sao elas
        for(j = 0; j < 28; j++){
            if(mapa[i][j] == 'W'){
                indestrutiveis->posicoes_Xw[aux] = x;
                indestrutiveis->posicoes_Yw[aux] = y;//guarda as coordenadas das paredes indestrutiveis
                aux++;
            }
             if(mapa[i][j] == 'P'){
                pocao->posicoes_Xp[aux2] = x;
                pocao->posicoes_Yp[aux2] = y;//guarda as coordenadas das pocoes
                aux2++;
            }
            x = x + ARESTA;
        }
        y = y + ARESTA;
        x = 0;
    }
    indestrutiveis->qntdW = aux;//quantas paredes indestrutiveis
    pocao->qntdP = aux2;//quantas pocoes
}

void desenhaMapaEstatico(char mapa[][28])
{
    int x = 0, y = 0;
    int i, j;
    //desenha graficamente o mapa est�tico, que n�o ter� atualiza��es, ou seja, as paredes indestrutiveis
     for(i = 0; i < 11; i++){
        for(j = 0; j < 28; j++){
            if(mapa[i][j] == 'W'){
                DrawRectangle(x, y, ARESTA, ARESTA, BLACK);//desenha um cubo azul onde h� parede indestruitivel
            }
            if(mapa[i][j] == 'P'){
                DrawRectangle(x, y, ARESTA, ARESTA, MAGENTA);//desenha as pocoes
            }
            x = x + ARESTA; //posi��o no eixo horizontal do pr�ximo elemento
        }
        y = y + ARESTA;
        x = 0;//desce uma linha e volta para a coluna '0'.
    }
}

void desenhaJogo(char mapa[][28], int pos_dinamicaPersX, int pos_dinamicaPersY, int menu, CONTADORES info)
{
    //faz qualquer atualiza��o grafica
    BeginDrawing();
    ClearBackground(LIGHTGRAY);
    desenhaMapaEstatico(mapa);//desenha o mapa
    DrawRectangle(pos_dinamicaPersX, pos_dinamicaPersY, ARESTA, ARESTA, BLUE);//desenha o personagem
    if(menu){//por enquanto s� exibe, as opcoes nao sao funcionais ainda
        DrawRectangle(40, 40, 460, 140, BEIGE);
        DrawText("MENU", 210, 50, 30, BLACK);
        DrawText("(N)Novo Jogo", 210, 100, 20, BLACK);
        DrawText("(C)Carregar Jogo", 210, 125, 20, BLACK);
        DrawText("(S)Salvar Jogo", 210, 150, 20, BLACK);
        DrawText("(Q)Sair do Jogo", 210, 175, 20, BLACK);
        DrawText("(V)Voltar", 210, 200, 20, BLACK);
    }
    DrawText(TextFormat("Score: %i", info.pontuacao), 5, 225, 30, GRAY);//vai contando o SCORE do player
    DrawText(TextFormat("Vidas: %i", info.vidas), 410, 238, 20, BLACK);
    DrawText(TextFormat("Bombas: %i", info.bombas), 410, 220, 20, BLACK);
    DrawText(TextFormat("Lvl:%i", info.nivel), 225, 227, 25, RED);
    EndDrawing();
}

int podeMover(PERSONAGEM jogador, PAREDES indestrutiveis)
{
    //ve se o personagem consegue se mover, ou seja, se n�o vai ocupar o mesmo espa�o que outra parede indestrutivel
    int colidiu = 0;
    int i;

    for(i = 0; i < indestrutiveis.qntdW; i++){
        if(((jogador.pos_dinamicaPersX + jogador.persdx)== indestrutiveis.posicoes_Xw[i])&&((jogador.pos_dinamicaPersY + jogador.persdy)== indestrutiveis.posicoes_Yw[i])){
            colidiu = 1;
        }
    }
    return colidiu;

}


void colhePocao(PERSONAGEM *jogador, char mapa[][28], CONTADORES *info)
{
    int j, i;

    jogador->pos_dinamicaPersX += jogador->persdx;
    jogador->pos_dinamicaPersY += jogador->persdy;//primeiro, atualiza a posicao do jogador

    j = jogador->pos_dinamicaPersX/ARESTA;
    i = jogador->pos_dinamicaPersY/ARESTA;//Se as posicoes dinamicas s�o o produto da aresta pela posicao na matriz caracter, logo as posicoes na matriz caracter serao a divisao das posi�oes dinamicas pela aresta.

    mapa[i][j] = ' ';//onde h� pocao e o est� em cima dessa posicao, nossa matriz caracter tera esse 'P' trocado por um ' ', ou seja, ir� liberar um espa�o de livre.
    info->pontuacao = info->pontuacao + 50;//contabiliza 50 pontos por pocao pega
}


int moveParaPocao(PERSONAGEM jogador, CONSUMIVEL *pocao)
{
    //ve se o personagem vai ocupar as  mesmas coordenadas de uma pocao
    int colidiu = 1;
    int i;

    for(i = 0; i < pocao->qntdP; i++){
        if(((jogador.pos_dinamicaPersX + jogador.persdx)== pocao->posicoes_Xp[i])&&((jogador.pos_dinamicaPersY +jogador.persdy)== pocao->posicoes_Yp[i])){
            colidiu = 0;//vai ocupar
        }
    }
    return colidiu;

}



