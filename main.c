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
#define PONTUACAO_MAX 10
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
    float contador_frames;//conta frames por segundo
    bool timer;//o timer que calcula o periodo em que a bomba está ativa, e tb de quando ela explode
}BOMBA;

typedef struct{
    int desl;//direcao do movimento
    int qtd_passos;//conta os passos de cada monstro/ser
    char direcao_desl;//qual o eixo de deslocamento dos monstros
    int posX, posY;//posicoes dos monstros/seres
}MONSTROS;

//----------------------------------------------------------------------------

//Protótipos------------------------------------------------------------------
void quantosSeres();
void quantosMonstros();
void iniMonstros(MONSTROS monstros[], int contamonstros, char mapa[][28]);
void iniSeres(MONSTROS seres[], int contaseres, char mapa[][28]);
int moveParaSer();
bool canMove(int pos_x, int pos_y, int desl, char mapa[][28], char direcao);
int geraDeslocamento();
char geraDirecao();
void moveCriaturas(MONSTROS monstros[], MONSTROS seres[], int contamonstros, int contaseres, char mapa[][28]);
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
    bool perdeVida = false;

    //pocao
    CONSUMIVEL pocao;
    pocao.qntdP = 0;

    //quantidade de monstros e seres
    int contaseres = 0;
    int contamonstros = 0;

    //menu
    bool menu = false;//comeca com o menu fechado
    bool novoJogo = false;

    //mapa
    char mapa[11][28];
    int contador_de_niveis = 1;
    bool trocaNivel = true;

    //atualiza contadores
    CONTADORES info;
    info.bombas = 3;
    info.pontuacao = 0;
    info.vidas = 3;
    int contador_de_mov_criatura = 0;

    InitWindow(LARGURA, ALTURA, "O jogo");
    SetTargetFPS(60);

    //main game loop
    while (!WindowShouldClose()&&info.vidas>0)
    {
         if(trocaNivel == true){//está funcionando legal

             trocaNivel = false;//já torna falso, para a condição não ficar se repetindo durante o while
             info.nivel = contador_de_niveis;//atualiza o visor de nível a partir do contador de níveis

             FILE* fase;

             char nivel[11];
             sprintf(nivel, "nivel%d.txt", contador_de_niveis);//define o nome do arquivo a partir do contador de niveis
             fase = fopen(nivel, "r");
             if(fase == NULL)
                printf("Erro na abertura do arquivo.");
             else{
                for(i = 0; i < 11; i++)
                    for(j = 0; j < 28; j++)
                        mapa[i][j] = getc(fase);//passa a matriz de caracteres do arquivo para uma matriz variavel do programa
             }
             fclose(fase);
            //reinicializações
             contaseres = 0;
             contamonstros = 0;
             for(i = 0; i < 3; i++){
                bomba[i].bomba = false;
                bomba[i].timer = false;
                bomba[i].contador_frames = 0;
             }
             info.bombas = 3;
             explode = false;
             indestrutiveis.qntd = 0;
             destrutiveis.qntd = 0;
             pocao.qntdP = 0;
             contador_de_mov_criatura = 0;

             posicaoJogador(mapa, &(jogador.pos_dinamicaPersX), &(jogador.pos_dinamicaPersY));//descobre coordenadas do jogador e o número de monstros/seres
             quantosSeres(&contaseres, mapa);
             quantosMonstros(&contamonstros, mapa);

             MONSTROS seres[contaseres], monstros[contamonstros];//sabendo já o número de monstros/seres, é possível definir o limite dos vetores

             iniSeres(seres, contaseres, mapa);
             iniMonstros(monstros, contamonstros, mapa);
             initJogo(mapa, &indestrutiveis, &pocao, &destrutiveis);//inicializa paredes e poções

         }

        MONSTROS seres[contaseres], monstros[contamonstros];

        explode = false;//explosao desligada
        contador_de_mov_criatura++;

        if(IsKeyPressed(KEY_TAB))
            menu = true;//se TAB for pressionado abre um menu

        if(IsKeyPressed(KEY_V))//sai do menu
            menu = false;

        if(novoJogo == true){//variavel controle
            menu = false;
            novoJogo = false;
        }


        if(IsKeyPressed(KEY_J)){
            trocaNivel = true;
            contador_de_niveis++;///TEMPORARIO, FUNCAO DE TESTE PARA AVANÇAR NÍVEIS
        }

        if(menu){
            if(IsKeyPressed(KEY_Q))//fecha o jogo
               CloseWindow();

            if(IsKeyPressed(KEY_N))//só não está funcionando para os monstros
            {
                 novoJogo = true;
                 contador_de_niveis = 1;
                 trocaNivel = false;
                 FILE* fase;
                 char nivel[11];
                 info.nivel = contador_de_niveis;
                 sprintf(nivel, "nivel%d.txt", contador_de_niveis);
                 fase = fopen(nivel, "r");
                 if(fase == NULL)
                    printf("Erro na abertura do arquivo.");
                 else{
                    for(i = 0; i < 11; i++)
                        for(j = 0; j < 28; j++)
                            mapa[i][j] = getc(fase);
                 }
                 fclose(fase);

                 contaseres = 0;
                 contamonstros = 0;
                 explode = false;
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

            jogador.persdx = 0;
            jogador.persdy = 0;//zera os deslocamentos

            if(IsKeyPressed(KEY_D)||IsKeyPressed(KEY_RIGHT))jogador.persdx = ARESTA;
            if(IsKeyPressed(KEY_A)||IsKeyPressed(KEY_LEFT))jogador.persdx =- ARESTA;
            if(IsKeyPressed(KEY_W)||IsKeyPressed(KEY_UP))jogador.persdy =- ARESTA;
            if(IsKeyPressed(KEY_S)||IsKeyPressed(KEY_DOWN))jogador.persdy = ARESTA;//analisa o deslocamento

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
                    explosao(&(info.vidas), mapa, jogador.pos_dinamicaPersX, jogador.pos_dinamicaPersY, bomba[i].pos_x_bomba, bomba[i].pos_y_bomba, danoX, danoY, &(info.pontuacao), &perdeVida);//cria a explosao de dano
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

            if(podeMover(jogador, indestrutiveis, destrutiveis, bomba)== 0){//se puder mover
                if(moveParaPocao(jogador, &pocao)== 0){//se for para cima de uma pocao
                    int pontos = 50;
                    colhePocao(&jogador, mapa, &(info.pontuacao), pontos);//realiza o processo de captura da pocao
                    pocao.qntdP -= 1;//diminui uma pocao do vetor de pocoes
                    initJogo(mapa, &indestrutiveis, &pocao, &destrutiveis);//reinicializa o mapa
                }
                else if(moveParaSer(jogador, seres, contaseres, mapa)== 0){
                    int pontos = 100;
                    ////PRECISA SER ARRUMADO
                }else{
                    jogador.pos_dinamicaPersX += jogador.persdx;
                    jogador.pos_dinamicaPersY += jogador.persdy;//caso contrario, só desloca o personagem
                }
            }
            for(i = 1; i < PONTUACAO_MAX; i++){
                if((info.pontuacao>=(1000*i))&&(vida_extra==false)){//quando a pontuação for multipla de 1000, ganha uma vida extra
                    vida_extra = !vida_extra;
                    info.vidas+=1;
                }
            }
            if(contador_de_mov_criatura%50==0)//movimenta as criaturas em multiplos de 50
                moveCriaturas(monstros, seres, contamonstros, contaseres, mapa);
        }

        desenhaJogo(mapa, jogador, menu, info, bomba, danoX, danoY, explode, seres, monstros, contamonstros, contaseres);//reproduz o grafico

    }
    if(info.vidas==0){//se o laço for finalizado por motivos de falta de vidas, escreve "game over" e fecha a janela
        BeginDrawing();
        ClearBackground(BLACK);
        DrawText("GAME OVER", 90, 70, 60, RED);
        EndDrawing();
        _sleep(2000);
    }//se for apenas clicado "ESC" a janela só fecha
    CloseWindow();

    return 0;
}

void quantosSeres(int *contaseres, char mapa[][28]){
    int linha, coluna;
    //descobre quantos monstros existem no mapa
    for(linha = 0; linha < 11; linha++)
        for(coluna = 0; coluna < 28; coluna++)
                if(mapa[linha][coluna] == 'K'){
                    *contaseres+=1;
                }
}

void quantosMonstros(int *contamonstros, char mapa[][28]){
    int linha, coluna;
    //descobre quantos monstros existem no mapa
    for(linha = 0; linha < 11; linha++)
        for(coluna = 0; coluna < 28; coluna++)
                if(mapa[linha][coluna] == 'M'){
                    *contamonstros+=1;
                }
}

void iniSeres(MONSTROS seres[], int contaseres, char mapa[][28]){
    int linha, coluna;
    int i;
    i = 0;
    // acha posicao inicial x e y de cada ser
    for(linha = 0; linha < 11; linha++)
        for(coluna = 0; coluna < 28; coluna++)
                if(mapa[linha][coluna] == 'K'){
                    seres[i].posX = coluna;
                    seres[i].posY = linha;
                    i++;
                }

    // inicializar qtd passos em zero
    for(i = 0; i < contaseres; i++)
        seres[i].qtd_passos = 0;

}

void iniMonstros(MONSTROS monstros[], int contamonstros, char mapa[][28]){

    int linha, coluna;
    int i;
    i = 0;
    // acha posicao inicial x e y de cada monstro
    for(linha = 0; linha < 11; linha++)
        for(coluna = 0; coluna < 28; coluna++)
            if(mapa[linha][coluna] == 'M'){
                monstros[i].posX = coluna;
                monstros[i].posY = linha;
                i++;
            }

    // inicializar qtd passos em zero
    for(i = 0; i < contamonstros; i++)
        monstros[i].qtd_passos = 0;
}

 void moveCriaturas(MONSTROS monstros[], MONSTROS seres[], int contamonstros, int contaseres, char mapa[][28]){
     int i;
     //char direcao;
     //int desl;

     // MONSTROS
     for(i = 0; i < contamonstros; i++){
        // gera direcao de movimento do monstro i
        if(monstros[i].qtd_passos == 0){
            monstros[i].direcao_desl = geraDirecao();
            monstros[i].desl = geraDeslocamento();
        }
        // verifica se pode mover e se qtdpassos está de acordo
        if(canMove(monstros[i].posX, monstros[i].posY, monstros[i].desl, mapa, monstros[i].direcao_desl)&&(monstros[i].qtd_passos < 5)){
            if(monstros[i].direcao_desl == 'x')
                monstros[i].posX = monstros[i].posX + monstros[i].desl;
            else
                monstros[i].posY = monstros[i].posY + monstros[i].desl;
            monstros[i].qtd_passos = monstros[i].qtd_passos + 1;
        }
        else // se nao estiver de acordo zera contador de passos e gera novo deslocamento lá em cima
            monstros[i].qtd_passos = 0;
     }
     // SERES
     for(i = 0; i < contaseres; i++){
        // gera direcao de movimento do ser i
        if(seres[i].qtd_passos == 0){
            seres[i].direcao_desl = geraDirecao();
            seres[i].desl = geraDeslocamento();
        }
        if(canMove(seres[i].posX, seres[i].posY, seres[i].desl, mapa, seres[i].direcao_desl)&&(seres[i].qtd_passos < 5)){
            if(seres[i].direcao_desl == 'x')
                seres[i].posX = seres[i].posX + seres[i].desl;
            else
                seres[i].posY = seres[i].posY + seres[i].desl;
            seres[i].qtd_passos = seres[i].qtd_passos + 1;
        }
        else
            seres[i].qtd_passos = 0;
     }
}
// gera direcao em que monstros ou seres irao se mover na matriz/mapa
char geraDirecao(){

    char direcaoNaMatriz;
    int valorXY;

    valorXY = GetRandomValue(0,1);
    if(valorXY)
        direcaoNaMatriz = 'x';
    else
        direcaoNaMatriz = 'y';
    return direcaoNaMatriz;

}
// gera deslocamento, dado a direcao atual e o deslocamento atual
// retorna novo deslocamento
int geraDeslocamento(){
    int deslocamento;
    do{
        deslocamento = GetRandomValue(-1,1);
    }while(deslocamento == 0);
    return deslocamento;
}

bool canMove(int pos_x, int pos_y, int desl, char mapa[][28], char direcao){
    bool move;
    // assumimos inicialmente q pode mover
    move = true;
    if(direcao == 'x')
        pos_x = pos_x + desl;
    else // senao eh x eh y
        pos_y = pos_y + desl;
    if(mapa[pos_y][pos_x] == 'W')
        move = false;
    if(mapa[pos_y][pos_x] == 'D')
        move = false;
    return move;
}
