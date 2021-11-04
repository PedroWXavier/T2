#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "mpi.h"

#define INFINITY 9999
#define MAX 2000

//Função necessária para usar qsort, define comparação de inteiros
int compare (const void * a, const void * b)
{
  return ( *(int*)a - *(int*)b );
}
 
int main(int argc, char** argv)
{

    //É gerada uma matriz de MAX*MAX inteiros e um vetor de MAX booleans para saber quais colunas já foram processadas
    //O saco de trabalho é iniciado com valores aleatórios de 0 à MAX*MAX e o vetor de porntos é inicializado com "false" em todas as posições
    //Os processos escravos são disparados e pedem trabalho sempre que estiverem ociosos

    double runtime = 0.0;
    int i,j, startNode;
    int th_id, nthreads;
    int proxVetor = 0; //Indica o próximo vetor do saco de trabalho a ser enviado para escravos
    int sacoTrabalho[MAX][MAX];
    bool vetProntos[MAX];
    int vetRecv[MAX]; //vetor de trabalho para escravo
    int vetRetorno[MAX]; //vetor de retorno ordenado
    int my_rank; //Identificador do processo
    int proc_n = 1; //Número de processos
    int tag; /* Tag para as mensagens */
    int source; /* Iterador de processos */
    MPI_Status status; /* Status de retorno */
    double t1,t2;

    //printf("Entre o número de processos: ");
    //scanf("%d", &proc_n);

    //Inicializa o saco de trabalho com valores aleatórios
    srand(time(NULL));

    for(i=0;i<MAX;i++){
        vetProntos[i] = false;
        for(j=0;j<MAX;j++){
            sacoTrabalho[i][j] = rand() % MAX*MAX;
        }
    }
    
    //Decide o número de escravos
    //printf("\nEnter the number of threads: ");
    //scanf("%d",&nthreads);
        

    //Inicia o timer
    t1 = MPI_Wtime();  // inicia a contagem do tempo
    
    MPI_Init(&argc, & argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &proc_n);

    if (my_rank != 0){ //Se é escravo
        //Aqui pede trabalho para o mestre e entrega o vetor pronto
        //                                        v- Utiliza tag como o identificador do vetor enviado
        MPI_Recv(vetRecv, MAX, MPI_INT, my_rank, tag, MPI_COMM_WORLD, &status);
        memcpy ( &vetRetorno, &vetRecv, MAX );
        //*vetRetorno = *vetRecv;
        qsort(vetRetorno, MAX, sizeof(int), compare);
        MPI_Send(vetRetorno, MAX, MPI_INT, 0, tag, MPI_COMM_WORLD);
    }
    else{ //Se é mestre
        //Aqui entrega mais trabalhos
        for (source = 1; source < proc_n; source++){
            //                                                 v- Utiliza tag como o identificador do vetor enviado
            MPI_Send(sacoTrabalho[proxVetor], MAX, MPI_INT, 0, proxVetor, MPI_COMM_WORLD);
            proxVetor++;
        }
        //E recebe os prontos
        for (source = 1; source < proc_n; source++){
            MPI_Recv(vetRetorno, MAX, MPI_INT, my_rank, tag, MPI_COMM_WORLD, &status);
            //sacoTrabalho[tag] = vetRetorno;
        }
    }

    //Para o timer
    t2 = MPI_Wtime(); // termina a contagem do tempo
    printf("\nTempo de execucao: %f\n\n", t2-t1);
    
    MPI_Finalize();
}
 
