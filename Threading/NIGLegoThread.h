// NXT++ test.cpp : Defines the entry point for the console application.
// http://stackoverflow.com/questions/20595760/c-communication-between-threads
//http://social.msdn.microsoft.com/Forums/en-US/c1eebce9-9777-42a7-b5ac-4d612d71fe18/basics-of-multithreading-programming-communicating-between-threads?forum=vclanguage
//http://msdn.microsoft.com/en-us/library/ms686946(v=vs.85).aspx
//LEGO
//#include "NXT++.h"
#ifndef LegoThread_H
#define LegoThread_H
#if 0
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
#endif

#include "Timer.h"
#include "legocontrol.h"
#include "NIGMultiThread.h"
#include "Thread.h"
#include "vnl/vnl_vector.h"



class SharedObjects;

/*!
 * \brief TrakstarObjects  Class for the Trakstar thread 
 * Remarks
 *
 */
class LegoObjects
{

public:

	/*!
	 * \brief  Returns  
	 *
	 * Remarks
	 *
	 */
	
	/*!
	 * \brief  Sets  
	 *
	 * Remarks
	 *
	 */

	LegoObjects()
	{
	}

	virtual ~LegoObjects()
	{
	}
};



template  <class Tin,class Tout>  class LegoThread:public thread<Tin,Tout> , public Lego
{

private:
	SharedObjects* m_sharedobjs;
	Timer *m_timer;
	bool _bLegoFound;
	bool _bLegoReady;
	static LegoThread *m_Legothread;
public:
	LegoThread();
	~LegoThread(void);
	static LegoThread<Tin,Tout> * New(thread<Tin,Tout> * _thread=NULL);

	LegoThread* LegoThread::NewLC()
	{
	LegoThread * self= new  LegoThread();
	//CleanupStack::PushL(self);
	//self->ConstructL();
	return self;
	}


	virtual void Initialize();
	virtual void Initialize(const char *fmt, ...);
	virtual void Update();
	virtual void ThreadEntryPoint();
	virtual void ExecuteCommand();
	bool isLegoFound() {return _bLegoFound;}
	
	virtual void Calibrate();


	static unsigned __stdcall ThreadStaticLegoEntryPoint(void * pThis)
	{
		LegoThread * pthX = (LegoThread*)pThis;   // the tricky cast

		pthX->ThreadEntryPoint();    // now call the true entry-point-function

		// A thread terminates automatically if it completes execution,
		// or it can terminate itself with a call to _endthread().
		return 1;          // the thread exit code
	}

private:
};

#include "NIGLegoThread.txx"

#endif