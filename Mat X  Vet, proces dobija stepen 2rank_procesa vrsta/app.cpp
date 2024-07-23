/*Napisati MPI program koji realizuje mnozenje matrice Akxn i vektora bn.Matrica i vektor se inicijalizuju u master procesu.
Matrica je podeljena u blokove po vrstama tako da ce proces Pi dobiti prvih 2^i vrsta,proces Pi+1 sledecih 2^i+1 vrsta itd.
Vektor b se u celosti salje svim procesima.Predvideti da se slanje blokova matrica svakom procesu salje jednim MPI_Send pozivom
kojim se salju svi neophodni elementi matrice,dok se slanje vektora b obavlja grupnim operacijama.Svaki proces obavlja odgovarajuca 
izracunavanja i ucestvuje u generisanju rezultata.Rezultujuci vektor d treba se naci u procesu koji je ucitao najvise vrsta matrice A.*/

//ovaj slucaj radi za p=4 procesa,p=5->k=31,p=3->k=7...
#include<stdio.h>
#include<mpi.h>
#include<stdlib.h>
#include<math.h>
#define K 15
#define N 8
#define MASTER 0

void initMatrix(int* mat, int rows, int cols);
void printMatrix(int* mat, int rows, int cols, const char* msg);
void initArray(int* a, int rows);
void printArray(int* a, int rows, const char* msg);

int main(int argc, char** argv) {
	int rank, p;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &p);

	int a[K][N], b[N], c[K];

	int* locA = (int*)calloc((K / 2 + 1) * N, sizeof(int)),//blok ce biti popunjen za zadnji proces
		* locC = (int*)calloc(K, sizeof(int));

	if (locC == NULL || locA == NULL) return -1;

	if (rank == MASTER) {
		initMatrix(&a[0][0], K, N);
		initArray(&b[0], N);

		for (int i = 0; i < N; i++) {
			locA[i] = a[0][i];
		}

		int proc = 1;
		for (int i = 1; i < p; i++) {
			int power = pow(2, i) - 1;
			MPI_Send(&a[power][0], pow(2, i) * N, MPI_INT, proc++, 0, MPI_COMM_WORLD);
		}
	}
	else {
		MPI_Recv(locA, (K / 2 + 1) * N, MPI_INT, MASTER, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	}
	MPI_Bcast(&b[0], N, MPI_INT, MASTER, MPI_COMM_WORLD);

	for (int i = 0; i < K / 2 + 1; i++)
		for (int j = 0; j < N; j++) {
			locC[i] += locA[i * N + j] * b[j];
		}

	if (rank != 0)
	{
		int index = pow(2, rank) - 1;//shift svakog bloka

		for (int i = index; i >= 0; i--)
		{
			locC[i + index] = locC[i];
			locC[i] = 0;
		}
	}
	MPI_Reduce(&locC[0], &c[0], K, MPI_INT, MPI_SUM, p - 1, MPI_COMM_WORLD);
	if (rank == p - 1) {
		printf("Niz c u procesu %d koji je ucitao najvise vrsta:\n", rank);
		for (int i = 0; i < K; i++) {
			printf("%d ", c[i]);
		}
		printf("\n");
	}
	if (rank == MASTER) {
		printMatrix(&a[0][0], K, N, "\nMatrica A:\n");
		printArray(&b[0], N, "\Niz B:\n");
		//printArray(&c[0], K, "\Niz C:\n");
	}

	printf("\n[Process %d] \n", rank);
	printMatrix(locA, (K / 2 + 1), N, "locA:\n");
	printArray(b, N, "locB:\n");
	printArray(locC, K, "locC:\n");

	free(locA);
	free(locC);

	MPI_Finalize();
	return 0;
}
void initArray(int* a, int rows) {
	for (int i = 0; i < rows; i++)
		a[i] = i + 1;
}

void initMatrix(int* mat, int rows, int cols) {
	for (int i = 0; i < rows; i++)
		for (int j = 0; j < cols; j++)
			if (i == 4)
				mat[i * cols + j] = 0;
			else
				mat[i * cols + j] = i + j;
}
void printArray(int* a, int rows, const char* msg) {
	msg&& printf(msg);

	for (int i = 0; i < rows; i++) {
		printf("%d ", a[i]);
	}
	printf("\n");
}
void printMatrix(int* mat, int rows, int cols, const char* msg) {
	msg&& printf(msg);

	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			printf("%d\t", mat[i * cols + j]);
		}
		printf("\n");
	}
}