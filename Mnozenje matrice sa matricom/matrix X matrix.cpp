#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>
#include<math.h>

#define MASTER 0
#define N 4

void printMatrix(int* mat, int rows, int cols, const char* msg);

int main(int argc, char** argv) {
	int rank, p;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &p);

	int a[N][N], b[N][N], c[N][N];

	if (rank == MASTER) {
		for (int i = 0; i < N; i++) {
			for (int j = 0; j < N; j++) {
				a[i][j] = rand() % 4 + 1;
				b[i][j] = rand() % 4 + 1;


			}
		}

		for (int i = 0; i < N; i++)
		{
			for (int j = 0; j < N; j++)
				printf("%d", a[i][j]);
			printf("\n");
		}printf("\n"); printf("\n");
		for (int i = 0; i < N; i++)
		{
			for (int j = 0; j < N; j++)
				printf("%d", b[i][j]);
			printf("\n");
		}printf("\n");
	}

	const int q = (int)sqrt(p);

	MPI_Comm row_comm, col_comm;
	MPI_Comm_split(MPI_COMM_WORLD, rank / q, rank % q, &row_comm);
	MPI_Comm_split(MPI_COMM_WORLD, rank % q, rank / q, &col_comm);

	int row_rank, col_rank;
	MPI_Comm_rank(row_comm, &row_rank);
	MPI_Comm_rank(col_comm, &col_rank);

	// Sending blocks of matrix a **********************************************************
	const int l = N / q;
	int* loc_a = (int*)malloc(l * N * sizeof(int));

	MPI_Datatype send_type;
	MPI_Type_vector(l, N, q * N, MPI_INT, &send_type);
	MPI_Type_create_resized(send_type, 0, N * sizeof(int), &send_type);
	MPI_Type_commit(&send_type);

	if (row_rank == MASTER) {//indeks od row_comm znaci procesi P0 i P2 ovde ulaze
		MPI_Scatter(&a[0][0], 1, send_type, loc_a, l * N, MPI_INT, MASTER, col_comm); //P0 i P2 dobijaju podatke i ovde je col_comm jer oba su 0, 0
	}
	MPI_Bcast(loc_a, l * N, MPI_INT, MASTER, row_comm);// P0 salje dobijene podatke procesu P1 , a P2 salje procesu P3
	//***********************************************************************************

	// Sending blocks of matrix b **********************************************************
	int* loc_b = (int*)malloc(l * N * sizeof(int));
	printf("A= %d\n", rank);
	for (int i = 0; i < l * N; i++)
		printf("%d ", loc_a[i]);
	printf("\n");
	MPI_Type_vector(l * N, 1, q, MPI_INT, &send_type);
	MPI_Type_create_resized(send_type, 0, sizeof(int), &send_type);
	MPI_Type_commit(&send_type);

	if (col_rank == MASTER) {
		MPI_Scatter(&b[0][0], 1, send_type, loc_b, l * N, MPI_INT, MASTER, row_comm);
	}
	MPI_Bcast(loc_b, l * N, MPI_INT, MASTER, col_comm);
	//************************************************************************************
	printf("B= %d\n", rank);
	for (int i = 0; i < l * N; i++)
		printf("%d ", loc_b[i]);
	printf("\n");
	// Local Matrix Multiplication
	int* loc_c = (int*)malloc(l * l * sizeof(int));
	for (int i = 0; i < l; i++) {
		for (int j = 0; j < l; j++) {
			loc_c[i * l + j] = 0;
			for (int k = 0; k < N; k++) {
				loc_c[i * l + j] += loc_a[i * N + k] * loc_b[k * l + j];
			
			}
		}
	}

	// Gather the results
	MPI_Gather(loc_c, l * l, MPI_INT, &c[0][0], l * l, MPI_INT, MASTER, MPI_COMM_WORLD);

	MPI_Finalize();
	free(loc_a);
	free(loc_b);
	//free(loc_c);

	if (rank == MASTER) {
		printMatrix(&c[0][0], N, N, "Result Matrix:");
	}

	return 0;
}

void printMatrix(int* mat, int rows, int cols, const char* msg) {
	printf("%s\n", msg);
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			printf("%d\t", mat[i * cols + j]);
		}
		printf("\n");
	}
}
