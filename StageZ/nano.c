#include <stdio.h>
#include <stdlib.h>


#include "Madlib.h"



int InitNanoHandle()
{
	int handle1;
	
	if (handle1 = MCL_GetHandleBySerial(3093))
		printf("Initial NanoDrive successfully !\n%d\n",handle1);
	else
	{
		printf("error : NANO HANDLE FAILED!\n");
	}

	return handle1;
}

int NanoInfo(int handle)
{
	int i;
	struct ProductInformation pi;
	MCL_PrintDeviceInfo(handle);
	i=MCL_GetProductInfo(&pi,handle);//unfinished
	return i;
}

void GamePadZ(char direction,int handle)
{
	
	double distance = 20.0;
	double position = MCL_SingleReadZ(handle);

	printf("zposition=%lf\n",position);
	if(direction=='+')
		position = position+distance;
	if(direction=='-')
		position = position-distance;

	position = MCL_MonitorZ(position,handle);
	printf("zposition=%lf\n",position);

}

void MoveZ(double position,int handle)
{
	position = MCL_MonitorZ(position,handle);
	printf("zposition=%lf\n",position);
}

int AutoMoveZ(int handle)
{
	int i,j;
	int step = 20;
	int datapoint = step*2;
	double z,stepsize = 6;
	double start = 100-0.5*stepsize*step;
	double *waveform = &z;
	double milliseconds = 1;

	for(j=0; j<step; j++)
	{
		*(waveform+j) = stepsize*j+start;
		*(waveform+j+step) = 200-start-step*j;
	}
	i=MCL_LoadWaveFormN(3,datapoint,milliseconds,waveform,handle);
	return i;
}

void ReleaseNanoHandle(int handle)
{
	MCL_ReleaseHandle(handle);
} 
