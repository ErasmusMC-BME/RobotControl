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
#include <process.h>    /* CreateRecorderThread, _endthread */
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
private:
	vnl_matrix<double> _measures;

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
	vnl_matrix<double>  GetMeasures()
	{
		return _measures;
	}
	void SetMeasures(vnl_matrix<double> measures )
	{
		_measures=measures;
	}



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
	virtual void Initialize(const char *fmt=NULL, ...);	
	virtual void ThreadEntryPoint();
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
  void RecordPositionData();
private:
};




#include "NIGTrakstarThread.txx"
#endif