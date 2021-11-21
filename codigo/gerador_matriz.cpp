#include <iostream>
#include <random>
#include <chrono>
#include <vector>
#include <array>
#include <fstream>

using namespace std;

int main(int argc, char const *argv[])
{
    //gerar valores aleatórios
    srand(time(NULL));
    //dimensão recebida no terminal
    int matriz1_linha = atoi(argv[1]);
    int matriz1_coluna = atoi(argv[2]);
    int matriz2_linha = atoi(argv[3]);
    int matriz2_coluna = atoi(argv[4]);

    //inciando os arquivos
    ofstream arquivo_matriz_1;
    arquivo_matriz_1.open("matriz_1.txt");
    arquivo_matriz_1 << matriz1_linha << endl << matriz1_coluna << endl;
    ofstream arquivo_matriz_2;
    arquivo_matriz_2.open("matriz_2.txt");
    arquivo_matriz_2 << matriz2_linha << endl << matriz2_coluna << endl;
    
    //matriz
    vector<vector<int>> matriz1 (matriz1_linha, vector<int> (matriz1_coluna));
    vector<vector<int>> matriz2 (matriz2_linha, vector<int> (matriz2_coluna));
    //verificação da condição para multiplicação das matrizes
    if(matriz1_coluna == matriz2_linha){
        //criando matriz 1
        for (int i = 0; i < matriz1_linha; i++)
        {
            for (int j = 0; j < matriz1_coluna; j++)
            {
                matriz1[i][j] = rand()%25;
            }  
        }
        //escrevendo no txt
        for (int i = 0; i < matriz1_linha; i++)
        {
            for (int j = 0; j < matriz1_coluna; j++)
            {
                arquivo_matriz_1 << matriz1[i][j] << endl;
            }
        }
        arquivo_matriz_1.close();
        
        //criando matriz 2
        for (int i = 0; i < matriz2_linha; i++)
        {
            for (int j = 0; j < matriz2_coluna; j++)
            {
                matriz2[i][j] = rand()%100;
            }
            
        }
        //escrevendo em txt
        for (int i = 0; i < matriz2_linha; i++)
        {
            for (int j = 0; j < matriz2_coluna; j++)
            {
                arquivo_matriz_2 << matriz2[i][j] << endl;
            }
        }
        arquivo_matriz_2.close();
    }
    else{
        cout << "Não atende as condições para multiplicação" << endl;
        return -1;
    }

}