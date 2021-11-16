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
vector<vector<int>> matriz_resultado;


void multiplicando_matriz(int i, int * mem){

    //auto inicio_time = chrono::steady_clock::now();  
    int id = i;  
    int inicial_linha = 0;
    int inicial_coluna = 0;
    int final_linha;
    int final_coluna;
    int ponto_mem = 0;
    if(id == 0){
        final_linha =  *p / matriz1_linha;
        final_coluna = *p % matriz1_linha;
    }else{
        int inicio = id * *p;
        ponto_mem = inicio;
        inicial_linha = inicio / matriz1_linha;
        inicial_coluna = inicio % matriz1_linha;
        final_linha = (inicio + *p) / matriz1_linha;
        final_coluna = (inicio + *p) % matriz1_linha;   
    }
    int controle = 0, acumula = 0, coluna = 0;
    for (int linha = inicial_linha; linha < matriz1_linha; linha++)  
    {
        //matriz_resultado.push_back(vector<int>());
        //cout << "c["<< linha <<"] "<< endl;
        
        for (coluna = inicial_coluna; coluna < matriz2_coluna; coluna++)  
        {
            //matriz_resultado[linha].push_back(0);
            //cout << "c["<< linha <<"][" << coluna << "] "<< endl;
            for (int i = 0; i < matriz1_linha; i++)  
            {
                acumula = acumula + matriz1[linha][i] * matriz2[i][coluna];  
            }
            //matriz_resultado[linha][coluna] = acumula;
            mem[ponto_mem++] = acumula;
            //cout << "Id = "<< id << " c["<< linha <<"][" << coluna << "] "<< acumula << endl;
            acumula = 0;
            if(coluna == matriz1_coluna -1){
                inicial_coluna = 0;
            }
            if(controle == *p - 1){
                //cout << "hora de parar" << endl;
                //cout << "Filho " << id << " terimei..." << endl;
                //auto final_time = chrono::steady_clock::now();
                //time[i] = chrono::duration_cast<chrono::microseconds>(final_time - inicio_time).count(); 
                //cout << "T " << time[i] << endl;  
                pthread_exit(NULL);
            }
            controle++; 
        }
    }
    //cout << "Filho " << id << " terimei..." << endl;
    //auto final_time = chrono::steady_clock::now();
    //time[i] = chrono::duration_cast<chrono::microseconds>(final_time - inicio_time).count(); 
    //cout << "T " << time[i] << endl;  

}

int main(int argc, char const *argv[])
{
    string matriz1_nome_txt = argv[1];
    string matriz2_nome_txt = argv[2];
    int a = atoi(argv[3]);
    p = &a;
    
    vector<vector<int>> matriz_resultado;
    vector<int> lista_temp1;
    vector<int> lista_temp2;

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
    int qtd_processos;
    if(total_elementos % *p == 0){
        qtd_processos = total_elementos/ *p;
        //cout << "1 = " << qtd_processos << endl;
    }
    else{
        qtd_processos = total_elementos/ *p+1;
        //cout << "2 = " << qtd_processos << endl;
    }

    pid_t processo[qtd_processos];
    int * mem;
    int * time;
    //int time[qtd_processos];
    int seg_id = shmget(IPC_PRIVATE, total_elementos * sizeof(int), IPC_CREAT | 0666);
    int seg_id1 = shmget(IPC_PRIVATE, total_elementos * sizeof(int), IPC_CREAT | 0666);
    
    mem = (int *)shmat(seg_id, NULL, 0);
    time = (int *)shmat(seg_id1, NULL, 0);
    auto inicio_time = chrono::steady_clock::now();
    for (int i = 0; i < qtd_processos; i++)
    {          
        inicio_time = chrono::steady_clock::now(); 
        processo[i] = fork();    
        if(processo[i] < 0){
            cerr << "[ERRO] processo " << i << endl;
            exit(-1);
        }
        else if(processo[i] == 0){
            multiplicando_matriz(i, mem);
            shmdt(mem);
            shmdt(time);   
            exit(0);          
        }
    }
    
    for (int i = 0; i < qtd_processos; i++)
    {  
        wait(NULL);
        auto final_time = chrono::steady_clock::now();
        time[i] = chrono::duration_cast<chrono::microseconds>(final_time - inicio_time).count(); 
    }
    
    int max_time = 0;
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
        int controle = 0;
        for (int i = 0; i < matriz2_linha; i++)
        {
            for (int j = 0; j < matriz2_coluna; j++)
            {
                resultado << "c" << i << j << " " << mem[controle++]<<endl;
            }   
        }
        resultado << time[i] << endl;
        if(max_time< time[i]){
            max_time = time[i];
        }
        resultado.close();
    }
    ofstream resultado_principal;
    resultado_principal.open("resultado_processo/resultado", ios::app);
    resultado_principal << "M" << matriz1_linha << "x" << matriz2_coluna <<" | P = " << *p <<" | Qtd processos = " << qtd_processos << " | Tempo = " << max_time <<" " << endl;

    shmctl(*mem,IPC_RMID, NULL);
    shmctl(*time,IPC_RMID, NULL);
    return 0;
}
