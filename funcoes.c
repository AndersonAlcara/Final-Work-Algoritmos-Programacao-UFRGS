//Bibliotecas------------------------------------------------------------------
#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
//----------------------------------------------------------------------------

//Constantes------------------------------------------------------------------
#define ARESTA   20
#define LARGURA  540
#define ALTURA   260
//----------------------------------------------------------------------------

//Estruturas-------------------------------------------------------------------
typedef struct{
    int posicoes_X[308];//vetor para as coordenadas X das paredes indestrutiveis 'W'// 308 caso tivessemos apenas paredes indestrutiveis em todo o mapa
    int posicoes_Y[308];//vetor para as coordenadas Y das paredes indestrutiveis 'W'
    int qntd;//conta quantas paredes indestrutiveis existem no mapa
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

typedef struct{
    bool bomba;
    int pos_x_bomba;
    int pos_y_bomba;
    int contador_frames;
    bool timer;
}BOMBA;

typedef struct{
    int desl;
    int qtd_passos;
    char direcao_desl;
    int posX, posY;
}MONSTROS;
//----------------------------------------------------------------------------

void posicaoJogador(char mapa[][28], int* pos_dinamicaPersX, int* pos_dinamicaPersY, int *contaseres, int *contamonstros)
{
    int x = 0, y = 0;
    int i, j;

    for(i = 0; i < 11; i++){//percorre a matriz mapa e identifica a posicao inicial do jogador, onde há paredes indestrutiveis e quantas sao elas
        for(j = 0; j < 28; j++){
            if(mapa[i][j] == 'J'){
                *pos_dinamicaPersX = x;
                *pos_dinamicaPersY = y;//guarda a posicao inicial do jogador
            }
            if(mapa[i][j] == 'K'){
                *contaseres+= 1;//achou um ser
            }
            if(mapa[i][j] == 'M'){
                *contamonstros+= 1;//achou um monstro
            }
            x = x + ARESTA;//vai uma coluna pro lado
        }
        y = y + ARESTA;//vai uma linha pra baixo
        x = 0;//e volta para a primeira coluna
    }
}

void inicializaMonstro(MONSTROS monstros[], char mapa[][28])
{
    int x = 0, y = 0, aux = 0;
    int i, j;

    for(i = 0; i < 11; i++){
        for(j = 0; j < 28; j++){
            if(mapa[i][j] == 'M'){
                monstros[aux].posX = x;
                monstros[aux].posY = y;//guarda as coordenadas desse monstro em um espaço do vetor
                aux++;
            }
            x = x + ARESTA;//vai uma coluna pro lado
        }
        y = y + ARESTA;//vai uma linha pra baixo
        x = 0;//e volta para a primeira coluna
    }

}

void inicializaSer(MONSTROS seres[], char mapa[][28])
{
    int x = 0, y = 0, aux = 0;
    int i, j;

    for(i = 0; i < 11; i++){
        for(j = 0; j < 28; j++){
            if(mapa[i][j] == 'K'){
                seres[aux].posX = x;
                seres[aux].posY = y;//guarda as coordenadas desse ser em um espaço do vetor
                aux++;
            }
            x = x + ARESTA;//vai uma coluna pro lado
        }
        y = y + ARESTA;//vai uma linha pra baixo
        x = 0;//e volta para a primeira coluna
    }

}

void initJogo(char mapa[][28], PAREDES *indestrutiveis, CONSUMIVEL *pocao, PAREDES *destrutiveis)
{
    int x = 0, y = 0;
    int aux = 0, aux2 = 0, aux3 = 0;
    int i, j;

    for(i = 0; i < 11; i++){//percorre a matriz mapa e identifica a posicao inicial do jogador, onde há paredes indestrutiveis e quantas sao elas
        for(j = 0; j < 28; j++){
            if(mapa[i][j] == 'W'){
                indestrutiveis->posicoes_X[aux] = x;
                indestrutiveis->posicoes_Y[aux] = y;//guarda as coordenadas das paredes indestrutiveis
                aux++;
            }
            if(mapa[i][j] == 'P'){
                pocao->posicoes_Xp[aux2] = x;
                pocao->posicoes_Yp[aux2] = y;//guarda as coordenadas das pocoes
                aux2++;
            }
            if(mapa[i][j] == 'D'){
                destrutiveis->posicoes_X[aux3] = x;
                destrutiveis->posicoes_Y[aux3] = y;//guarda as coordenadas das paredes destrutiveis
                aux3++;
            }
            x = x + ARESTA;
        }
        y = y + ARESTA;
        x = 0;
    }
    indestrutiveis->qntd = aux;//quantas paredes indestrutiveis
    pocao->qntdP = aux2;//quantas pocoes
    destrutiveis->qntd = aux3;//quantas paredes destrutiveis
}

void desenhaMapaEstatico(char mapa[][28])
{
    int x = 0, y = 0;
    int i, j;
    //desenha graficamente o mapa estático, que não terá atualizações, ou seja, as paredes indestrutiveis
     for(i = 0; i < 11; i++){
        for(j = 0; j < 28; j++){
            if(mapa[i][j] == 'W'){
                DrawRectangle(x, y, ARESTA, ARESTA, BLACK);//desenha um cubo azul onde há parede indestruitivel
            }
            if(mapa[i][j] == 'P'){
                DrawRectangle(x, y, ARESTA, ARESTA, MAGENTA);//desenha as pocoes
            }
            if(mapa[i][j] == 'D'){
                DrawRectangle(x, y, ARESTA, ARESTA, GRAY);//desenha paredes destrutiveis
            }
            x = x + ARESTA; //posição no eixo horizontal do próximo elemento
        }
        y = y + ARESTA;
        x = 0;//desce uma linha e volta para a coluna '0'.
    }
}

void desenhaJogo(char mapa[][28], PERSONAGEM jogador, int menu, CONTADORES info, BOMBA bomba[], int danoX[], int danoY[], bool explode, MONSTROS seres[], MONSTROS monstros[], int contamonstros, int contaseres)
{
    int i;
    //faz qualquer atualização grafica
    BeginDrawing();
    ClearBackground(LIGHTGRAY);
    desenhaMapaEstatico(mapa);//desenha o mapa
    DrawRectangle(jogador.pos_dinamicaPersX, jogador.pos_dinamicaPersY, ARESTA, ARESTA, BLUE);//desenha o personagem

    for(i = 0; i < contaseres; i++)
        DrawRectangle(seres[i].posX*ARESTA, seres[i].posY*ARESTA, ARESTA, ARESTA, ORANGE);//desenha os seres
    for(i = 0; i < contamonstros; i++)
        DrawRectangle(monstros[i].posX*ARESTA, monstros[i].posY*ARESTA, ARESTA, ARESTA, RED);//desenha os monstros

    for(i = 0; i < 3; i++){//verifica se alguma das 3 bombas está plantada
        if(bomba[i].bomba==true){//se estiver
            DrawRectangle(bomba[i].pos_x_bomba, bomba[i].pos_y_bomba, ARESTA, ARESTA, YELLOW);//desenha uma bomba amarela
        }
        if(explode == true){//se estiver em sua fase de explosao
            for(i = 0; i < 5; i++){
                DrawRectangle(danoX[i], danoY[i], ARESTA, ARESTA, BROWN);//desenha um '+' rapidamente, mostrando o raio de dano
            }
        }
    }
    if(menu){//por enquanto só exibe, as opcoes nao sao funcionais ainda
        DrawRectangle(40, 40, 460, 140, BEIGE);
        DrawText("MENU", 210, 50, 30, BLACK);
        DrawText("(N)Novo Jogo", 210, 100, 20, BLACK);
        DrawText("(C)Carregar Jogo", 210, 125, 20, BLACK);
        DrawText("(S)Salvar Jogo", 210, 150, 20, BLACK);
        DrawText("(Q)Sair do Jogo", 210, 175, 20, BLACK);
        DrawText("(V)Voltar", 210, 200, 20, BLACK);
    }

    DrawText(TextFormat("Score: %i", info.pontuacao), 5, 225, 30, GRAY);//vai contando o SCORE do player
    DrawText(TextFormat("Vidas: %i", info.vidas), 410, 238, 20, BLACK);//vai contando a VIDA do player
    DrawText(TextFormat("Bombas: %i", info.bombas), 410, 220, 20, BLACK);//vai contando o ARSENAL de bombas do player
    DrawText(TextFormat("Lvl:%i", info.nivel), 225, 227, 25, RED);//informa o nivel em que o jogador se encotnra (??)
    EndDrawing();
}

int podeMover(PERSONAGEM jogador, PAREDES indestrutiveis, PAREDES destrutiveis, BOMBA bomba[])
{
    //ve se o personagem consegue se mover, ou seja, se não vai ocupar o mesmo espaço que outra parede indestrutivel
    int colidiu = 0;
    int i;

    for(i = 0; i < indestrutiveis.qntd; i++){//verifica se há colisao com paredes indestrutiveis
        if(((jogador.pos_dinamicaPersX + jogador.persdx)== indestrutiveis.posicoes_X[i])&&((jogador.pos_dinamicaPersY + jogador.persdy)== indestrutiveis.posicoes_Y[i])){
            colidiu = 1;
        }
    }
    for(i = 0; i < destrutiveis.qntd; i++){//verifica se há colisao com paredes destrutiveis
        if(((jogador.pos_dinamicaPersX + jogador.persdx)== destrutiveis.posicoes_X[i])&&((jogador.pos_dinamicaPersY + jogador.persdy)== destrutiveis.posicoes_Y[i])){
            colidiu = 1;
        }
    }

    for(i = 0; i < 3; i++){
        if(bomba[i].bomba==true)
            if(((jogador.pos_dinamicaPersX + jogador.persdx)== bomba[i].pos_x_bomba)&&(jogador.pos_dinamicaPersY + jogador.persdy)== bomba[i].pos_y_bomba)
                colidiu = 1;
    }
    return colidiu;
}

void explosao(int *vidas, char mapa[][28], int posJogadorX, int posJogadorY, int posBombaX, int posBombaY, int danoX[], int danoY[], int *pontuacao)
{
    int i;
    //define o raio de dano a partir da posicao de onde a bomba foi plantada, ou seja, um quadrado a cima, um abaixo, um a esquerda, e um a direita, e claro, no mesmo lugar em que foi plantada.
    danoX[0] = posBombaX + ARESTA;
    danoX[1] = posBombaX - ARESTA;
    danoX[2] = posBombaX;
    danoX[3] = posBombaX;
    danoX[4] = posBombaX;
    danoY[0] = posBombaY;
    danoY[1] = posBombaY;
    danoY[2] = posBombaY + ARESTA;
    danoY[3] = posBombaY - ARESTA;
    danoY[4] = posBombaY;

    for(i = 0; i < 5; i++){
        if(danoX[i]== posJogadorX && danoY[i]== posJogadorY){//se alguma coordenada do raio de dano coincidir com a posicao do jogador
            *vidas -= 1;//perde uma vida
            if(*pontuacao>=100)
                *pontuacao -= 100;//se tiver pontos suficientes para perder, perde
            else
                if(*pontuacao<100)//se não tiver pontos suficientes, zera o score, visando não torná-lo negativo
                    *pontuacao = 0;
        }
    }

}

void colhePocao(PERSONAGEM *jogador, char mapa[][28], CONTADORES *info)
{
    int j, i;

    jogador->pos_dinamicaPersX += jogador->persdx;
    jogador->pos_dinamicaPersY += jogador->persdy;//primeiro, atualiza a posicao do jogador

    j = jogador->pos_dinamicaPersX/ARESTA;
    i = jogador->pos_dinamicaPersY/ARESTA;//Se as posicoes dinamicas são o produto da aresta pela posicao na matriz caracter, logo as posicoes na matriz caracter serao a divisao das posiçoes dinamicas pela aresta.

    mapa[i][j] = ' ';//onde há pocao e o está em cima dessa posicao, nossa matriz caracter tera esse 'P' trocado por um ' ', ou seja, irá liberar um espaço de livre.
    info->pontuacao = info->pontuacao + 50;//contabiliza 50 pontos por pocao pega
}

int moveParaPocao(PERSONAGEM jogador, CONSUMIVEL *pocao)
{
    //ve se o personagem vai ocupar as  mesmas coordenadas de uma pocao
    int colidiu = 1;
    int i;
    for(i = 0; i < pocao->qntdP; i++){
        if(((jogador.pos_dinamicaPersX + jogador.persdx)== pocao->posicoes_Xp[i])&&((jogador.pos_dinamicaPersY +jogador.persdy)== pocao->posicoes_Yp[i])){//
            colidiu = 0;//vai ocupar
        }
    }
    return colidiu;
}
