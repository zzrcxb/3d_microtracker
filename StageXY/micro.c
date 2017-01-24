#include <stdio.h>
#include <stdlib.h>

#include <math.h>

//MCL_MicroDrive
#include "MicroDrive.h"


int InitMicroHandle()
{
	int handle;
	int i;
	double velocity = 0.10;// 0.02 < V < 2.0
	double distance = 0.20;
	
	if (handle = MCL_InitHandle())
		printf("Initial MicroDrive successfully !\nhandle=%d\n",handle);
	else
	{
		printf("error : MICRO HANDLE FAILED !\n");
	}
	
	return handle;
}

void MicroInfo(int handle)
{
	double encoder,stepsize,Vmax,Vmax2,Vmax3,Vmin;
	MCL_MicroDriveInformation(&encoder,&stepsize,&Vmax,&Vmax2,&Vmax3,&Vmin,handle);
	printf("encoder resolution:%f\n step size:%f\n \
	max velocity:%f\n VM2:%f VM3:%f\n min velocity:%f\n",\
	encoder,stepsize,Vmax,Vmax2,Vmax3,Vmin);
}

int GamePadXY(int direction,int handle)
{
	int i=0;
	int X=1,Y=2;
	double velocity = 2.0;// 0.02 < V < 2.0
	double distance = 5.0;
	
	int sx,sy,sz;

	//MCL_MicroDriveCurrentMicroStepPosition(&sx,&sy,&sz,handle);
	//printf("xstep=%d\nystep=%d\n",sx,sy);

	/*
	*ATTENTION:the x,y direction on the panel is not in accordance with the x,y here.
	*For convenience, we define x axis is vertical to the plane of symmetry of the microscope
	*Try it and you will find out
	*/
	
	switch(direction)
	{
		case 6:
			i=MCL_MicroDriveMoveProfile(Y, velocity, -distance, 0, handle);
			break;
		case 4:
			i=MCL_MicroDriveMoveProfile(Y, velocity, distance, 0, handle);
			break;
		case 2:
			i=MCL_MicroDriveMoveProfile(X, velocity, -distance, 0, handle);
			break;
		case 8:
			i=MCL_MicroDriveMoveProfile(X, velocity, distance, 0, handle);
			break;

		case 3:
			i=MCL_MicroDriveMoveProfileXYZ(velocity,-distance,0,velocity,-distance,0,0,0,0,handle);
			break;
		case 9:
			i=MCL_MicroDriveMoveProfileXYZ(velocity,distance,0,velocity,-distance,0,0,0,0,handle);
			break;
		case 1:
			i=MCL_MicroDriveMoveProfileXYZ(velocity,-distance,0,velocity,distance,0,0,0,0,handle);
			break;
		case 7:
			i=MCL_MicroDriveMoveProfileXYZ(velocity,distance,0,velocity,distance,0,0,0,0,handle);
			break;

		default:
			break;
	}
	MCL_MicroDriveWait(handle);
	MCL_MicroDriveCurrentMicroStepPosition(&sx,&sy,&sz,handle);
	printf("xstep=%d\nystep=%d\n",sx,sy);

	if(i!=0)
	{
		printf("i=%d\n",i);
		printf("GamePadXY ERROR!\n");
	}
	return i;
}

//int automoveXY()

int MoveXY(double xposit,double yposit,int handle)
{
	int i;
	double Vmax = 2.0;
	i=MCL_MicroDriveMoveProfileXYZ(Vmax,xposit,0,Vmax,yposit,0,0,0,0,handle);
	MCL_MicroDriveWait(handle);
	return i;
}

void ReleaseMicroHandle(int handle)
{
	MCL_ReleaseHandle(handle);
}