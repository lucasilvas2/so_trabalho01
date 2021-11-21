#include <iostream>
#include <vector>
#include <fstream>
#include <chrono>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <stdlib.h>
using namespace std;

int *p;
int matriz1_linha = 0, matriz1_coluna = 0;
int matriz2_linha = 0, matriz2_coluna = 0;
vector<vector<int>> matriz1;
vector<vector<int>> matriz2;

//receve o id e a ponteiro para memoria compartilhada
int multiplicando_matriz(int i, int * mem){

    int id = i;  
    int inicial_linha = 0;
    int inicial_coluna = 0;
    int final_linha;
    int final_coluna;
    //determinar o ponto no poteiro onde será salvo o valor do elemento
    int ponto_mem = 0;
    //definindo no primeiro até onde deve calcular ses elementos
    if(id == 0){
        final_linha =  *p / matriz1_linha;
        final_coluna = *p % matriz1_linha;
    }
    else{
        //primeiro elemento
        int inicio = id * *p;
        ponto_mem = inicio;
        //elemmento inicial - linha e coluna
        inicial_linha = inicio / matriz1_linha;
        inicial_coluna = inicio % matriz1_linha;
        //elemmento final - linha e coluna
        final_linha = (inicio + *p) / matriz1_linha;
        final_coluna = (inicio + *p) % matriz1_linha;   
    }

    int controle = 0, acumula = 0, coluna = 0;
    for (int linha = inicial_linha; linha < matriz1_linha; linha++)  
    {    
        for (coluna = inicial_coluna; coluna < matriz2_coluna; coluna++)  
        {
            for (int i = 0; i < matriz1_linha; i++)  
            {
                acumula = acumula + matriz1[linha][i] * matriz2[i][coluna];  
            }
 
            mem[ponto_mem++] = acumula;

            acumula = 0;
            //se o cauculo da matriz chegar no último elemento da coluna 
            //o valor é zerado para continuar a interação de forma correta
            if(coluna == matriz1_coluna -1){
                inicial_coluna = 0;
            }
            //se processo chegou a quantidade de elemento que deve calcular é forçado a encerrar
            if(controle == *p - 1){

                return 0;
            }
            controle++; 
        }
    }
    return 0;
}
int main(int argc, char const *argv[])
{
    //recebendo o nome do arquivo das matrizes e quantidade de elementos que cada processo deve calcular
    string matriz1_nome_txt = argv[1];
    string matriz2_nome_txt = argv[2];
    int a = atoi(argv[3]);
    p = &a;

    //vector temporario para salvar os elementos da leitura do arquivo txt   
    vector<int> lista_temp1;
    vector<int> lista_temp2;

    //inciando a leitura
    int temp;
    ifstream matriz1_e;
    ifstream matriz2_e;
    matriz1_e.open(matriz1_nome_txt);
    matriz2_e.open(matriz2_nome_txt);
    
    //verificando se os arquivos foram aberto de forma correta
    if(matriz1_e.bad()){
        cerr << "erro matriz 1" << endl;
        return 1;
    }
    if(matriz2_e.bad()){
        cerr << "erro matriz 2" << endl;
        return 1;
    }
    
    //leitura de todos os elementos
    while (matriz1_e >> temp) {
        lista_temp1.push_back(temp);
    }
    while (matriz2_e >> temp) {
        lista_temp2.push_back(temp);
    }

    //variaveis com informação do tamanho das matrizes
    matriz1_linha = lista_temp1[0];
    matriz1_coluna = lista_temp1[1];
    matriz2_linha = lista_temp2[0];
    matriz2_coluna = lista_temp2[1];

    //controlado para ignorar a informação do tamanho das matrizes
    //passando dos vetore para um vetor de vetor
    int controlador = 2;
    for (int i = 0; i < matriz1_linha; i++)
    {
        matriz1.push_back(vector<int>());
        for (int j = 0; j < matriz1_coluna; j++)
        {
            matriz1[i].push_back(0);
            matriz1[i][j] = lista_temp1[controlador++];
        }    
    }
    controlador = 2;
    for (int i = 0; i < matriz2_linha; i++)
    {
        matriz2.push_back(vector<int>());  
        for (int j = 0; j < matriz2_coluna; j++)
        {   
            matriz2[i].push_back(0);
            matriz2[i][j] = lista_temp2[controlador++];
        }    
    }
    //fechando o arquivo
    matriz1_e.close();
    matriz2_e.close();

    //verificar quantidas de processos
    int total_elementos = (matriz2_linha * matriz2_linha);
    int qtd_processos;
    if(total_elementos % *p == 0){
        qtd_processos = total_elementos/ *p;
    }
    else{
        qtd_processos = total_elementos/ *p+1;
    }

    //vetor de processos para criar os processos
    pid_t processo[qtd_processos];
    //criando o ponteiro para a usar na memória compartilhada
    int * mem;
    int * time;
    //criando memoria compartilhada
    int seg_id = shmget(IPC_PRIVATE, total_elementos * sizeof(int), IPC_CREAT | 0666);
    int seg_id1 = shmget(IPC_PRIVATE, total_elementos * sizeof(int), IPC_CREAT | 0666);
    //anexando ao ponteiro e local da memória
    mem = (int *)shmat(seg_id, NULL, 0);
    time = (int *)shmat(seg_id1, NULL, 0);


    for (int i = 0; i < qtd_processos; i++)
    {          
        processo[i] = fork();
        if(processo[i] < 0){
            cerr << "[ERRO] processo " << i << endl;
            exit(-1);
        }
        else if(processo[i] == 0){
            auto inicio_time = chrono::steady_clock::now();    
            multiplicando_matriz(i, mem);
            auto final_time = chrono::steady_clock::now();
            int teste = chrono::duration_cast<chrono::microseconds>(final_time - inicio_time).count();
            time[i] = teste;
            //processo desanexando o local da memória
            shmdt(mem);
            shmdt(time);   
            exit(0);          
        }
    }
    
    for (int i = 0; i < qtd_processos; i++)
    {  
        wait(NULL);
    }
    
    int max_time = 0, controle = 0, qtd_elementos = (int)(size_t) *p ,primeiro_elemento, ultimo_elemento;
    //salvando os resultado em arquivos
    for (size_t i = 0; i < qtd_processos; i++)
    {
        ofstream resultado;
        string pasta = "resultado_processo/";
        string nome_arquivo = "resultado_processo_";
        string id = to_string(i);
        string txt = ".txt";
        pasta += nome_arquivo;
        pasta += id;
        pasta += txt;
        
        resultado.open(pasta);
        resultado << matriz1_linha << " " << matriz2_coluna << endl;
        if(i == 0){
            primeiro_elemento = 0;
            ultimo_elemento = qtd_elementos;
        }else{
            primeiro_elemento = i * qtd_elementos;
            ultimo_elemento = primeiro_elemento + qtd_elementos;
        }
        
        for (int j = primeiro_elemento; j < ultimo_elemento; j++)
        {
            resultado << "c" << (j / matriz1_linha) << (j % matriz1_linha) << " " << mem[j] << endl;
            controle++;
        }
        
        resultado << time[i] << endl;
        if(max_time< time[i]){
            max_time = time[i];
        }
        resultado.close();
    }

    ofstream resultado_principal;
    resultado_principal.open("resultado_processo/resultado_1", ios::app);
    resultado_principal << matriz1_linha << "x" << matriz2_coluna <<" | P = " << *p <<" | Qtd processos = " << qtd_processos << " | Tempo = " << max_time <<" " << endl;

    //liberando a memoria compartilhada
    shmctl(*mem,IPC_RMID, NULL);
    shmctl(*time,IPC_RMID, NULL);
    return 0;
}
