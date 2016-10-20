
#include<stdio.h>
#include <stdlib.h> 
#include "mpi.h"
#include<time.h>
#include<iostream>

using namespace std;

int MatSum(int * Mat, int n, int m)
{
	int sum = 0;
	for (int i = 0; i < n * m; i++)
		sum += Mat[i];
	return sum;
}

int main(int argc, char* argv[])
{
	setlocale(LC_ALL, "Russian");


	MPI_Status status;
	MPI_Init(&argc, &argv);



	int ProcNum, ProcRank;

	MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
	MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);

	int  n, m, a, b;
	n = 100;
	m = 100;
	a = 1;
	b = 10;
	
	if (argc >= 3) {
		n = atoi(argv[1]);
		m = atoi(argv[2]);
	}
	else {
		printf("Error with argv\n");
	}

	int l = n * m;

	double t1, t2, dt;
	int Sum1 = 0;
	int* arr = new int[l / ProcNum];

	if (ProcRank == 0)
	{
		srand(time(NULL));

		int* mat = new int[l];
		for (int i = 0; i < l; i++)
		{

			mat[i] = rand() % (b - a) + a;
			printf("%d ", mat[i]);

			if ((i + 1) % m == 0)
				printf("\n");
		}
		int Sum = 0;
		cout << "\n Not parallel version";
		t1 = MPI_Wtime();
		Sum = MatSum(mat, n, m);
		t2 = MPI_Wtime();
		dt = t2 - t1;
		printf("\nSUM  =  %d", Sum);
		printf("\nTIME = %f\n", dt);

		t1 = MPI_Wtime();
		for (int i = 1; i < ProcNum; i++)
			MPI_Send(mat + (i * (l / ProcNum)), l / ProcNum, MPI_INT, i, 0, MPI_COMM_WORLD);

		for (int i = 0; i < l / ProcNum; i++)
		{
			arr[i] = mat[i];
		}
		Sum = 0;

		for (int i = (l - (l % ProcNum)); i < l; i++)
			Sum1 += mat[i];

		delete[] mat;

	}




	if (ProcRank != 0)
		MPI_Recv(arr, l / ProcNum, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);

	int cSum, Sum;
	cSum = 0;
	for (int i = 0; i < l / ProcNum; i++)
		cSum += arr[i];

	MPI_Reduce(&cSum, &Sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

	if (ProcRank == 0)
	{
		Sum += Sum1;
		t2 = MPI_Wtime();
	}


	if (ProcRank == 0)
	{
		cout << "\nParallel version";
		printf("\nSUM  = %d", Sum);
		dt = t2 - t1;
		printf("\nTIME = %f\n", dt);
	}



	delete[] arr;



	MPI_Finalize();
	return 0;
}