// 
//
//Tiepie

#ifndef TiepieThread_H
#define TiepieThread_H
#include <process.h>    /* CreateRecorderThread, _endthread */
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

#include "NIGTiepieThread.txx"

#endif