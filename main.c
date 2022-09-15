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
    int nivel;//mostra em qual nivel o jogador se encontra
    int bombas;//numero de bombas no arsenal do jogador
}CONTADORES;

typedef struct{
    bool bomba;//verifica se a bomba esta ativada ou nao
    int pos_x_bomba;//guarda a posicao x de quando a bomba foi plantada
    int pos_y_bomba;//guarda a posicao y de quando a bomba foi plantada
    float contador_de_bomba;//conta frames por segundo
    int contador_de_explosao;
    bool explosao;
    bool timer;//o timer que calcula o periodo em que a bomba está ativa, e tb de quando ela explode
}BOMBA;

typedef struct{
    int desl;//direcao do movimento
    int qtd_passos;//conta os passos de cada monstro/ser
    char direcao_desl;//qual o eixo de deslocamento dos monstros
    int posX, posY;//posicoes dos monstros/seres
    bool vivo;
}MONSTROS;

typedef struct{
    MONSTROS monstros[];
    MONSTROS seres[];
    BOMBA bomba[3];
    CONTADORES info;
    PAREDES destrutiveis
    PAREDES indestrutiveis
    PERSONAGEM jogador;
    CONSUMIVEL pocao;
}ESTADO

//----------------------------------------------------------------------------

//Protótipos------------------------------------------------------------------
void funcBomba();
void quantosSeres();
void quantosMonstros();
void iniMonstros();
void iniSeres();
int moveParaSer();
bool canMove();
int geraDeslocamento();
char geraDirecao();
void moveCriaturas();
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

    //bônus
    bool vida_extra = false;

    //bomba
    BOMBA bomba[3];
    for(i = 0; i < 3; i++){
        bomba[i].bomba = false;
        bomba[i].timer = false;
        bomba[i].explosao = false;
    }

    //dano
    int danoX[5];
    int danoY[5];
    bool perdeVida = false;
    bool deu_dano = false;

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

    //monstros e seres
    int contaseres = 0;
    int contamonstros = 0;
    int serCapturado;
    int contador_de_mov_criatura = 0;

    //menu
    bool menu = false;//comeca com o menu fechado
    bool novoJogo = false;

    //mapa
    char mapa[11][28];
    bool trocaNivel = true;

    //atualiza contadores
    CONTADORES info;
    info.bombas = 3;
    info.pontuacao = 0;
    info.vidas = 3;
    info.nivel = 0;

    InitWindow(LARGURA, ALTURA, "O jogo");
    SetTargetFPS(60);

    //main game loop
    while (!WindowShouldClose()&&info.vidas>0)
    {
        if(trocaNivel == true){
            info.nivel++;//já começa no nivel '1'
            trocaNivel = false;//já torna falso, para a condição não ficar se repetindo durante o while
            char nivel[11];

            //OPERA O ARQUIVO
            FILE* fase;
            sprintf(nivel, "nivel%d.txt", info.nivel);//define o nome do arquivo a partir do contador de niveis
            fase = fopen(nivel, "r");
            if(fase == NULL){
                printf("Erro na abertura do arquivo.");
                CloseWindow();
            }else{
                for(i = 0; i < 11; i++)
                    for(j = 0; j < 28; j++)
                        mapa[i][j] = getc(fase);//passa a matriz de caracteres do arquivo para uma matriz variavel do programa
            }
            fclose(fase);

            //REINICIA
            zereEstado(estadoZero);
             contaseres = 0;
             contamonstros = 0;
             for(i = 0; i < 3; i++){
                bomba[i].bomba = false;
                bomba[i].timer = false;
                bomba[i].contador_de_bomba = 0;
             }
             info.bombas = 3;

             indestrutiveis.qntd = 0;
             destrutiveis.qntd = 0;
             pocao.qntdP = 0;
             contador_de_mov_criatura = 0;

             posicaoJogador(mapa, &(jogador.pos_dinamicaPersX), &(jogador.pos_dinamicaPersY));//descobre coordenadas do jogador e o número de monstros/seres
             quantosSeres(&contaseres, mapa);
             quantosMonstros(&contamonstros, mapa);

             MONSTROS seres[contaseres], monstros[contamonstros];//sabendo já o número de monstros/seres, é possível definir o limite dos vetores

             for(i = 0; i < contamonstros; i++){
                monstros[i].vivo = true;
             }
             for(i = 0; i < contaseres; i++){
                seres[i].vivo = true;
             }

             iniSeres(seres, contaseres, mapa);
             iniMonstros(monstros, contamonstros, mapa);
             initJogo(mapa, &indestrutiveis, &pocao, &destrutiveis);//inicializa paredes e poções

         }
          MONSTROS seres[contaseres], monstros[contamonstros];
           if(IsKeyPressed(KEY_V))//sai do menu
                menu = false;
        if(menu){
            if(IsKeyPressed(KEY_Q))//fecha o jogo
               CloseWindow();

            if(IsKeyPressed(KEY_N))//só não está funcionando para os monstros
            {
                 novoJogo = true;
                 info.nivel = 1;
                 trocaNivel = false;
                 FILE* fase;
                 char nivel[11];
                 sprintf(nivel, "nivel%d.txt", info.nivel);
                 fase = fopen(nivel, "r");
                 if(fase == NULL){
                    printf("Erro na abertura do arquivo.");
                    CloseWindow();
                 }else{
                    for(i = 0; i < 11; i++)
                        for(j = 0; j < 28; j++)
                            mapa[i][j] = getc(fase);
                 }
                 fclose(fase);

                 contaseres = 0;
                 contamonstros = 0;

                 indestrutiveis.qntd = 0;
                 destrutiveis.qntd = 0;
                 pocao.qntdP = 0;
                 contador_de_mov_criatura = 0;
                 info.pontuacao = 0;
                 info.vidas = 3;
                 info.bombas = 3;

                 posicaoJogador(mapa, &(jogador.pos_dinamicaPersX), &(jogador.pos_dinamicaPersY));//descobre coordenadas do jogador e o número de monstros/seres
                 quantosSeres(&contaseres, mapa);
                 quantosMonstros(&contamonstros, mapa);

                 MONSTROS seres[contaseres], monstros[contamonstros];//sabendo já o número de monstros/seres, é possível definir o limite dos vetores
                 for(i = 0; i < contamonstros; i++){
                    monstros[i].vivo = true;
                 }
                 for(i = 0; i < contaseres; i++){
                    seres[i].vivo = true;
                 }

                 iniSeres(seres, contaseres, mapa);
                 iniMonstros(monstros, contamonstros, mapa);
                 initJogo(mapa, &indestrutiveis, &pocao, &destrutiveis);//inicializa paredes e poções
            }

            if(IsKeyPressed(KEY_C))//AINDA NAO CONFIGURADO
            {

            }
            if(IsKeyPressed(KEY_S))//AINDA NAO CONFIGURADO
            {

            }

        }else{

            //TIMERS
            contador_de_mov_criatura++;//faz a contagem do tempo para a movimentação das criaturas

            //CONDIÇÕES
            if(IsKeyPressed(KEY_TAB))
                menu = true;//se TAB for pressionado abre um menu

            if(novoJogo == true){//para fechar o menu após a escolha de NOVO JOGO
                menu = false;
                novoJogo = false;
            }
            for(i = 0; i < 3; i++){
                if(bomba[i].contador_de_explosao==40&&bomba[i].explosao==true){
                    bomba[i].explosao = false;
                }
            }
            //REINICIA OS DESLOCAMENTOS DO PERSONAGEM
            jogador.persdx = 0;
            jogador.persdy = 0;

            //IDENTIFICA OS COMANDOS DE MOVIMENTO DO JOGADOR
            if(IsKeyPressed(KEY_D)||IsKeyPressed(KEY_RIGHT))jogador.persdx = ARESTA;
            if(IsKeyPressed(KEY_A)||IsKeyPressed(KEY_LEFT))jogador.persdx =- ARESTA;
            if(IsKeyPressed(KEY_W)||IsKeyPressed(KEY_UP))jogador.persdy =- ARESTA;
            if(IsKeyPressed(KEY_S)||IsKeyPressed(KEY_DOWN))jogador.persdy = ARESTA;

            //PARA PLANTAR BOMBAS
            if(info.bombas>0){//se no arsenal tiver mais do que uma bomba, é possivel plantar uma bomba
                if(IsKeyPressed(KEY_B)){
                    if(bomba[0].bomba == false)//se a bomba '0' não estiver plantada, é possivel plantá-la
                        funcBomba(bomba, &info.bombas, 0, jogador);

                    if((bomba[1].bomba == false)&&(bomba[0].bomba == true))//se a bomba '1' não estiver plantada E a bomba '0' já estiver plantada, é possivel plantá-la
                        funcBomba(bomba, &info.bombas, 1, jogador);

                    if((bomba[2].bomba == false)&&(bomba[0].bomba == true)&&(bomba[1].bomba == true))//se a bomba '2' não estiver plantada E a bomba '0' e '1' já estiverem plantadas, é possivel plantá-la
                        funcBomba(bomba, &info.bombas, 2, jogador);
                }
            }

             for(i = 0; i < 3; i++){//procura se a explosao de alguma das bombas foi iniciada
                if(bomba[i].explosao==true){//se sim,
                    bomba[i].contador_de_explosao++;//comeca o contador de quadros daquela bomba
                }
            }


            for(i = 0; i < 3; i++){//procura se o timer de alguma das 3 bombas foi inicializado
                if(bomba[i].timer==true){//se sim,
                    bomba[i].contador_de_bomba++;//comeca o contador de quadros daquela bomba
                }
            }



            for(i = 0; i < 3; i++){//procura se o contador de alguma das 3 bombas chegou ao final
                if(bomba[i].contador_de_bomba<180 && bomba[i].contador_de_bomba>1){//enquanto o contador não chegar a 180 frames, ou seja, 3 segundos
                    bomba[i].bomba = true;//a bomba esta plantada
                }
                if(bomba[i].contador_de_bomba==180){//quando chega ao final
                    explosao(&(info.vidas), mapa, jogador.pos_dinamicaPersX, jogador.pos_dinamicaPersY, bomba[i].pos_x_bomba, bomba[i].pos_y_bomba, danoX, danoY, &(info.pontuacao), &perdeVida, monstros, contamonstros, &serCapturado);//cria a explosao de dano
                    monstros[serCapturado].vivo = false;
                    bomba[i].bomba = false;//a bomba passa a não existir mais
                    bomba[i].explosao = true;//sinaliza que ocorre uma explosao grafica
                    bomba[i].contador_de_explosao = 0;
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

            if(podeMover(jogador, indestrutiveis, destrutiveis, bomba)== 0){//se puder mover
                if(moveParaPocao(jogador, &pocao)== 0){//se for para cima de uma pocao
                    int pontos = 50;
                    colhePocao(&jogador, mapa, &(info.pontuacao), pontos);//realiza o processo de captura da pocao
                    pocao.qntdP -= 1;//diminui uma pocao do vetor de pocoes
                    initJogo(mapa, &indestrutiveis, &pocao, &destrutiveis);//reinicializa o mapa
                }
                else if(moveParaSer(jogador, seres, contaseres, mapa, &serCapturado)== 0){
                    seres[serCapturado].vivo = false;
                    jogador.pos_dinamicaPersX += jogador.persdx;
                    jogador.pos_dinamicaPersY += jogador.persdy;
                }else if(moveParaSer(jogador, monstros, contamonstros, mapa, &serCapturado)== 0){
                    deu_dano = true;
                    jogador.pos_dinamicaPersX += jogador.persdx;
                    jogador.pos_dinamicaPersY += jogador.persdy;
                }else{
                    jogador.pos_dinamicaPersX += jogador.persdx;
                    jogador.pos_dinamicaPersY += jogador.persdy;//caso contrario, só desloca o personagem
                }
            }
            for(i = 1; i < 10; i++){
                if((info.pontuacao>=(1000*i))&&(vida_extra==false)){//quando a pontuação for multipla de 1000, ganha uma vida extra
                    vida_extra = !vida_extra;
                    info.vidas+=1;
                }
            }
            if(contador_de_mov_criatura%50==0)//movimenta as criaturas em multiplos de 50
                moveCriaturas(monstros, seres, contamonstros, contaseres, mapa, bomba);

            for(i = 0; i < contamonstros; i ++){
                if(monstros[i].vivo==true)
                    if(deu_dano == true){
                        info.vidas-=1;
                        deu_dano = false;
                    }
            }


        }
        //GRÁFICOS
        desenhaJogo(mapa, jogador, menu, info, bomba, danoX, danoY, seres, monstros, contamonstros, contaseres);
    }

    //se o laço for finalizado por motivos de falta de vidas, escreve "game over" e fecha a janela
    if(info.vidas==0){
        BeginDrawing();
        ClearBackground(BLACK);
        DrawText("GAME OVER", 90, 70, 60, RED);
        EndDrawing();
        _sleep(2000);
    }

    //se for apenas clicado "ESC" a janela só fecha
    CloseWindow();
    return 0;
}

