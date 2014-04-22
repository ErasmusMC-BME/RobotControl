// NXT++ test.cpp : Defines the entry point for the console application.
//
//LEGO
#include "NXT++.h"

#include <iostream>
#include <fstream>
#include <iostream>
#include <iomanip>

#include <sys/stat.h>
#include <windows.h>
#include <time.h>
#include <snmp.h>
#include <conio.h>
#include <stdio.h>
#include "winsock.h"
#include <math.h>

#include <vector>
#include <string>
#include <map>
#include <list>
#include <vector>
#include <process.h>    /* _beginthread, _endthread */
#define MAX_THREADS 1

//TiePie
#include "TiePieDLL.h"
#include "tiepie.h"

// trakstar
#include <ATC3DG.h>
#include <sample2.h>
#include "NIGMultiThread.h"

threadargs1 myargs2;	
int param[MAX_THREADS] ;
HANDLE handle[MAX_THREADS];
unsigned tid[MAX_THREADS];
DWORD retval[MAX_THREADS];
DWORD tslice[MAX_THREADS];

unsigned __stdcall  StartOpenCVSubFunction(void *args)
{
	int index1=0 ;
	threadargs1 *myargs1 = (threadargs1*) args;
	OpenCVSubFunction  *thisptr = (OpenCVSubFunction *) myargs1->thisptr;
	thisptr->Initialize();
	thisptr->t= myargs1->t;
	_endthreadex( index1 );
	return index1;
}

unsigned __stdcall  StartTiepieSubFunction(void *args)
{
	int index1=0 ;
	threadargs1 *myargs1 = (threadargs1*) args;
	TiepieSubFunction  *thisptr = (TiepieSubFunction *) myargs1->thisptr;
	thisptr->Initialize();
	thisptr->t= myargs1->t;
	_endthreadex( index1 );
	return index1;
}

unsigned __stdcall  StartTrakstarSubFunction(void *args)
{
	int index1=0 ;
	threadargs1 *myargs1 = (threadargs1*) args;
	TrakstarSubFunction  *thisptr = (TrakstarSubFunction *) myargs1->thisptr;
	thisptr->Initialize();
	thisptr->t= myargs1->t;
	_endthreadex( index1 );
	return index1;
}

unsigned __stdcall  StartNXTSubFunction(void *args)
{
	int index1=0 ;
	threadargs1 *myargs1 = (threadargs1*) args;
	NXTSubFunction  *thisptr = (NXTSubFunction *) myargs1->thisptr;
	thisptr->Initialize();
	std::cout << "EndStartImageViewerThread: " << index1<< std::endl;
   _endthreadex( index1 );
	// std::cout << "_endthreadex " << std::endl;  
	return index1;
}

int TestNXTThreadedSubFunction()
{
	int param[MAX_THREADS] = { 1 };
	int i=0;
	handle[0] = 0;
	tid[0] = 0;

	// create threads
	NXTSubFunction *subFunction=new NXTSubFunction;
	myargs2.thisptr = subFunction;
	handle[i] = (HANDLE)_beginthreadex(0, 0, &StartNXTSubFunction, (void *)(&myargs2), CREATE_SUSPENDED, &tid[i]);

	cout << "Thread[" << handle[i] <<" " << tid[i]  << "] Created" << endl;
	// resume thread execution
	::ResumeThread(handle[i]);
	tslice[0] = 5000;
	retval[i] = ::WaitForSingleObject(handle[i], tslice[i]);
	std::cout << "WaitForSingleObject " << std::endl;

	while(1) //main loop
	{
		std::cout  <<  "Pres Key to give command" << std::endl;
		std::cout  <<  "1: Start loop test" << std::endl;
		std::cout  <<  "2: Play Tone" << std::endl;
		char input=std::getchar();
		int command=atoi(&input);
		switch(command)
		{
		case 1:
			subFunction->StartLoop();
			break;
		case 2:
			subFunction->PlayTone();
			break;
		default:
			break;
		}


	}

}

