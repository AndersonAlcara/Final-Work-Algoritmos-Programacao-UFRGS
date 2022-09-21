//Bibliotecas------------------------------------------------------------------
#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
//----------------------------------------------------------------------------

//Constantes------------------------------------------------------------------
#define ARESTA   20
#define LARGURA  540//JANELA
#define ALTURA   260//JANELA
#define LINHAS 11//MAPA
#define COLUNAS 27//MAPA
//----------------------------------------------------------------------------

//Estruturas-------------------------------------------------------------------

//ESTRUTURA PARA PAREDES INDESTRUTÍVEIS/DESTRUTÍVEIS
typedef struct{
    int posicoes_X[308];//vetor para as coordenadas X das paredes 'W' ou 'D' // 308 caso tivessemos apenas paredes em todo o mapa
    int posicoes_Y[308];//vetor para as coordenadas Y das paredes 'W' ou 'D' // 308 caso tivessemos apenas paredes em todo o mapa
    int qntd;//conta quantas paredes existem no mapa
}PAREDES;

//ESTRUTURA PARA DADOS RELATIVOS AO JOGADOR
typedef struct{
    int pos_dinamicaPersX;//posicao do personagem no eixc X
    int pos_dinamicaPersY;//posicao do personagem no eixo Y
    int persdx;//deslocamento do personagem no eixo X
    int persdy;//deslocamento do personagem no eixo Y
}PERSONAGEM;

//ESTRUTURA PARA AS POÇÕES
typedef struct{
    int posicoes_Xp[308];//vetor para as coordenadas X das poçoes 'P' //308 caso tivessemos apenas poçoes em todo o mapa
    int posicoes_Yp[308];//vetor para as coordenadas Y das pocoes 'P' //308 caso tivessemos apenas poçoes em todo o mapa
    int qntdP;//conta quantas pocoes existem no mapa
}CONSUMIVEL;

//ESTRUTURA PARA CONTADORES DE INFORMAÇÕES
typedef struct{
    int pontuacao;//calcula o SCORE do player
    int vidas;//calcula a quantidade de vidas do player
    int nivel;//mostra em qual nivel o jogador se encontra
    int bombas;//numero de bombas no arsenal do jogador
}CONTADORES;

//ESTRUTURA PARA INFORMAÇÕES RELATIVAS À BOMBA E SUA EXPLOSÃO
typedef struct{
    bool bomba;//verifica se a bomba está ativada ou não
    int pos_x_bomba;//guarda a posicao x de quando a bomba foi plantada
    int pos_y_bomba;//guarda a posicao y de quando a bomba foi plantada
    float contador_de_bomba;//conta frames por segundo para a bomba
    int contador_de_explosao;//conta frames por segundo para a explosão
    bool explosao;//verifica se a explosão está ativada ou não
    bool timer;//verifica se deve ou não começar a contagem do tempo de existência da bomba/explosão
}BOMBA;

//ESTRUTURA PARA INFORMAÇÕES RELATIVAS AOS SERES E AOS MONSTROS
typedef struct{
    int desl;//direcao do movimento da criatura
    int qtd_passos;//conta os passos de cada criatura
    char direcao_desl;//qual o eixo de deslocamento das criaturas
    int posX, posY;//posicoes das criaturas
    bool vivo;//verifica se a criatura está viva ou não
}MONSTROS;

//ESTRUTURA GERAL, QUE CONTÉM OUTRAS ESTRUTURAS E VARIÁVEIS INDISPENSÁVEIS PARA CARREGAR OU SALVAR O JOGO
typedef struct{
    char mapa[11][27];//mapa interno do jogo
    BOMBA bomba[3];//bombas (3 no máximo no arsenal)
    MONSTROS seres[20];//coloca um limite arbitrário de '20' para os seres
    MONSTROS monstros[20];//coloca um limite arbitrário de '20' para os monstros
    CONTADORES info;
    CONSUMIVEL pocao;
    PERSONAGEM jogador;
    int contaseres;//NAO TENHO CERTEZA SE PRECISARIAM ESTAR DENTRO DA STRUCT
    int contamonstros;
    char nivel[11];//nome do arquivo texto (MAPA) em que o jogador se encontra
    PAREDES indestrutiveis;
    PAREDES destrutiveis;
}ESTADO;
//----------------------------------------------------------------------------

//Protótipos------------------------------------------------------------------
int  carregaJogo();
void leMapa();
int  salvaJogo();
void funcBomba();
void quantosSeres();
void quantosMonstros();
void iniMonstros();
void iniSeres();
int  moveParaSer();
bool canMove();
int  geraDeslocamento();
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
void iniciaVariaveisEstadoCarregado();
void variaveisParaProximaFase();
//----------------------------------------------------------------------------

//Funcão Principal------------------------------------------------------------
int main()
{
    //DECLARAÇÃO DE ALGUMAS VARIÁVEIS
    char arqbin[16] = "dadosSalvos.bin";//nome do arquivo binário para salvar e/ou carregar informações
    int i, j, k = 0;//variaveis auxiliares
    bool vida_extra = false; //bônus de vida
    int danoX[5];//raio de dano da bomba
    int danoY[5];//raio de dano da bomba
    bool perdeVida = false;//verifica se o jogador deve perder vida
    bool deu_dano = false;//verifica se o monstro deu dano
    int serCapturado;//número de ser que foi encostado pelo jogador
    int contador_de_mov_criatura = 0;//conta frames para decidir se a criatura deve mover ou não
    bool menu = false;//verifica se o 'menu' está aberto ou fechado
    bool trocaNivel = true;//se verdadeiro, faz avançar a fase
    ESTADO estadoCarregado;//variável do tipo ESTADO(estrutura principal)

    iniciaVariaveisEstadoCarregado(&estadoCarregado);

    InitWindow(LARGURA, ALTURA, "O jogo");
    SetTargetFPS(60);

    //LOOP PRINCIPAL DO JOGO
    while (!WindowShouldClose()&&estadoCarregado.info.vidas>0)//O JOGO SÓ CONTINUA ENQUANTO 'ESC' NÃO FOR PRESSIONADO *E* ENQUANTO O JOGADOR TIVER PELO MENOS '1' VIDA
    {
        //VERIFICA SE O JOGADOR DEVE PASSAR DE FASE
        for(i = 0; i < estadoCarregado.contaseres; i++){
            if(estadoCarregado.seres[i].vivo==false)
                k++;
        }
        if(k == estadoCarregado.contaseres){
            k=0;
            trocaNivel = true;
        }

        //FAZ O AVANÇO DE FASE (NO CASO DO PRIMEIRO JOGO, JÁ FARÁ O AVANÇO DA FASE '0' PARA A FASE '1'(PRIMEIRA EXISTENTE))
        if(trocaNivel == true){
            estadoCarregado.info.nivel++;//já começa no nivel '1'
            trocaNivel = false;//já torna falso, para a condição não ficar se repetindo durante o while

            //OPERA O ARQUIVO TEXTO (MAPA)
            FILE* fase;
            sprintf(estadoCarregado.nivel, "nivel%d.txt", estadoCarregado.info.nivel);//define o nome do arquivo a partir do contador de niveis
            fase = fopen(estadoCarregado.nivel, "r");//abre o arquivo
            if(fase == NULL){//verifica se foi bem sucedido
                printf("Erro na abertura do arquivo.");
                CloseWindow();//se não for, já fecha o jogo
            }else{
                for(i = 0; i < LINHAS; i++){
                    for(j = 0; j < COLUNAS; j++){
                        estadoCarregado.mapa[i][j] = getc(fase);//passa a matriz de caracteres do arquivo texto (MAPA) para uma matriz variavel do programa
                    }
                    getc(fase);
                }
            }
            fclose(fase);//fecha o arquivo texto

            //REINICIA ALGUMAS VARIÁVEIS
            variaveisParaProximaFase(&estadoCarregado, contador_de_mov_criatura);
         }

        //SE MENU ESTIVER ATIVO
        if(menu){
            //SAI DO MENU
            if(IsKeyPressed(KEY_V))
                menu = false;

            //FECHA O JOGO
            if(IsKeyPressed(KEY_Q))//fecha o jogo
               CloseWindow();

            //CRIA UM NOVO JOGO
            if(IsKeyPressed(KEY_N))//só não está funcionando para os monstros
            {
                 menu = false; //fecha o menu
                 estadoCarregado.info.nivel = 1; //começa o nível em '1'

                //OPERA O ARQUIVO TEXTO (MAPA)
                FILE* fase;
                sprintf(estadoCarregado.nivel, "nivel%d.txt", estadoCarregado.info.nivel);//define o nome do arquivo a partir do contador de niveis
                fase = fopen(estadoCarregado.nivel, "r");//abre o arquivo
                if(fase == NULL){//verifica se foi bem sucedido
                    printf("Erro na abertura do arquivo.");
                    CloseWindow();//se não for, já fecha o jogo
                }else{
                    for(i = 0; i < LINHAS; i++){
                        for(j = 0; j < COLUNAS; j++){
                            estadoCarregado.mapa[i][j] = getc(fase);//passa a matriz de caracteres do arquivo texto (MAPA) para uma matriz variavel do programa
                        }
                        getc(fase);
                    }
                }
                fclose(fase);//fecha o arquivo texto

                //REINICIA ALGUMAS VARIÁVEIS
                iniciaVariaveisEstadoCarregado(&estadoCarregado);
                variaveisParaProximaFase(&estadoCarregado, contador_de_mov_criatura);
            }

            //CARREGA O ÚLTIMO JOGO SALVO
            if(IsKeyPressed(KEY_C))
            {
                menu = false;
                if(carregaJogo(&estadoCarregado, arqbin)==1){
                    leMapa(estadoCarregado);
                    initJogo(&estadoCarregado);
                }else
                    printf("Erro ao carregar jogo!\n");
            }

            //SALVA O JOGO NA POSIÇÃO ATUAL
            if(IsKeyPressed(KEY_S))
            {
                menu = false;
                if(salvaJogo(estadoCarregado, arqbin)==1){
                    printf("Jogo salvo com sucesso!\n");
                }else
                    printf("Problema ao salvar jogo!\n");
            }

        }else{//SOMENTE SE O MENU ESTIVER DESATIVADO, VAI ATUALIZAR O JOGO

            //TIMERS
            contador_de_mov_criatura++;//faz a contagem do tempo para a movimentação das criaturas

            //CONDIÇÕES
            if(IsKeyPressed(KEY_TAB))
                menu = true;//se TAB for pressionado abre um menu

            //REINICIA OS DESLOCAMENTOS DO PERSONAGEM
            estadoCarregado.jogador.persdx = 0;
            estadoCarregado.jogador.persdy = 0;

            //IDENTIFICA OS COMANDOS DE MOVIMENTO DO JOGADOR (PODENDO SER NA CONFIGURAÇÃO A-W-S-D OU NAS SETAS)
            if(IsKeyPressed(KEY_D)||IsKeyPressed(KEY_RIGHT))estadoCarregado.jogador.persdx = ARESTA;
            if(IsKeyPressed(KEY_A)||IsKeyPressed(KEY_LEFT))estadoCarregado.jogador.persdx =- ARESTA;
            if(IsKeyPressed(KEY_W)||IsKeyPressed(KEY_UP))estadoCarregado.jogador.persdy =- ARESTA;
            if(IsKeyPressed(KEY_S)||IsKeyPressed(KEY_DOWN))estadoCarregado.jogador.persdy = ARESTA;

            //BOMBAS
            if(estadoCarregado.info.bombas>0){//se no arsenal tiver mais do que uma bomba, é possivel plantar uma bomba
                if(IsKeyPressed(KEY_B)){
                    if(estadoCarregado.bomba[0].bomba == false)//se a bomba '0' não estiver plantada, é possivel plantá-la
                        funcBomba(&estadoCarregado, 0);

                    if((estadoCarregado.bomba[1].bomba == false)&&(estadoCarregado.bomba[0].bomba == true))//se a bomba '1' não estiver plantada E a bomba '0' já estiver plantada, é possivel plantá-la
                        funcBomba(&estadoCarregado, 1);

                    if((estadoCarregado.bomba[2].bomba == false)&&(estadoCarregado.bomba[0].bomba == true)&&(estadoCarregado.bomba[1].bomba == true))//se a bomba '2' não estiver plantada E a bomba '0' e '1' já estiverem plantadas, é possivel plantá-la
                        funcBomba(&estadoCarregado,  2);
                }
            }

            for(i = 0; i < 3; i++){//procura se o contador de alguma das 3 bombas chegou ao final
                if((estadoCarregado.bomba[i].contador_de_explosao == 40) && (estadoCarregado.bomba[i].explosao == true)){//descobre se chegou a hora da explosão acabar
                    estadoCarregado.bomba[i].explosao = false;
                }
                if(estadoCarregado.bomba[i].explosao == true){//procura se a explosao de alguma das bombas foi iniciada
                    estadoCarregado.bomba[i].contador_de_explosao++;//comeca o contador de quadros daquela bomba
                }
                if(estadoCarregado.bomba[i].timer == true){//procura se o timer de alguma das 3 bombas foi inicializado
                    estadoCarregado.bomba[i].contador_de_bomba++;//comeca o contador de quadros daquela bomba
                }
                if(estadoCarregado.bomba[i].contador_de_bomba < 180 && estadoCarregado.bomba[i].contador_de_bomba > 1){//enquanto o contador não chegar a 180 frames, ou seja, 3 segundos
                    estadoCarregado.bomba[i].bomba = true;//a bomba esta plantada
                }
                if(estadoCarregado.bomba[i].contador_de_bomba == 180){//quando chega ao final
                    explosao(&estadoCarregado, danoX, danoY, &perdeVida, &serCapturado, i);//cria a explosao de dano
                    estadoCarregado.monstros[serCapturado].vivo = false;
                    estadoCarregado.bomba[i].bomba = false;//a bomba passa a não existir mais
                    estadoCarregado.bomba[i].explosao = true;//sinaliza que ocorre uma explosao grafica
                    estadoCarregado.bomba[i].contador_de_explosao = 0;
                    estadoCarregado.info.bombas+=1;//reabastece o arsenal
                        for(i = 0; i < LINHAS; i++){//percorre a matriz mapa e verifica onde há paredes destrutiveis E FUTURAMENTE MONSTROS
                            for(j = 0; j < COLUNAS; j++){
                                if(estadoCarregado.mapa[i][j] == 'D'){
                                    for(k = 0; k < 5; k++)
                                        if((i == (danoY[k]/ARESTA)) && (j == (danoX[k]/ARESTA))){//compara as posicoes das paredes destrutiveis com o RAIO DE DANO DA BOMBA que é em forma de '+'
                                            estadoCarregado.mapa[i][j] = ' ';//se existir, substitui aquela parede por um espaço vazio
                                            estadoCarregado.info.pontuacao+=10; //ganha 10 pontos por parede destruida
                                            initJogo(&estadoCarregado);//reinicializa o mapa, para guardar as alteraçoes
                                        }
                                    }
                            }

                        }
                }
            }

            //MOVIMENTAÇÃO
            if(podeMover(estadoCarregado)== 0){//se puder mover
                if(moveParaPocao(estadoCarregado.jogador, &estadoCarregado.pocao)== 0){//se for para cima de uma pocao
                    int pontos = 50;
                    colhePocao(&estadoCarregado, pontos);//realiza o processo de captura da pocao
                    estadoCarregado.pocao.qntdP -= 1;//diminui uma pocao do vetor de pocoes
                    initJogo(&estadoCarregado);//reinicializa o mapa
                }

                if(moveParaSer(estadoCarregado, &serCapturado)== 0){//se for para cima de um ser
                    estadoCarregado.seres[serCapturado].vivo = false; //esse ser deixa de estar vivo
                    estadoCarregado.contaseres = estadoCarregado.contaseres - 1; //diminui '1' no contador de seres
                    estadoCarregado.info.pontuacao += 10; //pontua
                }
                if(moveParaSer(estadoCarregado, &serCapturado)== 0)//se for para cima de um monstro
                    deu_dano = true;

                estadoCarregado.jogador.pos_dinamicaPersX += estadoCarregado.jogador.persdx;
                estadoCarregado.jogador.pos_dinamicaPersY += estadoCarregado.jogador.persdy;//desloca o personagem
            }

            //VIDA EXTRA
            for(i = 1; i < 10; i++){
                if((estadoCarregado.info.pontuacao >= (1000 * i))&&(vida_extra == false)){//quando a pontuação for multipla de 1000, ganha uma vida extra
                    vida_extra = !vida_extra;//para não ficar repetindo
                    estadoCarregado.info.vidas += 1;//ganha uma vida
                }
            }

            //VERIFICA SE DEVE HAVER MOVIMENTAÇÃO DOS MONSTROS
            if(contador_de_mov_criatura % 50 == 0)//movimenta as criaturas em multiplos de 50
                moveCriaturas(&estadoCarregado);


            for(i = 0; i < estadoCarregado.contamonstros; i ++){
                if(estadoCarregado.monstros[i].vivo == true)//se o monstro esta vivo
                    if(deu_dano == true){//e encostou no jogador
                        estadoCarregado.info.vidas-=1; //perde uma vida
                        deu_dano = false;//para não ficar se repetindo
                    }
            }


        }
        //GRÁFICOS
        desenhaJogo(&estadoCarregado, menu, danoX, danoY);
    }

    //se o laço for finalizado por motivos de falta de vidas, escreve "game over" e fecha a janela
    if(estadoCarregado.info.vidas==0){
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

//CHECKLIST
// dano dos monstros
// captura de seres após a primeira fase
// quando jogador perde uma vida, os seres ,monstros e jogador precisam voltar para as posições iniciais da fase
// bombas coexistentes desaparecem quando alguma explode
//comentários nas funções
