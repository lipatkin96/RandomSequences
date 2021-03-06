#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "../include/externs.h"
#include "../include/cephes.h"  
#include "../include/stat_fncs.h"

double psi2(int m, int n, BoolIterator epsilon);

std::pair<double, double>
Serial(int m, int n, BoolIterator epsilon)
{
	double	p_value1, p_value2, psim0, psim1, psim2, del1, del2;
	
	psim0 = psi2(m, n, epsilon);
	psim1 = psi2(m-1, n, epsilon);
	psim2 = psi2(m-2, n, epsilon);
	del1 = psim0 - psim1;
	del2 = psim0 - 2.0*psim1 + psim2;
	p_value1 = cephes_igamc(pow(2, m-1)/2, del1/2.0);
	p_value2 = cephes_igamc(pow(2, m-2)/2, del2/2.0);
   // printf("pv1 = %f, pv2 = %f\n", p_value1, p_value2);
	
	/*fprintf(stats[TEST_SERIAL], "\t\t\t       SERIAL TEST\n");
	fprintf(stats[TEST_SERIAL], "\t\t---------------------------------------------\n");
	fprintf(stats[TEST_SERIAL], "\t\t COMPUTATIONAL INFORMATION:		  \n");
	fprintf(stats[TEST_SERIAL], "\t\t---------------------------------------------\n");
	fprintf(stats[TEST_SERIAL], "\t\t(a) Block length    (m) = %d\n", m);
	fprintf(stats[TEST_SERIAL], "\t\t(b) Sequence length (n) = %d\n", n);
	fprintf(stats[TEST_SERIAL], "\t\t(c) Psi_m               = %f\n", psim0);
	fprintf(stats[TEST_SERIAL], "\t\t(d) Psi_m-1             = %f\n", psim1);
	fprintf(stats[TEST_SERIAL], "\t\t(e) Psi_m-2             = %f\n", psim2);
	fprintf(stats[TEST_SERIAL], "\t\t(f) Del_1               = %f\n", del1);
	fprintf(stats[TEST_SERIAL], "\t\t(g) Del_2               = %f\n", del2);
	fprintf(stats[TEST_SERIAL], "\t\t---------------------------------------------\n");
*/
	//printf("Serial #1:\t\t%s\t\tp_value1 = %f\n", p_value1 < ALPHA ? "FAILURE" : "SUCCESS", p_value1);
//	fprintf(results[TEST_SERIAL], "%f\n", p_value1);

	//printf("Serial #2:\t\t%s\t\tp_value2 = %f\n\n", p_value2 < ALPHA ? "FAILURE" : "SUCCESS", p_value2);// fflush(stats[TEST_SERIAL]);
	//fprintf(results[TEST_SERIAL], "%f\n", p_value2); fflush(results[TEST_SERIAL]);

	return std::make_pair(p_value1, p_value2);
}

double
psi2(int m, int n, BoolIterator epsilon)
{
	int				i, j, k, powLen, numOfBlocks;
	double			sum;
	unsigned int	*P;
	
	if ( (m == 0) || (m == -1) )
		return 0.0;
	numOfBlocks = n;
	powLen = (int)pow(2, m+1)-1;
	if ( (P = (unsigned int*)calloc(powLen,sizeof(unsigned int)))== NULL ) {
		printf("Serial Test:  Insufficient memory available.\n");
		//fflush(stats[TEST_SERIAL]);
		return 0.0;
	}
//#pragma omp parallel for
	for ( i=1; i<powLen-1; i++ )
		P[i] = 0;	  /* INITIALIZE NODES */
    // doesn't work in linux with gcc
#ifndef __GNUC__
//#pragma omp parallel for private(j, k) shared(P, epsilon)
#endif
    for (int i = 0; i < numOfBlocks; i++ ) {		 /* COMPUTE FREQUENCY */
		k = 1;
		for ( j=0; j<m; j++ ) {
			auto iter = epsilon;
			std::advance(iter, (i + j) % n);
			if ( *iter == 0 )
				k *= 2;
			else //if (*iter == 1 )
				k = 2*k+1;
		}
		P[k-1]++;
	}
	sum = 0.0;
//#pragma omp parallel for reduction(+:sum)
	for ( i=(int)pow(2, m)-1; i<(int)pow(2, m+1)-1; i++ )
		sum += pow(P[i], 2);
	sum = (sum * pow(2, m)/(double)n) - (double)n;
	free(P);
	
	return sum;
}
