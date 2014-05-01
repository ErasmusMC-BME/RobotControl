// NXT++ test.cpp : Defines the entry point for the console application.
//
//LEGO 

#include "Timer.h"

#include <iostream>
#include <fstream>
#include <iostream>
#include <time.h>
#include <stdio.h>
#include <windows.h>          // for HANDLE
#include <process.h>    /* CreateRecorderThread, _endthread */

//#define USEOPENCVTHREAD
#define USELEGOTHREAD
//#define USETIEPIETHREAD
//#define USETRAKSTARTHREAD
//TiePie
#include "TiePieDLL.h"
#include "tiepie.h"

// trakstar
#include <ATC3DG.h>
#include <sample2.h>
#include "NIGLegoThread.h"
#include "NIGTiepieThread.h"
#include "NIGTrakstarThread.h"
#include "NIGOpenCVThread.h"
#include <stdlib.h> 
#include <vtkSmartPointer.h>

#include "utility.h"
#include "itkImageToVTKImageFilter.h"
#include "itkVTKImageToImageFilter.h"

#include <stdlib.h>


template <class Tin,class Tout>  OpenCVThread<Tin,Tout> *  OpenCVThread<Tin,Tout>::m_OpenCVthread=0;
template <class Tin,class Tout>  TrakstarThread<Tin,Tout> *  TrakstarThread<Tin,Tout>::m_Trakstarthread=0;
template <class Tin,class Tout>  LegoThread<Tin,Tout> *  LegoThread<Tin,Tout>::m_Legothread=0;

template <class Tin,class Tout>  thread<Tin,Tout> *  thread<Tin,Tout>::m_thread=0;


int main()
{
	// user variables
	utility<double> helper;
	double acquisitionTime = 30; // (sec)
	std::string acquisitionTag = "010";
	//std::string dataPath = "D:/Data/ExperimentalData/CalibrationThreeWirePhantom/InitExperiment_19-Jan-2014/";
	std::string dataPath = "D:/Data/";
	int CommandNr=0;

	vnl_vector<double> *_timeVideo;
	std::vector<DuplicatorType::Pointer> *_VideoImage;

	// create sharedobjs object
	SharedObjects * sharedobjs = new SharedObjects(); 

	vnl_vector<double> * m_CurrentMeasures=sharedobjs->GetCurrentMeasures();

	// create sync  object	and start timer;
	SyncTimerObject * syncTimer = new SyncTimerObject();
	syncTimer->start();	
	
	// create threads 	
	#ifdef USETRAKSTARTHREAD

	// create TraKStar object
	TrakstarObjects * TrakstarOut;
	TrakstarThread<SharedObjects,TrakstarObjects> *TrakStarThreadObj;
	TrakStarThreadObj=TrakstarThread<SharedObjects,TrakstarObjects>::New(); //Create thread
	TrakStarThreadObj->SetInput(sharedobjs);

	// initialize TraKStar object
	double df_TrakStar = 80; // (Hz)
	int recordLength_TrakStar = ceil( df_TrakStar * acquisitionTime );
	TrakStarThreadObj->Initialize("id", recordLength_TrakStar, df_TrakStar  );
	TrakStarThreadObj->SetSync(syncTimer);
	TrakStarThreadObj->CreateRecorderThread();
#endif

#ifdef USETIEPIETHREAD
	// create Tiepie object
	TiepieObjects * TiepieOut;
	TiepieThread<SharedObjects,TiepieObjects> *TiepieThreadObj;
	TiepieThreadObj=TiepieThread<SharedObjects,TiepieObjects>::New(); //Create thread
	TiepieThreadObj->SetInput(sharedobjs);

	// initialize Tiepie object	
	double df_Tiepie = 1000.0; // (Hz)
	double sensCh1_Tiepie = 1.0;
	dword recordLength_Tiepie = (dword) ceil( df_Tiepie * acquisitionTime );
	TiepieThreadObj->Initialize("Wdd",  recordLength_Tiepie, sensCh1_Tiepie, df_Tiepie  );
	TiepieThreadObj->SetSync(syncTimer);
	TiepieThreadObj->CreateRecorderThread();
#endif

#ifdef USEOPENCVTHREAD
		// create OpenCV object
	OpenCVObjects * VideoOut;
	OpenCVThread<SharedObjects,OpenCVObjects> *OpenCVThreadObj;
	OpenCVThreadObj=OpenCVThread<SharedObjects,OpenCVObjects>::New(); //Create thread
	OpenCVThreadObj->SetInput(sharedobjs);
	
	// initialize OpenCV object
	OpenCVThreadObj->Initialize();
	OpenCVThreadObj->SetSync(syncTimer);
	OpenCVThreadObj->CreateRecorderThread();
#endif

	bool bValue=true;

#ifdef USELEGOTHREAD
	// create Lego object
	LegoObjects * LegoOut;
	LegoThread<SharedObjects,LegoObjects> *LegoThreadObj;
	LegoThreadObj=LegoThread<SharedObjects,LegoObjects>::New(); //Create thread
	LegoThreadObj->SetInput(sharedobjs);
	
	// initialize Lego object
	LegoThreadObj->Initialize();
	LegoThreadObj->SetSync(syncTimer);	//
//	LegoThreadObj->Calibrate();
	if(LegoThreadObj->isLegoFound())
	{
		LegoThreadObj->BoolSend(bValue,MAILBOX_RESET);//init PID; start PID without Calibration, reset motor position values
		LegoThreadObj->WordSend(0,MAILBOX_A);//Z Plane + <---> -
		LegoThreadObj->WordSend(0,MAILBOX_B);//Angle 0 <---> -
		LegoThreadObj->WordSend(0,MAILBOX_C);//Image Plane  + <---> -

		LegoThreadObj->CreateRecorderThread();
	}
#endif	

#ifdef USETRAKSTARTHREAD
	TrakStarThreadObj->StartRecorderThread();
#endif

#ifdef USEOPENCVTHREAD
	OpenCVThreadObj->StartRecorderThread();
#endif
#ifdef USETIEPIETHREAD
	TiepieThreadObj->StartRecorderThread();
#endif

#ifdef USELEGOTHREAD
	if(LegoThreadObj->isLegoFound())
	{
		LegoThreadObj->StartRecorderThread();
	}

#endif
#ifdef USEOPENCVTHREAD
	do 
	{
		if( OpenCVThreadObj->isOpenCVFound())
		{
			VideoOut=OpenCVThreadObj->GetOutput();
			_timeVideo=VideoOut->GetTimeVideo();
			_VideoImage=VideoOut->GetVideoImage();

			//std::cout << "_timeVideo  " << CommandNr << (*_timeVideo)[CommandNr]  << std::endl;
			WriterType::Pointer writer = WriterType::New();
			writer->SetFileName("d:\\cam23.tif");
			ImageType::Pointer clonedImage = (*_VideoImage)[0]->GetOutput();
			writer->SetInput(clonedImage);
			writer->Update();
			utility<double> helper;
			helper.matlabSaveVnlVector( dataPath + "VideoTime" + acquisitionTag + ".mat", *_timeVideo, "VideoTime" + acquisitionTag );
		}
	} while (1);
#endif
	// wait for the measurement threads to finish


#ifdef USETRAKSTARTHREAD
	TrakStarThreadObj->WaitUntilRecorderThreadIsDone();
	// retrieve and save data
	TrakstarOut = TrakStarThreadObj->GetOutput();
	vnl_matrix<double> _measures= TrakstarOut->GetMeasures();
	helper.matlabSaveVnlMatrix( dataPath + "TrakStarData" + acquisitionTag + ".mat", TrakstarOut->GetMeasures(), "TrakStarData" + acquisitionTag );
#endif

#ifdef USETIEPIETHREAD
	TiepieThreadObj->WaitUntilRecorderThreadIsDone();
	//helper.matlabSaveVnlVector( dataPath + "TiePieTime" + acquisitionTag + ".mat", TiepieOut->GetTimeCh1(), "TiePieTime" + acquisitionTag );
	//helper.matlabSaveVnlVector( dataPath + "TiePieVoltage" + acquisitionTag  +  ".mat", TiepieOut->GetVoltageCh1(), "TiePieVoltage" + acquisitionTag );
#endif


#ifdef USEOPENCVTHREAD
OpenCVThreadObj->WaitUntilRecorderThreadIsDone();
#endif
	int val=0;


#ifdef USELEGOTHREAD
do 
{

	if(LegoThreadObj->isLegoFound())
	{
		
	//	(*m_CurrentMeasures)[1]=(val)%90;
		(*m_CurrentMeasures)[2]=(val)%90;
	//	(*m_CurrentMeasures)[3]=(val)%5;
		val +=2;
		Wait(5000);
	}
} while (1);
#endif

#ifdef USELEGOTHREAD
	LegoThreadObj->WaitUntilRecorderThreadIsDone();
	LegoOut = LegoThreadObj->GetOutput();

	Wait(2000);
	if(LegoThreadObj->isLegoFound())
	{

		// should be done in a thread

	//LegoThreadObj->BoolSend(bValue,MAILBOX_INIT);//init PID + Calibration steps ;
	LegoThreadObj->BoolSend(bValue,MAILBOX_RESET);//init PID;
	LegoThreadObj->WordSend(0,MAILBOX_A);//Z Plane + <---> -
	LegoThreadObj->WordSend(0,MAILBOX_B);//Angle 0 <---> -
	LegoThreadObj->WordSend(0,MAILBOX_C);//Image Plane  + <---> -
	LegoThreadObj->BoolSend(bValue,MAILBOX_START);// Move motors to 0 Position
	LegoThreadObj->WaitForRotationIdle();


	double Anglefactor=450/90;
	double Zfactor=26026/15;
	double Imagefactor=804/190;


	double zz,aa,bb;
	zz=-1*Zfactor;  // corresponds to 1 CM (-7.5.. +7.5)
	aa=-45*Anglefactor; // corresponds to -45 deg (-90.. +90)
	bb=-45*Imagefactor; // corresponds to -45 deg  (-90.. +90)
	LegoThreadObj->WordSend(zz,MAILBOX_A);//Z Plane + <---> -
	LegoThreadObj->WordSend(aa,MAILBOX_B);//Angle 0 <---> -
	LegoThreadObj->WordSend(bb,MAILBOX_C);//Image Plane  + <---> -
	LegoThreadObj->BoolSend(bValue,MAILBOX_START);//Start Motors
	LegoThreadObj->WaitForRotationIdle();
	
	LegoThreadObj->WordSend(0,MAILBOX_A);//Z Plane + <---> -
	LegoThreadObj->WordSend(0,MAILBOX_B);//Angle 0 <---> -
	LegoThreadObj->WordSend(0,MAILBOX_C);//Image Plane  + <---> -
	LegoThreadObj->BoolSend(bValue,MAILBOX_START);// Move motors to 0 Position
	LegoThreadObj->WaitForRotationIdle();
	}
#endif

//	getchar();	


	Beep( 750, 300 );
	//getchar();	
	return 0;
}

