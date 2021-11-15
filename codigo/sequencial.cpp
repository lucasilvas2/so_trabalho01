#include <iostream>
#include <string>
#include <chrono>
#include <vector>
#include <array>
#include <fstream>

using namespace std;

int main(int argc, char const *argv[])
{
    string matriz1_nome_txt = argv[1];
    string matriz2_nome_txt = argv[2];
    
    int matriz1_linha = 0, matriz1_coluna = 0;
    int matriz2_linha = 0, matriz2_coluna = 0;

    vector<int> lista_temp1;
    vector<int> lista_temp2;
    int temp;
    ifstream matriz1_e;
    ifstream matriz2_e;
    matriz1_e.open(matriz1_nome_txt);
    matriz2_e.open(matriz2_nome_txt);

    if(matriz1_e.bad()){
        cerr << "erro matriz 1" << endl;
        return -1;
    }
    if(matriz2_e.bad()){
        cerr << "erro matriz 2" << endl;
        return -1;
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

    //matriz
    vector<vector<int>> matriz1 (matriz1_linha, vector<int> (matriz1_coluna));
    vector<vector<int>> matriz2 (matriz2_linha, vector<int> (matriz2_coluna));
    int controlador = 2;
    for (int i = 0; i < matriz1_linha; i++)
    {
        for (int j = 0; j < matriz1_coluna; j++)
        {
            matriz1[i][j] = lista_temp1[controlador++];
        }    
    }

    /*for (int i = 0; i < matriz1_linha; i++)
    {
        for (int j = 0; j < matriz1_coluna; j++)
        {
            cout << matriz1[i][j] << " ";
        }
        cout << endl;
    }*/
    controlador = 2;
    for (int i = 0; i < matriz2_linha; i++)
    {
        for (int j = 0; j < matriz2_coluna; j++)
        {
            matriz2[i][j] = lista_temp2[controlador++];
        }    
    }
    /*for (int i = 0; i < matriz2_linha; i++)
    {
        for (int j = 0; j < matriz2_coluna; j++)
        {
            cout << matriz2[i][j] << " ";
        }
        cout << endl;
    }*/

    matriz1_e.close();
    matriz2_e.close();

    //matriz resultado
    //vector<vector<int>> matriz_resultado (matriz1_linha, vector<int> (matriz2_coluna));
    vector<vector<int>> matriz_resultado;
    int acumula = 0;
    auto inicio_time = chrono::steady_clock::now();
    /*for (int linha = 0; linha < matriz1_linha; linha++) 
    {
        for (int coluna = 0; coluna < matriz2_coluna; coluna++)  
        {
            for (int k = 0; k < matriz1_linha; k++)  
            {
                acumula = acumula + matriz1[linha][k] * matriz2[k][coluna];  
            }
            matriz_resultado[linha][coluna] = acumula;
            acumula = 0;  
        }
    }  */  
    for (int i = 0; i < matriz1_linha; i++)
    {
        matriz_resultado.push_back(vector<int>());
        for (int j = 0; j < matriz2_coluna; j++)
        {
            matriz_resultado[i].push_back(0);
            for (int k = 0; k < matriz1_linha; k++)
            {
                matriz_resultado[i][j] += matriz1[i][k] * matriz2[k][j];
            }
            
        }
        
    }
    

    auto final_time = chrono::steady_clock::now();
    auto time_total = chrono::duration_cast<chrono::microseconds>(final_time - inicio_time).count();
        
    //salvando resultado em txt
    ofstream matriz_resultado_txt;
    matriz_resultado_txt.open("resultado _sequencial/matriz_resultado_sequencial.txt");
    matriz_resultado_txt << matriz1_linha << " " << matriz2_coluna << endl;
    //escrevendo noa arquivo o resultado
    for (int i = 0; i < matriz1_coluna; i++)
    {
        for (int j = 0; j < matriz2_linha; j++)
        {
            matriz_resultado_txt<< "c" << i << j << " " << matriz_resultado[i][j] << endl;
        }
    }
    matriz_resultado_txt << time_total;
    matriz_resultado_txt.close(); 
    ofstream resultado_principal;
    resultado_principal.open("resultado_sequencial/resultado", ios::app);
    resultado_principal << "M" << matriz1_linha << "x" << matriz2_coluna << " | Tempo = " << time_total <<" " << endl;     
    return 0;
}
