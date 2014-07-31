// NXT++ test.cpp : Defines the entry point for the console application.

//NIGViewing

#ifndef NIGViewingThread_H
#define NIGViewingThread_H
#include "thread.h"
#include "NIGMultiThread.h"



class SharedObjects;

/*!
 * \brief TrakstarObjects  Class for the Trakstar thread 
 * Remarks
 *
 */
class ViewingObjects
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

	ViewingObjects()
	{
	}

	virtual ~ViewingObjects()
	{
	}
};



template  <class Tin,class Tout>  class ViewingThread:public thread<Tin,Tout> 
{

private:
	SharedObjects* m_sharedobjs;
	bool _bViewingFound;
	bool _bViewingReady;
	static ViewingThread *m_Viewingthread;
public:
	ViewingThread();
	~ViewingThread(void);
	static ViewingThread<Tin,Tout> * New(thread<Tin,Tout> * _thread=NULL);
	virtual void Initialize(const char *fmt=NULL, ...);
	virtual void ThreadEntryPoint();
	bool isViewingFound() {return _bViewingFound;}
private:
};

#include "NIGViewingThread.txx"

#endif