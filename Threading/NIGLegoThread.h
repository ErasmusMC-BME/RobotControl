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
#include <process.h>    /* CreateRecorderThread, _endthread */
#endif
#include "NXT++.h"
#include "Timer.h"
#include "NIGMultiThread.h"
#include "Thread.h"
#include "vnl/vnl_vector.h"

const int MAILBOX_A = 1;	//! Position Mailbox for Motor port A on the NXT
const int MAILBOX_B = 2;	//! Position Mailbox for Motor port B on the NXT
const int MAILBOX_C = 3;	//! Position Mailbox for Motor port C on the NXT
const int MAILBOX_INIT   = 4; //! Intialization Mailbox for Motors
const int MAILBOX_START  = 5; //! Start Mailbox for Motors
const int MAILBOX_RESET  = 6; //! Start Mailbox for Motors
const int MAILBOX_RECIEVE  = 8; 



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



template  <class Tin,class Tout>  class LegoThread:public thread<Tin,Tout> 
{

private:
	SharedObjects* m_sharedobjs;
	Timer *m_timer;
	bool _bLegoFound;
	bool _bLegoReady;
	static LegoThread *m_Legothread;
	Comm::NXTComm comm;
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


	virtual void Initialize(const char *fmt=NULL, ...);

	virtual void ThreadEntryPoint();
	bool isLegoFound() {return _bLegoFound;}

	virtual void Calibrate();

	void TextMessageSend(std::string message, int inbox);
	std::string TextMessageRecieve(int mailbox, bool remove);
	void WordSend(int Value, int inbox);
	int WordRecieve(int mailbox, bool remove);
	void BoolSend(bool Value, int inbox);
	bool BoolRecieve(int mailbox, bool remove);

private:
};

#include "NIGLegoThread.txx"

#endif