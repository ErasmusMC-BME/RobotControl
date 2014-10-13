// NXT++ test.cpp : Defines the entry point for the console application.
//

// Function definitions
template  <class Tin,class Tout>  TrakstarThread<Tin,Tout>::TrakstarThread( void )
{
	_bTrakstarFound = false;
	_bTrakstarReady = false;
	_numSamples = 10;
	_samplingFreq = 80;
	_numSensors=0;
	//_sensorID = 0;
	_transmitterID = 0;
	
}

template  <class Tin,class Tout>  TrakstarThread<Tin,Tout>::~TrakstarThread( void )
{
}



template <class Tin,class Tout>  TrakstarThread<Tin,Tout> *  TrakstarThread<Tin,Tout>::New( thread<Tin,Tout> * _thread=NULL )  // Create the object
{
	if(m_Trakstarthread==NULL)
	{
		m_Trakstarthread= new TrakstarThread;
	}
	return (TrakstarThread<Tin,Tout> * ) m_Trakstarthread;
}

template  <class Tin,class Tout>  void  TrakstarThread<Tin,Tout>::Initialize(const char *fmt, ...)
{
  /*!
  Initialization of the TrakStar system. 
  */
	va_list args;
	va_start(args, fmt);
  _numSamples = va_arg( args, int );
  _samplingFreq = va_arg( args, double );
  va_end(args);
	
  std::cout << "Initializing TrakStar with " << _numSamples << " samples, and " << _samplingFreq << " Hz sampling frequency" << std::endl;

  // start initialization
	m_OutputData = new Tout;
  int				errorCode;		// used to hold error code returned from procedure call

	// Initialize the ATC3DG driver and DLL
	errorCode = InitializeBIRDSystem();
  errorCode = GetBIRDSystemConfiguration(&_ATC3DG.m_config);
	if(errorCode==BIRD_ERROR_SUCCESS) 
	{		
		_bTrakstarFound = true;
		// Set System Parameters
		SET_SYSTEM_PARAMETER( SELECT_TRANSMITTER, 0 );
		SET_SYSTEM_PARAMETER( POWER_LINE_FREQUENCY, 50.0 );
		SET_SYSTEM_PARAMETER( METRIC, true );
		SET_SYSTEM_PARAMETER( MEASUREMENT_RATE, _samplingFreq );
		//PrintSystemParamters();

		// set up sensor
		_pSensor = new CSensor[_ATC3DG.m_config.numberSensors];
    for( int i = 0 ; i < _ATC3DG.m_config.numberSensors; i++ )
	  {
		  errorCode = GetSensorConfiguration(i, &(_pSensor)->m_config);
      if(errorCode == BIRD_ERROR_SUCCESS)
      {
        BOOL sensorAttached = _pSensor->m_config.attached;
        if ( sensorAttached )
        {
          std::cout <<  "RX with id " << i << " with serial number "<< (ULONG) _pSensor->m_config.serialNumber << " is connected." << std::endl;
          _sensorID.push_back( i );
        }
      }
      else
      {
		    errorHandler(errorCode);
      }
        
	  }
		
    // set up transmitter
		_transmitterID = 0;
		
		std::cout  << "TrakStar initialization finished." << std::endl;
		 SetNumSensors(_sensorID.size());
		_bTrakstarReady = true;
	}
	else
	{
		 std::cout  << "TrakStar initialization failed." << std::endl;
		_bTrakstarFound = false;
		_bTrakstarReady = false;
	}

};


template  <class Tin,class Tout>  void TrakstarThread<Tin,Tout>::ThreadEntryPoint( void )
{
	RecordPositionData();
	_endthread(); // call to OS class

	// This is the desired entry-point-function but to get
	// here we have to use a 2 step procedure involving
	// the ThreadStaticEntryPoint() function.

}


template  <class Tin,class Tout> void TrakstarThread<Tin,Tout>::RecordPositionData()
{
	if ( _bTrakstarReady )
	{
		Timer *m_timer= (Timer *) m_SyncObject;
		// make sure transmitter is switched on
		SET_SYSTEM_PARAMETER( SELECT_TRANSMITTER,	_transmitterID );

		// measure position and angle
		DOUBLE_POSITION_ANGLES_RECORD record, *pRecord = &record;
		double currentTime;
		std::cout << "TrakStar started recording at: " << m_timer->getElapsedTimeInSec() << " sec." << std::endl;
	
    // prepare the vector containing the measurements
    vnl_matrix<double> currentMeasures( _numSamples, 7, -1.0 );
    for ( int sensorItr = 0; sensorItr < _sensorID.size(); sensorItr++ )
    {
      _measures.push_back( currentMeasures );
    }

   
		for (int idxMeasure = 0; idxMeasure < _numSamples; ++idxMeasure) // change name to possibly idxSlot
		{
      for ( int sensorItr = 0; sensorItr < _sensorID.size(); sensorItr++ )
      {
        /* measure position */
			  errorCode = GetAsynchronousRecord(_sensorID[sensorItr], pRecord, sizeof(record));
			  currentTime = m_timer->getElapsedTimeInSec();

			  if(errorCode!=BIRD_ERROR_SUCCESS) {errorHandler(errorCode);}

			  // get the status of the last data record
			  // only report the data if everything is okay
			  unsigned int status = GetSensorStatus( _sensorID[sensorItr]);
			  if( status == VALID_STATUS)
			  {
          currentMeasures = _measures[sensorItr]; // get data out of measure vector

          // need to change that  for multi sensor measurements (Alex 01-May)
				  vnl_vector<double> * m_CurrentMeasures = m_InputData->GetCurrentMeasures();
			    (*m_CurrentMeasures)[0] = currentTime;
				  (*m_CurrentMeasures)[1] = record.x;
				  (*m_CurrentMeasures)[2] = record.y;
				  (*m_CurrentMeasures)[3] = record.z;
				  (*m_CurrentMeasures)[4] = record.a;
				  (*m_CurrentMeasures)[5] = record.e;
				  (*m_CurrentMeasures)[6] = record.r;
          // END  need to change that  for multi sensor measurements (Alex 01-May)

				  currentMeasures[idxMeasure][0] = currentTime;
				  currentMeasures[idxMeasure][1] = record.x;
				  currentMeasures[idxMeasure][2] = record.y;
				  currentMeasures[idxMeasure][3] = record.z;
				  currentMeasures[idxMeasure][4] = record.a;
				  currentMeasures[idxMeasure][5] = record.e;
				  currentMeasures[idxMeasure][6] = record.r;
          
          // update measurements vector
          _measures[sensorItr] = currentMeasures;

          // wait at least half a sampling period of the TrakStar system
          // The wait time is a bit arbritary. Should change to Synchronous record (Alex 01-May)
				  while ( m_timer->getElapsedTimeInSec() - currentTime < 0.5/_samplingFreq )
				  {
				  }
          // END The wait time is a bit arbritary. Should change to Synchronous record (Alex 01-May)
			  }
			  else
			  {
				  std::cout << "Sensor status not valid." << std::endl;
			  }
      }
		}
		m_OutputData->TrakstarObjects::SetMeasures(_measures);
		SET_SYSTEM_PARAMETER(SELECT_TRANSMITTER,	-1);
	}

	// clean up 
	printf("Switch off transmitter\n");
}

template  <class Tin,class Tout> void TrakstarThread<Tin,Tout>::errorHandler(int error)
{
	char			buffer[1024];
	char			*pBuffer = &buffer[0];
	int				numberBytes;
	int				currentError = error;
	int				nextError;

	//////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////
	//
	//  The following code shows you how to use the procedure GetErrorText().
	//
	//  When making the call you will pass the errorcode to be decoded and
	//  a pointer to a buffer where the message string will be placed
	//  Note: This procedure like all the others will also return an error code.
	//  This new error code will either indicate a problem with the call itself or
	//  will simply be the next error code in the system error code queue.
	//  Looping on a test of this returned error code will cause the 
	//  extraction of all current errors in the queue.
	//
	do{
		nextError = GetErrorText(currentError, pBuffer, sizeof(buffer), SIMPLE_MESSAGE);

		//////////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////////
		//
		//	Display the message string for the "current error"
		//
		//  Insert display mechanism of choice here. As an example this sample
		//	simply sends the message string to the console display using printf()
		//	Note: The message strings returned from the call do not contain a
		//	terminating newline character. If the user needs the strings to be
		//	displayed on succeeding lines then a newline "\n" needs to be added.
		//
		numberBytes = strlen(buffer);
		buffer[numberBytes] = '\n';		// append a newline to buffer
		printf("%s", buffer);

		currentError = nextError;
	}while(currentError!=BIRD_ERROR_SUCCESS);

	exit(0);
}
template  <class Tin,class Tout> void TrakstarThread<Tin,Tout>::PrintSystemParamters()
{
	int				errorCode;		// used to hold error code returned from procedure call
	printf("GetSystemParameter()");
	printf("\n=============================\n");
	//
	// SELECT_TRANSMITTER
	//
	{
		short buffer, *pBuffer = &buffer;
		errorCode = GetSystemParameter(SELECT_TRANSMITTER, pBuffer, sizeof(buffer));
		if(errorCode!=BIRD_ERROR_SUCCESS) errorHandler(errorCode);
		printf("SELECT_TRANSMITTER: %d\n", buffer);
	}
	//
	// POWER_LINE_FREQUENCY
	//
	{
		double buffer, *pBuffer = &buffer;
		errorCode = GetSystemParameter(POWER_LINE_FREQUENCY, pBuffer, sizeof(buffer));
		if(errorCode!=BIRD_ERROR_SUCCESS) errorHandler(errorCode);
		printf("POWER_LINE_FREQUENCY: %5.2f\n", buffer);
	}
	//
	// AGC_MODE
	//
	{
		AGC_MODE_TYPE buffer, *pBuffer = &buffer;
		errorCode = GetSystemParameter(AGC_MODE, pBuffer, sizeof(buffer));
		if(errorCode!=BIRD_ERROR_SUCCESS) errorHandler(errorCode);
		printf("AGC_MODE: %d\n", buffer);
	}
	//
	// MEASUREMENT_RATE
	//
	{
		double buffer, *pBuffer = &buffer;
		errorCode = GetSystemParameter(MEASUREMENT_RATE, pBuffer, sizeof(buffer));
		if(errorCode!=BIRD_ERROR_SUCCESS) errorHandler(errorCode);
		printf("MEASUREMENT_RATE: %5.2f\n", buffer);
	}
	//
	// MAXIMUM_RANGE
	//
	{
		double buffer, *pBuffer = &buffer;
		errorCode = GetSystemParameter(MAXIMUM_RANGE, pBuffer, sizeof(buffer));
		if(errorCode!=BIRD_ERROR_SUCCESS) errorHandler(errorCode);
		printf("MAXIMUM_RANGE: %5.2f\n", buffer);
	}
	//
	// METRIC
	//
	{
		BOOL buffer, *pBuffer = &buffer;
		errorCode = GetSystemParameter(METRIC, pBuffer, sizeof(buffer));
		if(errorCode!=BIRD_ERROR_SUCCESS) errorHandler(errorCode);
		printf("METRIC: %d\n\n", buffer);
	}
}
template  <class Tin,class Tout> void TrakstarThread<Tin,Tout>::PrintSensorParameters(int sensorID)
{
	int				errorCode;		// used to hold error code returned from procedure call
	//////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////
	//
	// GetSensorParameter()
	//
	// This procedure allows the user to inspect selected sensor parameters. The 
	// procedure is directed at a specific sensor by selecting that sensor with 
	// the Sensor ID parameter. For the purpose of this exercise the ID = 0. This 
	// is the 1st sensor in a system. This sensor will need to be attached for
	// these calls to respond without an error.
	//
	printf("\nGetSensorParameter()");
	printf("\n====================\n");
	printf("Using sensor ID = 0\n");

	//
	// DATA_FORMAT
	//
	{
		DATA_FORMAT_TYPE buffer, *pBuffer = &buffer;
		errorCode = GetSensorParameter(sensorID, DATA_FORMAT, pBuffer, sizeof(buffer));
		if(errorCode!=BIRD_ERROR_SUCCESS) errorHandler(errorCode);
		printf("DATA_FORMAT: %d\n", buffer);
	}
	//
	// ANGLE_ALIGN
	//
	{
		DOUBLE_ANGLES_RECORD buffer, *pBuffer = &buffer;
		errorCode = GetSensorParameter(sensorID, ANGLE_ALIGN, pBuffer, sizeof(buffer));
		if(errorCode!=BIRD_ERROR_SUCCESS) errorHandler(errorCode);
		printf("ANGLE_ALIGN: %6.2f, %6.2f, %6.2f\n", 
			buffer.a,
			buffer.e,
			buffer.r	);
	}
	//
	// HEMISPHERE
	//
	{
		HEMISPHERE_TYPE buffer, *pBuffer = &buffer;
		errorCode = GetSensorParameter(sensorID, HEMISPHERE, pBuffer, sizeof(buffer));
		if(errorCode!=BIRD_ERROR_SUCCESS) errorHandler(errorCode);
		printf("HEMISPHERE: %4x\n", buffer);
	}
	//
	// FILTER_AC_WIDE_NOTCH
	//
	{
		BOOL buffer, *pBuffer = &buffer;
		errorCode = GetSensorParameter(sensorID, FILTER_AC_WIDE_NOTCH, pBuffer, sizeof(buffer));
		if(errorCode!=BIRD_ERROR_SUCCESS) errorHandler(errorCode);
		printf("FILTER_AC_WIDE_NOTCH: %d\n", buffer);
	}
	//
	// FILTER_AC_NARROW_NOTCH
	//
	{
		BOOL buffer, *pBuffer = &buffer;
		errorCode = GetSensorParameter(sensorID, FILTER_AC_NARROW_NOTCH, pBuffer, sizeof(buffer));
		if(errorCode!=BIRD_ERROR_SUCCESS) errorHandler(errorCode);
		printf("FILTER_AC_NARROW_NOTCH: %d\n", buffer);
	}
	//
	// FILTER_DC_ADAPTIVE
	//
	{
		double buffer, *pBuffer = &buffer;
		errorCode = GetSensorParameter(sensorID, FILTER_DC_ADAPTIVE, pBuffer, sizeof(buffer));
		if(errorCode!=BIRD_ERROR_SUCCESS) errorHandler(errorCode);
		printf("FILTER_DC_ADAPTIVE: %5.2f\n", buffer);
	}
	//
	// FILTER_ALPHA_PARAMETERS
	//
	{
		ADAPTIVE_PARAMETERS buffer, *pBuffer = &buffer;
		errorCode = GetSensorParameter(sensorID, FILTER_ALPHA_PARAMETERS, pBuffer, sizeof(buffer));
		if(errorCode!=BIRD_ERROR_SUCCESS) errorHandler(errorCode);
		printf("FILTER_ALPHA_PARAMETERS:\n");
		printf("    Alpha max   %5d, %5d, %5d, %5d, %5d, %5d\n",
			buffer.alphaMax[0],
			buffer.alphaMax[1],
			buffer.alphaMax[2],
			buffer.alphaMax[3],
			buffer.alphaMax[4],
			buffer.alphaMax[5],
			buffer.alphaMax[6]
		);
		printf("    Alpha Min   %5d, %5d, %5d, %5d, %5d, %5d\n",
			buffer.alphaMin[0],
			buffer.alphaMin[1],
			buffer.alphaMin[2],
			buffer.alphaMin[3],
			buffer.alphaMin[4],
			buffer.alphaMin[5],
			buffer.alphaMin[6]
		);
		printf("    Vm          %5d, %5d, %5d, %5d, %5d, %5d\n",
			buffer.vm[0],
			buffer.vm[1],
			buffer.vm[2],
			buffer.vm[3],
			buffer.vm[4],
			buffer.vm[5],
			buffer.vm[6]
		);
		printf("    On/Off      %5d\n", buffer.alphaOn);
	}
	//
	// FILTER_LARGE_CHANGE
	//
	{
		BOOL buffer, *pBuffer = &buffer;
		errorCode = GetSensorParameter(sensorID, FILTER_LARGE_CHANGE, pBuffer, sizeof(buffer));
		if(errorCode!=BIRD_ERROR_SUCCESS) errorHandler(errorCode);
		printf("FILTER_LARGE_CHANGE: %d\n", buffer);
	}
	//
	// QUALITY
	//
	{
		QUALITY_PARAMETERS buffer, *pBuffer = &buffer;
		errorCode = GetSensorParameter(sensorID, QUALITY, pBuffer, sizeof(buffer));
		if(errorCode!=BIRD_ERROR_SUCCESS) errorHandler(errorCode);
		printf("QUALITY: %d, %d, %d, %d\n\n", 
			buffer.error_offset,
			buffer.error_sensitivity,
			buffer.error_slope,
			buffer.filter_alpha
			);
	}
}
template  <class Tin,class Tout> void TrakstarThread<Tin,Tout>::PrintTransmitterParameters(int transmitterID)
{
	int				errorCode;		// used to hold error code returned from procedure call
	//////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////
	//
	// GetTransmitterParameter()
	//
	// This procedure allows the user to inspect selected transmitter parameters. The 
	// procedure is directed at a specific transmitter by selecting that transmitter with 
	// the Transmitter ID parameter. For the purpose of this exercise the ID = 0. This 
	// is the 1st transmitter in a system. 
	//
	printf("\nGetTransmitterParameter()");
	printf("\n=========================\n");
	printf("Using transmitter ID = 0\n");

	//
	// REFERENCE_FRAME
	//
	{
		DOUBLE_ANGLES_RECORD buffer, *pBuffer = &buffer;
		errorCode = GetTransmitterParameter(transmitterID, REFERENCE_FRAME, pBuffer, sizeof(buffer));
		if(errorCode!=BIRD_ERROR_SUCCESS) errorHandler(errorCode);
		printf("REFERENCE_FRAME: %6.2f, %6.2f, %6.2f\n", 
			buffer.a,
			buffer.e,
			buffer.r	);
	}
	//
	// XYZ_REFERENCE_FRAME
	//
	{
		BOOL buffer, *pBuffer = &buffer;
		errorCode = GetTransmitterParameter(transmitterID, XYZ_REFERENCE_FRAME, pBuffer, sizeof(buffer));
		if(errorCode!=BIRD_ERROR_SUCCESS) errorHandler(errorCode);
		printf("XYZ_REFERENCE_FRAME: %d\n\n", buffer);
	}
}