#include <iostream>
#include <chrono>
#include <vector>
#include <array>
#include <fstream>

using namespace std;

int main(int argc, char const *argv[])
{
    srand(time(NULL));
    int matriz1_linha = atoi(argv[1]);
    int matriz1_coluna = atoi(argv[2]);
    int matriz2_linha = atoi(argv[3]);
    int matriz2_coluna = atoi(argv[4]);
    ofstream arquivo_matriz_1;
    arquivo_matriz_1.open("matriz_1.txt");
    arquivo_matriz_1 << matriz1_linha << " " << matriz1_coluna << endl;
    ofstream arquivo_matriz_2;
    arquivo_matriz_2.open("matriz_2txt");
    arquivo_matriz_2 << matriz2_linha << " " << matriz2_coluna << endl;
    vector<vector<int>> matriz1 (matriz1_linha, vector<int> (matriz1_coluna));
    vector<vector<int>> matriz2 (matriz2_linha, vector<int> (matriz2_coluna));
    
    if(matriz1_coluna == matriz2_linha){
        //criando matriz 1
        for (int i = 0; i < matriz1_linha; i++)
        {
            for (int j = 0; j < matriz1_coluna; j++)
            {
                matriz1[i][j] = rand()%100;
            }
            
        }
        for (int i = 0; i < matriz1_linha; i++)
        {
            for (int j = 0; j < matriz1_coluna; j++)
            {
                arquivo_matriz_1 << matriz1[i][j] << " ";
            }
            arquivo_matriz_1 << endl;
        }
        cout << "m1" << endl;
        cout << "----------------------------" << endl;
        //criando matriz 2
        for (int i = 0; i < matriz2_linha; i++)
        {
            for (int j = 0; j < matriz2_coluna; j++)
            {
                matriz2[i][j] = rand()%100;
            }
            
        }
        for (int i = 0; i < matriz2_linha; i++)
        {
            for (int j = 0; j < matriz2_coluna; j++)
            {
                arquivo_matriz_2 << matriz2[i][j] << " ";
            }
            arquivo_matriz_2 << endl;
        }
        cout << "m2" << endl;
        cout << "----------------------------" << endl;

        //criando a matriz resultado
        vector<vector<int>> matriz_resultado (matriz1_linha, vector<int> (matriz2_coluna));
        //multiplicando as matrizes
        //3x2 2x3
        /*for (int i = 0; i < matriz1_linha; i++)
        {
            for (int j = 0; j < matriz2_coluna; j++)
            {
                for (int k = 0; k < matriz1_coluna; k++)
                {
                    matriz_resultado[i][j] += matriz1[i][k] * matriz2[k][j];
                }
                
            }
        }*/
        //cada iteração representa uma linha da matriz A
        int acumula = 0;
        int contador = 0;
        for (int linha = 0; linha < matriz1_linha; linha++)  
        {
            //em cada linha de A, itera nas colunas de B
            for (int coluna = 0; coluna < matriz2_coluna; coluna++)  
            {
                //itera, ao mesmo tempo, entre os elementos da linha de A e da coluna de B
                for (int i = 0; i < matriz1_linha; i++)  
                {
                    //acumula representa os valores que estávamos reservando
                    acumula = acumula + matriz1[linha][i] * matriz2[i][coluna];  
                }
                //quando a execução está aqui, já se tem mais um elemento da matriz AB
                matriz_resultado[linha][coluna] = acumula;
                //cout << "E[" << linha << "][" << coluna << "] " << endl;
                //contador++;
                //cout << "Valor do contador = " << contador << endl;
                //a variável então é zerada para que possa referenciar um novo elemento de AB
                acumula = 0;  
            }
        }

        //imprimir valor
        /*for (int i = 0; i < matriz1_coluna; i++)
        {
            for (int j = 0; j < matriz2_linha; j++)
            {
                cout << matriz_resultado[i][j] << " ";
            }
            cout << endl;
        }*/
        
        return 0;
    }
    else{
        cout << "Não atende as condições para multiplicação" << endl;
        return -1;
    }

}
