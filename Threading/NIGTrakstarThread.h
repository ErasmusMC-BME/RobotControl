// NXT++ test.cpp : Defines the entry point for the console application.
//
#ifndef TrakstarThread_H
#define TrakstarThread_H

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
#include "Timer.h"
#include "thread.h"
// trakstar
//#include <ATC3DG.h>
//#include <sample2.h>

#include "NIGMultiThread.h"

//#include "utility.h"
#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_matrix.h>
#include <vnl/algo/vnl_svd.h>
#include <vnl/vnl_matlab_filewrite.h>


class SharedObjects;

/*!
 * \brief TrakstarObjects  Class for the Trakstar thread 
 * Remarks
 *
 */
class TrakstarObjects
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

	TrakstarObjects()
	{
	}

	virtual ~TrakstarObjects()
	{
	}
};

template  <class Tin,class Tout>  class TrakstarThread:public thread<Tin,Tout>
{

private:
	SharedObjects* m_sharedobjs;
	Timer *m_timer;
	bool _bTrakstarFound;
	bool _bTrakstarReady;
	int _numSamples;
	double _samplingFreq;
	vnl_matrix<double> _measures;

	int _sensorID;
	int _transmitterID;
	int errorCode;

	static TrakstarThread *m_Trakstarthread;
public:
	TrakstarThread();
	~TrakstarThread(void);
	static TrakstarThread<Tin,Tout> * New(thread<Tin,Tout> * _thread=NULL);
	virtual void Initialize(const char *fmt, ...);
	virtual void Initialize();
	virtual void Update();
	virtual void ThreadEntryPoint();
	virtual void ExecuteCommand();
	bool isTrakstarFound(){return _bTrakstarFound;}


	void SetNumSamples(int numSamples)
	{
		_numSamples=numSamples;
	}
	void SetsamplingFreq(double samplingFreq)
	{
		_samplingFreq=samplingFreq;
	}

	vnl_matrix<double> Getmeasures()
	{
		return _measures;
	}


  void errorHandler(int error);
  void PrintSystemParamters();
  void PrintSensorParameters(int sensorID);
	void PrintTransmitterParameters(int transmitterID);
	static unsigned __stdcall ThreadStaticTrakstarInitializeEntryPoint(void * pThis)
	{
      TrakstarThread * pthX = (TrakstarThread*)pThis;   // the tricky cast

      pthX->Initialize();    // now call the true entry-point-function

      // A thread terminates automatically if it completes execution,
      // or it can terminate itself with a call to _endthread().
      return 1;          // the thread exit code
   }
  static unsigned __stdcall ThreadStaticRecordPositionDataEntryPoint(void * pThis)
  {
    TrakstarThread * pthX = (TrakstarThread*)pThis;   // the tricky cast

    pthX->RecordPositionData();    // now call the true entry-point-function

    // A thread terminates automatically if it completes execution,
    // or it can terminate itself with a call to _endthread().
    return 1;          // the thread exit code
  }
	static unsigned __stdcall ThreadStaticTrakstarEntryPoint(void * pThis)
	{
      TrakstarThread * pthX = (TrakstarThread*)pThis;   // the tricky cast

      pthX->ThreadEntryPoint();    // now call the true entry-point-function

      // A thread terminates automatically if it completes execution,
      // or it can terminate itself with a call to _endthread().
      return 1;          // the thread exit code
   }

  void RecordPositionData();
private:
};




#include "NIGTrakstarThread.txx"
#endif