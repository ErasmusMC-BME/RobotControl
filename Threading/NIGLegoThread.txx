// NXT++ test.cpp : Defines the entry point for the console application.
//
#include <iostream>
#include "NIGLegoThread.h"

#include "vnl/vnl_vector.h"
//#include "vxlutility.h"


// Base Function definitions
template  <class Tin,class Tout>  LegoThread<Tin,Tout>::LegoThread( void )
{
	_bLegoReady = false;
	_bLegoFound = false;
}

template  <class Tin,class Tout>  LegoThread<Tin,Tout>::~LegoThread( void )
{
}

template <class Tin,class Tout>  LegoThread<Tin,Tout> *  LegoThread<Tin,Tout>::New( thread<Tin,Tout> * _thread=NULL )  // Create the object
{
	if(m_Legothread==NULL)
	{
		m_Legothread= new LegoThread;
	}
	return (LegoThread<Tin,Tout> * ) m_Legothread;
}

template  <class Tin,class Tout>  void  LegoThread<Tin,Tout>::Initialize(const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);

	while (*fmt != '\0') {
		if (*fmt == 'd') {
			int i = va_arg(args, int);
			std::cout << i << '\n';
		} else if (*fmt == 's') {
			char * s = va_arg(args, char*);
			std::cout << s << '\n';
		}
		++fmt;
	}
	va_end(args);
};

template  <class Tin,class Tout>  void LegoThread<Tin,Tout>::ThreadEntryPoint( void )
{
	ExecuteCommand(); 
	// This is the desired entry-point-function but to get
	// here we have to use a 2 step procedure involving
	// the ThreadStaticEntryPoint() function.

}

template  <class Tin,class Tout>  void LegoThread<Tin,Tout>::Update( void )
{

}


template  <class Tin,class Tout> void LegoThread<Tin,Tout>::Initialize()
{  
	init();
	//calibration();
}
template  <class Tin,class Tout> void LegoThread<Tin,Tout>::Calibrate()
{  
	calibration();
}
template  <class Tin,class Tout> void LegoThread<Tin,Tout>::ExecuteCommand()
{  
	char cmds[4][12]={"probeUp","ProbeDown","ProbeLeft","Proberight"};


	std::string acquisitionTag = "010";
	std::string dataPath = "D:/Data/ExperimentalData/CalibrationThreeWirePhantom/InitExperiment_19-Jan-2014/";

	vnl_vector<double> _voltageCh1;
	vnl_vector<double> _timeCh1;
	//double Time = m_timer->getElapsedTimeInSec();
	//std::cout << "Start Time Lego: " << Time << std::endl;

	//m_sharedobjs->WaitForSync();
	_timeCh1= m_sharedobjs->GetTimeCh1();
	_voltageCh1=m_sharedobjs->GetVoltageCh1();
	float z = 1;
	float a = 0;
	float b=  0;
	move_relative(z, a, b);
	//utility<double> helper;
	//helper.matlabSaveVnlVector( dataPath + "TiePieTime" + acquisitionTag +  cmds[_LegoCommand] + ".mat", _timeCh1, "TiePieTime" + acquisitionTag );
	//helper.matlabSaveVnlVector( dataPath + "TiePieVoltage" + acquisitionTag + cmds[_LegoCommand] + ".mat", _voltageCh1, "TiePieVoltage" + acquisitionTag );
	//Time = m_timer->getElapsedTimeInSec();
	//std::cout << "End Time Lego: " << Time << std::endl;
	//std::cout << "LegoCommand: " << _LegoCommand << std::endl;
	_endthread();
}


