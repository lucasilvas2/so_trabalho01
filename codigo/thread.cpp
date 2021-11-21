#include <iostream>
#include <string>
#include <chrono>
#include <vector>
#include <array>
#include <fstream>
#include <math.h>
#include <pthread.h>
#include <unistd.h>
using namespace std;
//variaveis no escopo grobal
int p;
int matriz1_linha = 0, matriz1_coluna = 0;
int matriz2_linha = 0, matriz2_coluna = 0;
vector<int> tempo;
vector<vector<int>> matriz1;
vector<vector<int>> matriz2;
vector<vector<int>> matriz_resultado;
//struct que será passada como parametro para função de multiplicação
struct struct_thread
{
    int thread_id;
    int tam_matriz;
    vector<int> linha;
    vector<int> coluna;
    vector<int> resultado;
};


void * multiplicacao_matriz(void * struct_t){
    auto tempo_inicio = chrono::steady_clock::now();
    struct_thread * thread_temp = new struct_thread;
    thread_temp = (struct_thread *) struct_t;

    int inicial_linha = 0;
    int inicial_coluna = 0;
    int final_linha;
    int final_coluna;
    if(thread_temp->thread_id == 0){
        final_linha =  p / thread_temp->tam_matriz;
        final_coluna = p % thread_temp->tam_matriz;
    }else{
        int inicio = thread_temp->thread_id * p;
        inicial_linha = inicio / thread_temp->tam_matriz;
        inicial_coluna = inicio % thread_temp->tam_matriz;
        final_linha = (inicio + p) / thread_temp->tam_matriz;
        final_coluna = (inicio + p) % thread_temp->tam_matriz;   
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
            thread_temp->linha.push_back(linha);
            thread_temp->coluna.push_back(coluna);
            thread_temp->resultado.push_back(acumula);

            acumula = 0;
            if(coluna == matriz1_coluna -1){
                inicial_coluna = 0;
            }
            if(controle == p - 1){
                //cout << "hora de parar" << endl;
                auto tempo_final = chrono::steady_clock::now();
                auto tempo_total = chrono::duration_cast<chrono::microseconds>(tempo_final- tempo_inicio).count();
                //cout << "tempo = " << (int) tempo_total << endl;
                tempo.push_back(tempo_total);
                pthread_exit(NULL);
            }
            controle++; 
        }
        
    }
    auto tempo_final = chrono::steady_clock::now();
    auto tempo_total = chrono::duration_cast<chrono::microseconds>(tempo_final- tempo_inicio).count();
    tempo.push_back(tempo_total);
    pthread_exit(NULL);
}
int main(int argc, char const *argv[])
{

    vector<int> lista_temp1;
    vector<int> lista_temp2;
    string matriz1_nome_txt = argv[1];
    string matriz2_nome_txt = argv[2];
    p = atoi(argv[3]);
    
    int temp;
    ifstream matriz1_e;
    ifstream matriz2_e;
    matriz1_e.open(matriz1_nome_txt);
    matriz2_e.open(matriz2_nome_txt);

    if(matriz1_e.bad()){
        cerr << "erro matriz 1" << endl;
        return 1;
    }
    if(matriz2_e.bad()){
        cerr << "erro matriz 2" << endl;
        return 1;
    }
    
    while (matriz1_e >> temp) {
        lista_temp1.push_back(temp);
    }
    while (matriz2_e >> temp) {
        lista_temp2.push_back(temp);
    }
    //lendo tamanho da matriz
    matriz1_linha = lista_temp1[0];
    matriz1_coluna = lista_temp1[1];
    matriz2_linha = lista_temp2[0];
    matriz2_coluna = lista_temp2[1];

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

    matriz1_e.close();
    matriz2_e.close();

    //verificar quantidas de threads
    int total_elementos = (matriz2_linha * matriz2_linha);
    int qtd_threads;
    if(total_elementos % p == 0){
        qtd_threads = total_elementos/p;
    }
    else{
        qtd_threads = total_elementos/p+1;
    }

    pthread_t threads[qtd_threads];
    struct_thread s_thread[qtd_threads];
    long int time[qtd_threads];
    int status, i;
    void * thread_return;
    
    for(i=0; i < qtd_threads ; i++){
        s_thread[i].thread_id = i;
        s_thread[i].tam_matriz = matriz1_linha;
        status = pthread_create(&threads[i], NULL, multiplicacao_matriz, &s_thread[i]);
        if(status!= 0){
            cerr << "erro thread id : " << i << endl;
            return 1;
        }
    }

    for(i=0; i < qtd_threads ; i++){
        pthread_join(threads[i], &thread_return);
    }
    
    int max_time = 0;
    for (int i = 0; i < qtd_threads; i++)
    {   
        ofstream resultado; 
        string pasta = "resultado_threads/";
        string nome_arquivo = "resultado_thread_";
        string id = to_string(i);
        string txt = ".txt";
        pasta += nome_arquivo;
        pasta += id;
        pasta += txt;
        
        resultado.open(pasta);
        resultado << matriz1_linha << " " << matriz2_coluna << endl;
        for (int k = 0; k < (s_thread[i].resultado.size()); k++)
        {
            resultado << "c" << s_thread[i].linha[k] << s_thread[i].coluna[k] << " " << s_thread[i].resultado[k] << endl;
        }
        resultado << tempo[i] << endl;
        if(max_time < tempo[i]){
            max_time = tempo[i];
        }
        resultado.close();
    }
    ofstream resultado_principal;
    resultado_principal.open("resultado_threads/resultado_1", ios::app);
    resultado_principal << matriz1_linha << "x" << matriz2_coluna <<";" << p <<";" << qtd_threads << ";" << max_time <<";" << endl;

    return 0;
}