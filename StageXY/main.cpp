#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "micro.h"

int main()
{
	time_t start_t, end_t;
	double diff_t;
	int micro = InitMicroHandle();

	time(&start_t);
	MoveXY(1, 1, 1);
	MoveXY(1, 1, 1);
	time(&end_t);

	diff_t = difftime(end_t, start_t);
	printf("time used=%lf\n", diff_t);

	ReleaseMicroHandle(micro);

}