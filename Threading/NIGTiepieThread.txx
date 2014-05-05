// NXT++ test.cpp : Defines the entry point for the console application.
//
template  <class Tin,class Tout>  TiepieThread<Tin,Tout>::TiepieThread( void )
{
}

template  <class Tin,class Tout>  TiepieThread<Tin,Tout>::~TiepieThread( void )
{
}

template <class Tin,class Tout>  TiepieThread<Tin,Tout> *  TiepieThread<Tin,Tout>::New(void)  // Create the object
{
	if(m_thread==NULL)
	{
		m_thread= new TiepieThread;
	}
	return (TiepieThread<Tin,Tout> * ) m_thread;
}

/*!
void Initialize(const char *fmt, ...)
Pass a variable parameter list 
the format string gives a format code for each parameter  <BR>
- W = word  <BR>
- D = double  <BR>
<BR>
"Wdd",  recordLength_Tiepie, sensCh1_Tiepie, df_Tiepie 
*/


template  <class Tin,class Tout>  void TiepieThread<Tin,Tout>::Initialize(const char *fmt, ...)
{
	//  initialization of TiePie
	word wAddress = 0x308;
	char sssPath[22];
	strcpy(sssPath,"hs3.dll");

	if ( OpenDLL(sssPath) ) //Tiepie test
	{	
		if ( InitInstrument( wAddress ) == E_NO_ERRORS )
		{
			_bTiepieFound=true;
			std::cerr << "TiePie initialization finished." << std::endl;
		}
		else
		{
			_bTiepieFound=false;
			std::cerr << "TiePie initialization failed." << std::endl;
		}
	}
	else
	{
		_bTiepieFound=false;
		std::cerr << "Opening TiePie DLL failed." << std::endl;
	}

	va_list args;
	// actual parameter passing
	dword recLen;
	double sensCh1;
	double fs;
	va_start(args, fmt);
	recLen = va_arg(args, dword);
	sensCh1= va_arg(args, double);
	fs = va_arg(args, double);
	PrepareCh1ForAcq( recLen, &sensCh1, &fs );
	va_end(args);
};

template  <class Tin,class Tout>  void TiepieThread<Tin,Tout>::ThreadEntryPoint( void )
{

	std::cout << "Press Key to RetrieveCh1DataFromInstrument" << std::endl;
	m_SyncObject->StartObjectWriteSyncronisationSection(1);
	//m_sharedobjs->StartTiepieSync();
	getchar();
	//int * ActualFrameNr;

	//ActualFrameNr=m_sharedobjs->GetActualFrameNr();
	//*ActualFrameNr=0;
	//m_sharedobjs->StopTiepieSync();

	Timer *m_timer= (Timer *) m_SyncObject;
	_tMeasurementStarted = m_timer->getElapsedTimeInSec();
	std::cout << "TiePie started measuring at time: " << _tMeasurementStarted << std::endl;

	if ( _bTiepieReady )
	{
		// start measuring
		ADC_Start();
		//m_sharedobjs->StopTiepieSync();
		// wait till data is acquired
		while( ADC_Ready() != 1 )
		{
		}
		_bTiepieDataReady = true;
	}
	else
	{
		_bTiepieDataReady = false;
	}
	//std::cout << "TiePie end measuring at time: " << m_timer->getElapsedTimeInSec() << std::endl;
	_endthread();

}


template  <class Tin,class Tout> Tout  *  TiepieThread<Tin,Tout>::GetOutput( )
{
	RetrieveCh1DataFromInstrument();
	return m_OutputData;
}

template  <class Tin,class Tout>  void TiepieThread<Tin,Tout>::TestCalibrationDate ()
{
	dword dwCalDate;
	dword dwYear;
	dword dwMonth;
	dword dwDay;
	if(_bTiepieFound)
		if ( GetCalibrationDate( &dwCalDate ) == E_NO_ERRORS )
		{
			dwDay   = dwCalDate >> 24;
			dwMonth = (dwCalDate >> 16) & 0xFF;
			dwYear  = dwCalDate & 0xFFFF;
			std::cout  <<  "Calibrated" << " " << dwDay <<  " "  << dwMonth << " " << dwYear << std::endl;
		}
}

template  <class Tin,class Tout>  void TiepieThread<Tin,Tout>::PrepareCh1ForAcq( dword recLen, double* sensCh1, double* fs )
{  
	_recordLength = recLen;
	_samplingFreq = *fs;
	_bTiepieReady = false;
	if ( _bTiepieFound )
	{
		// setup channel 1
		SetSensitivity( lCh1,  sensCh1 );
		SetCoupling( lCh1, lctAC );

		// setup trigger
		SetTriggerSource( ltsNoTrig );

		// setup time base
		_recordLength = recLen;
		SetRecordLength( _recordLength );
		SetPostSamples( _recordLength );
		_samplingFreq = *fs;
		SetSampleFrequencyF( fs );

		// select channels to record
		SetMeasureMode( mmCh1 );

		// write settings to instrument and start measuring
		ADC_Start();
		ADC_Abort();
		_bTiepieReady = true;
		std::cout << "TiePie ready to measure." << std::endl;
	}
	else
	{
		_bTiepieReady = false;
	}
}

template  <class Tin,class Tout>  void TiepieThread<Tin,Tout>::RetrieveCh1DataFromInstrument()
{
	//m_OutputData->TiepieObject::recLen=0;
	m_OutputData = (TiepieObjects *) new TiepieObjects;
	vnl_vector<double> _voltageCh1;
	vnl_vector<double> _timeCh1;
	//	double Time = m_timer->getElapsedTimeInSec();
	Timer *m_timer= (Timer *) m_SyncObject;

	double Time = m_timer->getElapsedTimeInSec();
	std::cout << "Start Time RetrieveCh1DataFromInstrument: " << Time << std::endl;
	_timeCh1.set_size( _recordLength );
	double * Ch1DoubleArray = new double[_recordLength];
	_voltageCh1.set_size( _recordLength );

	if ( _bTiepieFound ) ADC_GetDataVoltCh( mmCh1, Ch1DoubleArray );

	// put data into vnl vectors
	for( int it = 0; it < _recordLength; ++it )
	{
		_timeCh1[it] = _tMeasurementStarted + (double)it/_samplingFreq;
		if ( _bTiepieFound ) _voltageCh1[it] = Ch1DoubleArray[it];
	}
	m_OutputData->TiepieObjects::SetTimeCh1(_timeCh1);
	m_OutputData->SetVoltageCh1(_voltageCh1);
	//m_sharedobjs->SetTimeCh1(_timeCh1);
	//m_sharedobjs->SetVoltageCh1(_voltageCh1);
	// m_sharedobjs->StopTiepieSync();
	//Time = m_timer->getElapsedTimeInSec();
	// std::cout << "End Time RetrieveCh1DataFromInstrument: " << Time << std::endl;
	delete [] Ch1DoubleArray;


	//m_OutputData->TiepieObject::_timeCh1.set_size( _recordLength );
#if 0
	double Time = m_timer->getElapsedTimeInSec();
	//std::cout << "Start Time RetrieveCh1DataFromInstrument: " << Time << std::endl;
	// create time vector
	_timeCh1.set_size( _recordLength );

	// retrieve Ch1 data
	double * Ch1DoubleArray = new double[_recordLength];
	ADC_GetDataVoltCh( mmCh1, Ch1DoubleArray );
	_voltageCh1.set_size( _recordLength );

	// put data into vnl vectors
	for( int it = 0; it < _recordLength; ++it )
	{
		_timeCh1[it] = _tMeasurementStarted + (double)it/_samplingFreq;
		_voltageCh1[it] = Ch1DoubleArray[it];
	}

	m_sharedobjs->SetTimeCh1(_timeCh1);
	m_sharedobjs->SetVoltageCh1(_voltageCh1);
	// m_sharedobjs->StopTiepieSync();
	Time = m_timer->getElapsedTimeInSec();
	// std::cout << "End Time RetrieveCh1DataFromInstrument: " << Time << std::endl;
	delete [] Ch1DoubleArray;
#endif

}

