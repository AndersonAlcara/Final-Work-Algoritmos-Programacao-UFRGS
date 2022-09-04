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
    int posicoes_X[308];//vetor para as coordenadas X das paredes // 308 caso tivessemos apenas paredes em todo o mapa
    int posicoes_Y[308];//vetor para as coordenadas Y das paredes
    int qntd;//conta quantas paredes existem no mapa
}PAREDES;

typedef struct{
    int pos_dinamicaPersX;//posicao do personagem no eixc X
    int pos_dinamicaPersY;//posicao do personagem no eixo Y
    int persdx;//deslocamento do personagem no eixo X
    int persdy;//deslocamento do personagem no eixo Y
}PERSONAGEM;

typedef struct{
    int posicoes_Xp[308];//vetor para as coordenadas X das poçoes 'P'//308 caso tivessemos apenas poçoes em todo o mapa
    int posicoes_Yp[308];//vetor para as coordenadas Y das pocoes 'P'
    int qntdP;//conta quantas pocoes existem no mapa
}CONSUMIVEL;

typedef struct{
    int pontuacao;//calcula o SCORE do player
    int vidas;//calcula a quantidade de vidas do player
    int nivel;//n sei ainda
    int bombas;//numero de bombas no arsenal do jogador
}CONTADORES;

typedef struct{
    bool bomba;//verifica se a bomba esta ativa ou nao
    int pos_x_bomba;//guarda a posicao x de quando a bomba foi plantada
    int pos_y_bomba;//guarda a posicao y de quando a bomba foi plantada
    float contador_frames;//conta frames por segundo
    bool timer;//o timer que calcula o periodo em que a bomba está ativa, e tb de quando ela explode
}BOMBA;

typedef struct{
    int posX, posY;
    int deslX, deslY;
}MONSTROS;

//----------------------------------------------------------------------------


//Protótipos------------------------------------------------------------------
void inicializaMonstro();
void inicializaSer();
void colhePocao();
void posicaoJogador();
void initJogo();
void desenhaMapaEstatico();
void desenhaJogo();
int  podeMover();
int  moveParaPocao();
void explosao();
//----------------------------------------------------------------------------


//Funcão Principal------------------------------------------------------------
int main()
{
    //variaveis auxiliares
    int i, j, k;

    //bomba
    BOMBA bomba[3];
    for(i = 0; i < 3; i++){
        bomba[i].bomba = false;
        bomba[i].timer = false;
    }
    bool explode = false;
    int danoX[5];
    int danoY[5];

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

    //quantidade de monstros e seres
    int contaseres = 0;
    int contamonstros = 0;

    //menu
    bool menu = false;//comeca com o menu fechado

    //mapa
    char mapa[11][28] =  {"WWWWWWWWWWWWWWWWWWWWWWWWWWW",
                          "W PP  WWWW PPPPP WWWW DP  W",
                          "WP         WWWWW       D  W",
                          "W  P    DDDDDDDDDDD       W",
                          "W  PP  DDDDDDDDDDDDDDDDP  W",
                          "W      DDD          DDDDD W",
                          "W  P  WWWWWWWWWWWWWWW  P  W",
                          "W            KM          JW",
                          "WWWW WWW  DDDDDDD  M K WWWW",
                          "WWWWWPP  WWWWWWWWW  PPWWWWW",
                          "WWWWWWWWWWWWWWWWWWWWWWWWWWW"};

    //atualiza contadores
    CONTADORES info;
    info.bombas = 3;
    info.nivel = 10; //??
    info.pontuacao = 0;
    info.vidas = 3;


    InitWindow(LARGURA, ALTURA, "O jogo");
    posicaoJogador(mapa, &(jogador.pos_dinamicaPersX), &(jogador.pos_dinamicaPersY), &contaseres, &contamonstros);

    MONSTROS seres[contaseres], monstros[contamonstros];

    inicializaSer(&seres, mapa);
    inicializaMonstro(&monstros, mapa);

    initJogo(mapa, &indestrutiveis, &pocao, &destrutiveis);
    SetTargetFPS(60);

    //main game loop
    while (!WindowShouldClose())
    {
        explode = false;//explosao desligada

        if(IsKeyPressed(KEY_TAB)){
            menu = !menu;//se TAB for pressionado abre um menu
        }

        if(menu){
            //algumas opçoes do menu (ainda não configuradas)
        }else{

            jogador.persdx = 0;
            jogador.persdy = 0;//zera os deslocamentos

            if(IsKeyPressed(KEY_RIGHT))jogador.persdx = ARESTA;
            if(IsKeyPressed(KEY_LEFT))jogador.persdx =- ARESTA;
            if(IsKeyPressed(KEY_UP))jogador.persdy =- ARESTA;
            if(IsKeyPressed(KEY_DOWN))jogador.persdy = ARESTA;//analisa o deslocamento

            if(info.bombas>0){//se no arsenal tiver mais do que uma bomba, é possivel plantar uma bomba
                if(IsKeyPressed(KEY_B)){
                    if(bomba[0].bomba == false){//se a bomba '0' não estiver plantada, é possivel plantá-la
                        bomba[0].contador_frames = 0;//zera o contador de quadros
                        bomba[0].timer = true;//sinaliza para o timer começar a contar
                        bomba[0].pos_x_bomba = jogador.pos_dinamicaPersX;
                        bomba[0].pos_y_bomba = jogador.pos_dinamicaPersY;//lê as posicoes de onde a bomba será plantada
                        info.bombas -= 1;//diminui uma bomba do arsenal
                    }
                    if((bomba[1].bomba == false)&&(bomba[0].bomba == true)){//se a bomba '1' não estiver plantada E a bomba '0' já estiver plantada, é possivel plantá-la
                        bomba[1].contador_frames = 0;//zera o contador de quadros
                        bomba[1].timer = true;//sinaliza para o timer começar a contar
                        bomba[1].pos_x_bomba = jogador.pos_dinamicaPersX;
                        bomba[1].pos_y_bomba = jogador.pos_dinamicaPersY;//lê as posicoes de onde a bomba será plantada
                        info.bombas -= 1;//diminui uma bomba do arsenal
                    }
                    if((bomba[2].bomba == false)&&(bomba[0].bomba == true)&&(bomba[1].bomba == true)){//se a bomba '2' não estiver plantada E a bomba '0' e '1' já estiverem plantadas, é possivel plantá-la
                        bomba[2].contador_frames = 0;//zera o contador de quadros
                        bomba[2].timer = true;//sinaliza para o timer começar a contar
                        bomba[2].pos_x_bomba = jogador.pos_dinamicaPersX;
                        bomba[2].pos_y_bomba = jogador.pos_dinamicaPersY;//lê as posicoes de onde a bomba será plantada
                        info.bombas -= 1;//diminui uma bomba do arsenal
                    }
                }
            }

            for(i = 0; i < 3; i++){//procura se o timer de alguma das 3 bombas foi inicializado
                if(bomba[i].timer==true){//se sim,
                    bomba[i].contador_frames++;//comeca o contador de quadros daquela bomba
                }
            }

            for(i = 0; i < 3; i++){//procura se o contador de alguma das 3 bombas chegou ao final
                if(bomba[i].contador_frames<180 && bomba[i].contador_frames>1){//enquanto o contador não chegar a 180 frames, ou seja, 3 segundos
                    bomba[i].bomba = true;//a bomba esta plantada
                }
                if(bomba[i].contador_frames==180){//quando chega ao final
                    explosao(&(info.vidas), mapa, jogador.pos_dinamicaPersX, jogador.pos_dinamicaPersY, bomba[i].pos_x_bomba, bomba[i].pos_y_bomba, danoX, danoY, &(info.pontuacao));//cria a explosao de dano
                    bomba[i].bomba = false;//a bomba passa a não existir mais
                    explode = true;//sinaliza que ocorre uma explosao grafica
                    info.bombas+=1;//re-abastece o arsenal
                        for(i = 0; i < 11; i++){//percorre a matriz mapa e verifica onde há paredes destrutiveis E FUTURAMENTE MONSTROS
                            for(j = 0; j < 28; j++){
                                if(mapa[i][j] == 'D'){
                                    for(k = 0; k < 5; k++)
                                        if((i == (danoY[k]/ARESTA)) && (j == (danoX[k]/ARESTA))){//compara as posicoes das paredes destrutiveis com o RAIO DE DANO DA BOMBA que é em forma de '+'
                                            mapa[i][j] = ' ';//se existir, substitui aquela parede por um espaço vazio
                                            info.pontuacao+=10; //ganha 10 pontos por parede destruida
                                            initJogo(mapa, &indestrutiveis, &pocao, &destrutiveis);//reinicializa o mapa, para guardar as alteraçoes
                                        }
                                    }
                            }

                        }
                }
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

        desenhaJogo(mapa, jogador, menu, info, bomba, danoX, danoY, explode, seres, monstros, contamonstros, contaseres);//reproduz o grafico

    }

    CloseWindow();

    return 0;
}
