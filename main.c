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
    float tempo_bomba;
}BOMBA;
//----------------------------------------------------------------------------


//Protótipos------------------------------------------------------------------
void colhePocao();
void posicaoJogador();
void initJogo();
void desenhaMapaEstatico();
void desenhaJogo();
int  podeMover();
int  moveParaPocao();
void explosao();
//----------------------------------------------------------------------------


//Funções Contadoras de Tempo ------------------------------------------------
void inicia_timer(float *timer, float duracao)
{
    if (timer != NULL)
        *timer = duracao;//inicia o timer com o tempo escolhido
}

void atualiza_timer(float* timer)
{
    if (timer != NULL && *timer > 0)//se o tempo escolhido for maior do que zero
        *timer -= GetFrameTime();//decrementa por frames/segundo
}

bool acabou_timer(float* timer)
{
    if (timer != NULL && *timer <= 0)
        return true;//se o timer chegar no final retorna TRUE
    else
        return false;//senão, retorna FALSE
}
//----------------------------------------------------------------------------


//Funcão Principal------------------------------------------------------------
int main()
{
    //timer
    float tempo_real = 0;
    //bombas
    BOMBA bomba;
    bomba.tempo_bomba = 3.0;//tempo de duração da bomba
    //paredes indestrutiveis
    PAREDES indestrutiveis;//cria a variavel para as paredes indestrutiveis
    indestrutiveis.qntd = 0; //inicia a contagem das paredes em 0
    //paredes destrutiveis
    PAREDES destrutiveis;
    destrutiveis.qntd = 0;
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
                          "W       DDDDDDDDDD        W",
                          "W  P  WWWWWWWWWWWWWWW  P  W",
                          "W                         W",
                          "W  P  WWWWWWWWWWWWWWW  P  W",
                          "W                         W",
                          "WWWW      DDDDDDD      WWWW",
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
    initJogo(mapa, &indestrutiveis, &pocao, &destrutiveis);//faz uma leitura do mapa e devolve a posicao inicial do jogador
    SetTargetFPS(60);

    //main game loop
    while (!WindowShouldClose())
    {
        bomba.bomba = false;//começa sem colocar bomba

        atualiza_timer(&tempo_real);//atualiza o timer

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
            if(info.bombas>0){
                if(IsKeyPressed(KEY_B)){
                    inicia_timer(&tempo_real, bomba.tempo_bomba);//começa a contagem para a explosao
                    bomba.pos_x_bomba = jogador.pos_dinamicaPersX;
                    bomba.pos_y_bomba = jogador.pos_dinamicaPersY;//lê as posicoes de onde a bomba será plantada
                    info.bombas -= 1;//diminui uma bomba do arsenal
                }
            }

            if(!acabou_timer(&tempo_real)){//enquanto o timer nao acabou
                bomba.bomba = true;//a bomba esta plantada
            }

            if(bomba.bomba){
                explosao(&(info.vidas), mapa, jogador.pos_dinamicaPersX, jogador.pos_dinamicaPersY, bomba.pos_x_bomba, bomba.pos_y_bomba);
            }


            if(podeMover(jogador, indestrutiveis, destrutiveis)== 0){//se puder mover
                if(moveParaPocao(jogador, &pocao)== 0){//se for para cima de uma pocao
                    colhePocao(&jogador, mapa, &(info.pontuacao));//realiza o processo de captura da pocao
                    pocao.qntdP -= 1;
                    initJogo(mapa, &indestrutiveis, &pocao, &destrutiveis);
                }else{
                    jogador.pos_dinamicaPersX += jogador.persdx;
                    jogador.pos_dinamicaPersY += jogador.persdy;//caso contrario, só desloca o personagem
                }
            }

        }

        desenhaJogo(mapa, jogador.pos_dinamicaPersX, jogador.pos_dinamicaPersY, menu, info, bomba);//reproduz o grafico

    }

    CloseWindow();

    return 0;
}
