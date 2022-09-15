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
    bool bomba;//verifica se a bomba esta ativada ou nao
    int pos_x_bomba;//guarda a posicao x de quando a bomba foi plantada
    int pos_y_bomba;//guarda a posicao y de quando a bomba foi plantada
    float contador_frames;//conta frames por segundo
    int contador_de_explosao;
    bool explosao;
    bool timer;//o timer que calcula o periodo em que a bomba está ativa, e tb de quando ela explode
}BOMBA;

typedef struct{
    int desl;
    int qtd_passos;
    char direcao_desl;
    int posX, posY;
    bool vivo;
}MONSTROS;
//----------------------------------------------------------------------------
void quantosSeres(int *contaseres, char mapa[][28])
{
    int linha, coluna;
    //descobre quantos monstros existem no mapa
    for(linha = 0; linha < 11; linha++)
        for(coluna = 0; coluna < 28; coluna++)
                if(mapa[linha][coluna] == 'K'){
                    *contaseres+=1;
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
                    if(seres[i].vivo==true){
                        seres[i].posX = coluna;
                        seres[i].posY = linha;
                        i++;
                    }
                }

    // inicializar qtd passos em zero
    for(i = 0; i < contaseres; i++)
        seres[i].qtd_passos = 0;

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

void iniMonstros(MONSTROS monstros[], int contamonstros, char mapa[][28]){

    int linha, coluna;
    int i;
    i = 0;
    // acha posicao inicial x e y de cada monstro
    for(linha = 0; linha < 11; linha++)
        for(coluna = 0; coluna < 28; coluna++)
            if(mapa[linha][coluna] == 'M'){
                if(monstros[i].vivo==true){
                    monstros[i].posX = coluna;
                    monstros[i].posY = linha;
                    i++;
                }
            }

    // inicializar qtd passos em zero
    for(i = 0; i < contamonstros; i++)
        monstros[i].qtd_passos = 0;
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

bool canMove(int pos_x, int pos_y, int desl, char mapa[][28], char direcao, BOMBA bomba[]){
    bool move;
    int i;
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
    for(i = 0; i < 2; i++){
        if((pos_y*ARESTA == bomba[i].pos_y_bomba)&&(pos_x*ARESTA == bomba[i].pos_x_bomba))
            move = false;
    }
    return move;
}

void moveCriaturas(MONSTROS monstros[], MONSTROS seres[], int contamonstros, int contaseres, char mapa[][28], BOMBA bomba[])
{
     int i;
     //char direcao;
     //int desl;

     // MONSTROS
     for(i = 0; i < contamonstros; i++){
        // gera direcao de movimento do monstro i
        if(monstros[i].vivo==true){
            if(monstros[i].qtd_passos == 0){
                monstros[i].direcao_desl = geraDirecao();
                monstros[i].desl = geraDeslocamento();
            }
            // verifica se pode mover e se qtdpassos está de acordo
            if(canMove(monstros[i].posX, monstros[i].posY, monstros[i].desl, mapa, monstros[i].direcao_desl, bomba)&&(monstros[i].qtd_passos < 5)){
                if(monstros[i].direcao_desl == 'x')
                    monstros[i].posX = monstros[i].posX + monstros[i].desl;
                else
                    monstros[i].posY = monstros[i].posY + monstros[i].desl;
                monstros[i].qtd_passos = monstros[i].qtd_passos + 1;
            }
            else // se nao estiver de acordo zera contador de passos e gera novo deslocamento lá em cima
                monstros[i].qtd_passos = 0;
        }
     }
     // SERES
     for(i = 0; i < contaseres; i++){
        // gera direcao de movimento do ser i
        if(seres[i].vivo==true){
            if(seres[i].qtd_passos == 0){
                seres[i].direcao_desl = geraDirecao();
                seres[i].desl = geraDeslocamento();
            }
            if(canMove(seres[i].posX, seres[i].posY, seres[i].desl, mapa, seres[i].direcao_desl, bomba)&&(seres[i].qtd_passos < 5)){
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
}

void posicaoJogador(char mapa[][28], int* pos_dinamicaPersX, int* pos_dinamicaPersY)
{
    int x = 0, y = 0;
    int i, j;

    for(i = 0; i < 11; i++){//percorre a matriz mapa e identifica a posicao inicial do jogador, onde há paredes indestrutiveis e quantas sao elas
        for(j = 0; j < 28; j++){
            if(mapa[i][j] == 'J'){
                *pos_dinamicaPersX = x;
                *pos_dinamicaPersY = y;//guarda a posicao inicial do jogador
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

void desenhaJogo(char mapa[][28], PERSONAGEM jogador, int menu, CONTADORES info, BOMBA bomba[], int danoX[], int danoY[], MONSTROS seres[], MONSTROS monstros[], int contamonstros, int contaseres)
{
    int i;
    //faz qualquer atualização grafica
    BeginDrawing();
    ClearBackground(LIGHTGRAY);
    desenhaMapaEstatico(mapa);//desenha o mapa
    DrawRectangle(jogador.pos_dinamicaPersX, jogador.pos_dinamicaPersY, ARESTA, ARESTA, BLUE);//desenha o personagem

    for(i = 0; i < contaseres; i++){
        if(seres[i].vivo==true)
            DrawRectangle(seres[i].posX*ARESTA, seres[i].posY*ARESTA, ARESTA, ARESTA, ORANGE);//desenha os seres
    }

    for(i = 0; i < contamonstros; i++){
        if(monstros[i].vivo==true)
            DrawRectangle(monstros[i].posX*ARESTA, monstros[i].posY*ARESTA, ARESTA, ARESTA, RED);//desenha os monstros
    }

    for(i = 0; i < 3; i++){//verifica se alguma das 3 bombas está plantada
        if(bomba[i].bomba==true){//se estiver
            DrawRectangle(bomba[i].pos_x_bomba, bomba[i].pos_y_bomba, ARESTA, ARESTA, YELLOW);//desenha uma bomba amarela
        }
        if(bomba[i].explosao == true){//se estiver em sua fase de explosao
            for(i = 0; i < 5; i++){
                DrawRectangle(danoX[i], danoY[i], ARESTA, ARESTA, BROWN);//desenha um '+' rapidamente, mostrando o raio de dano
            }
        }
    }
    if(menu){//por enquanto só exibe, as opcoes nao sao funcionais ainda
        DrawRectangle(40, 40, 460, 140, BEIGE);
        DrawText("MENU", 45, 45, 30, BLACK);
        DrawText("(N)Novo Jogo", 210, 50, 20, BLACK);
        DrawText("(C)Carregar Jogo", 210, 75, 20, BLACK);
        DrawText("(S)Salvar Jogo", 210, 100, 20, BLACK);
        DrawText("(Q)Sair do Jogo", 210, 125, 20, BLACK);
        DrawText("(V)Voltar", 210, 150, 20, BLACK);
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

void explosao(int *vidas, char mapa[][28], int posJogadorX, int posJogadorY, int posBombaX, int posBombaY, int danoX[], int danoY[], int *pontuacao, bool* perdeVida, MONSTROS monstro[], int contamonstros, int *serCapturado)
{
    int i, k, achou;
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
            *perdeVida = true;
            if(*pontuacao>=100)
                *pontuacao -= 100;//se tiver pontos suficientes para perder, perde
            else
                if(*pontuacao<100)//se não tiver pontos suficientes, zera o score, visando não torná-lo negativo
                    *pontuacao = 0;
        }
    }

    for(k = 0; k < 5; k++){
        for(i = 0; i < contamonstros; i++){
            if(monstro[i].vivo == true)
                if((danoX[k]== ((monstro[i].posX)*ARESTA)) && (danoY[k]== (monstro[i].posY)*ARESTA)){
                    achou = i;
                    *pontuacao += 15; //(?)
                }
        }
    }
    *serCapturado = achou;

}

void colhePocao(PERSONAGEM *jogador, char mapa[][28], CONTADORES *info, int pontos)
{
    int j, i;

    jogador->pos_dinamicaPersX += jogador->persdx;
    jogador->pos_dinamicaPersY += jogador->persdy;//primeiro, atualiza a posicao do jogador

    j = jogador->pos_dinamicaPersX/ARESTA;
    i = jogador->pos_dinamicaPersY/ARESTA;//Se as posicoes dinamicas são o produto da aresta pela posicao na matriz caracter, logo as posicoes na matriz caracter serao a divisao das posiçoes dinamicas pela aresta.

    mapa[i][j] = ' ';//onde há pocao e o está em cima dessa posicao, nossa matriz caracter tera esse 'P' trocado por um ' ', ou seja, irá liberar um espaço de livre.
    info->pontuacao = info->pontuacao + pontos;//contabiliza 50 pontos por pocao pega
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

int moveParaSer(PERSONAGEM jogador, MONSTROS seres[], int contaseres, char mapa[][28], int* serCapturado)
{
    //ve se o personagem vai ocupar as  mesmas coordenadas de uma pocao
    int colidiu = 1;
    int i, achou;
    for(i = 0; i < contaseres; i++){
        if(((jogador.pos_dinamicaPersX + jogador.persdx)== seres[i].posX*ARESTA)&&((jogador.pos_dinamicaPersY +jogador.persdy)== seres[i].posY*ARESTA)){//
            colidiu = 0;//vai ocupar
            achou = i;
        }
    }
    *serCapturado = achou;
    return colidiu;
}

void funcBomba(BOMBA bomba[], int* num_deBombas, int qualBomba, PERSONAGEM jogador)
{
    bomba[qualBomba].contador_frames = 0;//zera o contador de quadros
    bomba[qualBomba].timer = true;//sinaliza para o timer começar a contar
    bomba[qualBomba].pos_x_bomba = jogador.pos_dinamicaPersX;
    bomba[qualBomba].pos_y_bomba = jogador.pos_dinamicaPersY;//lê as posicoes de onde a bomba será plantada
    *num_deBombas-= 1;//diminui uma bomba do arsenal
}


