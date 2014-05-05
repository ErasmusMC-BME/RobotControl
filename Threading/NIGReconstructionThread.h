// NXT++ test.cpp : Defines the entry point for the console application.

//NIGReconstruction

#ifndef NIGReconstructionThread_H
#define NIGReconstructionThread_H
#include "thread.h"
#include "NIGMultiThread.h"



class SharedObjects;

/*!
 * \brief TrakstarObjects  Class for the Trakstar thread 
 * Remarks
 *
 */
class ReconstructionObjects
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

	ReconstructionObjects()
	{
	}

	virtual ~ReconstructionObjects()
	{
	}
};



template  <class Tin,class Tout>  class ReconstructionThread:public thread<Tin,Tout> 
{

private:
	SharedObjects* m_sharedobjs;
	bool _bReconstructionFound;
	bool _bReconstructionReady;
	static ReconstructionThread *m_Reconstructionthread;
public:
	ReconstructionThread();
	~ReconstructionThread(void);
	static ReconstructionThread<Tin,Tout> * New(thread<Tin,Tout> * _thread=NULL);
	virtual void Initialize(const char *fmt=NULL, ...);
	virtual void ThreadEntryPoint();
	bool isReconstructionFound() {return _bReconstructionFound;}
private:
};

#include "NIGReconstructionThread.txx"

#endif