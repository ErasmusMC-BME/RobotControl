// 
//
//Tiepie

#ifndef TiepieThread_H
#define TiepieThread_H

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
#define MAX_THREADS 1
#include "Timer.h"

#include "NIGMultiThread.h"

#include "vnl/vnl_vector.h"


//TiePie
#include "TiePieDLL.h"
#include "tiepie.h"

//#include "vxlutility.h"

class TiepieObjects
{
private:
	vnl_vector<double> _voltageCh1;
	vnl_vector<double> _timeCh1;
public:
	dword  recLen;
	double sensCh1;
	double fs; 

	vnl_vector<double>  GetTimeCh1()
	{
		return _timeCh1;
	}
	vnl_vector<double>  GetVoltageCh1()
	{
		return _voltageCh1;
	}
	void  SetTimeCh1(vnl_vector<double> timeCh1)
	{
		_timeCh1=timeCh1;
	}
	void SetVoltageCh1(vnl_vector<double> voltageCh1 )
	{
		_voltageCh1=voltageCh1;
	}
};


template  <class Tin,class Tout>  class TiepieThread:public thread<Tin,Tout>
{
private:
	bool _bTiepieFound;
	bool _bTiepieReady;
	bool _bTiepieDataReady;
	dword _recordLength;
	double _samplingFreq;
	double _tMeasurementStarted;
	//vnl_vector<double> _voltageCh1;
	//	vnl_vector<double> _timeCh1;

public:
	TiepieThread();
	~TiepieThread(void);
	static TiepieThread<Tin,Tout> * New(void);
	virtual void Initialize(const char *fmt=NULL, ...);
	virtual void ThreadEntryPoint();
	bool isTiepieFound(){return _bTiepieFound;}

	Tout *  GetOutput();
	virtual void TestCalibrationDate();
	virtual void RetrieveCh1DataFromInstrument();
	virtual void PrepareCh1ForAcq( dword recLen, double* sensCh1, double* fs );
private:
};

#if 0
class TiepieThread
{
private:
	SharedObjects* m_sharedobjs;	
	Timer *m_timer;
	bool _bTiepieFound;
	bool _bTiepieReady;
	bool _bTiepieDataReady;
	dword _recordLength;
	double _samplingFreq;
	double _tMeasurementStarted;
	vnl_vector<double> _voltageCh1;
	vnl_vector<double> _timeCh1;
public:

	TiepieThread( SharedObjects *sharedobjs )
	{
		
		m_sharedobjs = sharedobjs;
	}

	void SetTimer(Timer *_timer)
	{
		m_timer=_timer;
	}
	bool isTiepieFound()
	{
		return _bTiepieFound;

	}
	vnl_vector<double>  GetTimeCh1()
	{
		return _timeCh1;
	}
	vnl_vector<double>  GetVoltageCh1()
	{
		return _voltageCh1;
	}

  // In C++ you must employ a free (C) function or a static
  // class member function as the thread entry-point-function.

  static unsigned __stdcall ThreadStaticEntryPoint(void * pThis)
  {
      TiepieThread * pthX = (TiepieThread*)pThis;   // the tricky cast

      pthX->ThreadEntryPoint();    // now call the true entry-point-function

      // A thread terminates automatically if it completes execution,
      // or it can terminate itself with a call to _endthread().
      return 1;          // the thread exit code
  }
  static unsigned __stdcall ThreadStaticTiepieInitializeEntryPoint(void * pThis)
	{
      TiepieThread * pthX = (TiepieThread*)pThis;   // the tricky cast
      
      pthX->Initialize();    // now call the true entry-point-function

      // A thread terminates automatically if it completes execution,
      // or it can terminate itself with a call to _endthread().
      return 1;          // the thread exit code
   }
  static unsigned __stdcall ThreadStaticWriteEcgSampleEntryPoint(void * pThis)
  {
      TiepieThread * pthX = (TiepieThread*)pThis;   // the tricky cast

      pthX->WriteEcgSample();    // now call the true entry-point-function

      // A thread terminates automatically if it completes execution,
      // or it can terminate itself with a call to _endthread().
      return 1;          // the thread exit code
  }
  static unsigned __stdcall ThreadStaticStartMeasurementCh1EntryPoint(void * pThis)
  {
    TiepieThread * pthX = (TiepieThread*)pThis;   // the tricky cast

    pthX->StartMeasurementCh1();    // now call the true entry-point-function

    // A thread terminates automatically if it completes execution,
    // or it can terminate itself with a call to _endthread().
    return 1;          // the thread exit code
  }
  
  void ThreadEntryPoint()
  {
     // This is the desired entry-point-function but to get
     // here we have to use a 2 step procedure involving
     // the ThreadStaticEntryPoint() function.

  }
 	void TestCalibrationDate();
	void WriteEcgSample();
  void PrepareCh1ForAcq( dword recLen, double* sensCh1, double* fs );
  void StartMeasurementCh1();
  void RetrieveCh1DataFromInstrument();

};
#endif

#include "NIGTiepieThread.txx"

#endif