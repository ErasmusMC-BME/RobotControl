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

	//// create Tiepie object
	//TiepieObjects * TiepieOut;
	//TiepieThread<SharedObjects,TiepieObjects> *TiepieThreadObj;
	//TiepieThreadObj=TiepieThread<SharedObjects,TiepieObjects>::New(); //Create thread
	//TiepieThreadObj->SetInput(sharedobjs);

	//// initialize Tiepie object	
	//double df_Tiepie = 1000.0; // (Hz)
	//double sensCh1_Tiepie = 1.0;
	//dword recordLength_Tiepie = (dword) ceil( df_Tiepie * acquisitionTime );
	//TiepieThreadObj->Initialize("Wdd",  recordLength_Tiepie, sensCh1_Tiepie, df_Tiepie  );
	//TiepieThreadObj->SetSync(syncTimer);
	//TiepieThreadObj->CreateRecorderThread();

	//	// create OpenCV object
	//OpenCVObjects * VideoOut;
	//OpenCVThread<SharedObjects,OpenCVObjects> *OpenCVThreadObj;
	//OpenCVThreadObj=OpenCVThread<SharedObjects,OpenCVObjects>::New(); //Create thread
	//OpenCVThreadObj->SetInput(sharedobjs);
	
	//// initialize OpenCV object
	//OpenCVThreadObj->Initialize();
	//OpenCVThreadObj->SetSync(syncTimer);
	//OpenCVThreadObj->CreateRecorderThread();

	// create Lego object
	bool bValue=true;
	LegoObjects * LegoOut;
	LegoThread<SharedObjects,LegoObjects> *LegoThreadObj;
	LegoThreadObj=LegoThread<SharedObjects,LegoObjects>::New(); //Create thread
	LegoThreadObj->SetInput(sharedobjs);
	
	// initialize Lego object
	LegoThreadObj->Initialize();
	LegoThreadObj->SetSync(syncTimer);
  LegoThreadObj->Calibrate();
	LegoThreadObj->CreateRecorderThread();
		
	TrakStarThreadObj->StartRecorderThread();
	//OpenCVThreadObj->StartRecorderThread();
	//TiepieThreadObj->StartRecorderThread();

	if(LegoThreadObj->isLegoFound())
	{
		LegoThreadObj->StartRecorderThread();
	}



	//do 
	//{
	//	if( OpenCVThreadObj->isOpenCVFound())
	//	{
	//		VideoOut=OpenCVThreadObj->GetOutput();
	//		_timeVideo=VideoOut->GetTimeVideo();
	//		_VideoImage=VideoOut->GetVideoImage();

	//		//std::cout << "_timeVideo  " << CommandNr << (*_timeVideo)[CommandNr]  << std::endl;
	//		WriterType::Pointer writer = WriterType::New();
	//		writer->SetFileName("d:\\cam23.tif");
	//		ImageType::Pointer clonedImage = (*_VideoImage)[0]->GetOutput();
	//		writer->SetInput(clonedImage);
	//		writer->Update();
	//		utility<double> helper;
	//		helper.matlabSaveVnlVector( dataPath + "VideoTime" + acquisitionTag + ".mat", *_timeVideo, "VideoTime" + acquisitionTag );


	//	}
	//} while (1);

	//// wait for the measurement threads to finish



	TrakStarThreadObj->WaitUntilRecorderThreadIsDone();
	/*TiepieThreadObj->WaitUntilRecorderThreadIsDone();*/
	//OpenCVThreadObj->WaitUntilRecorderThreadIsDone();
	// retrieve and save data
	TrakstarOut = TrakStarThreadObj->GetOutput();
	vnl_matrix<double> _measures= TrakstarOut->GetMeasures();

	LegoThreadObj->WaitUntilRecorderThreadIsDone();
	LegoOut = LegoThreadObj->GetOutput();


	if(LegoThreadObj->isLegoFound())
	{

		// should be done in a thread

	//LegoThreadObj->BoolSend(bValue,MAILBOX_INIT);//init PID + Calibration steps ;
	LegoThreadObj->BoolSend(bValue,MAILBOX_RESET);//init PID;
	LegoThreadObj->WordSend(0,MAILBOX_A);//Z Plane + <---> -
	LegoThreadObj->WordSend(0,MAILBOX_B);//Angle 0 <---> -
	LegoThreadObj->WordSend(0,MAILBOX_C);//Image Plane  + <---> -
	LegoThreadObj->BoolSend(bValue,MAILBOX_START);// Move motors to 0 Position


	Wait(1000);
	double Anglefactor=450/90;
	double Zfactor=26026/15;
	double Imagefactor=804/190;


	double zz,aa,bb;
	zz=-1*Zfactor; // corresponds to 1 cm
	aa=-45*Anglefactor; // corresponds to 45 deg
	bb=-45*Imagefactor; // corresponds top 45 deg
	LegoThreadObj->WordSend(zz,MAILBOX_A);//Z Plane + <---> -
	LegoThreadObj->WordSend(aa,MAILBOX_B);//Angle 0 <---> -
	LegoThreadObj->WordSend(bb,MAILBOX_C);//Image Plane  + <---> -
	LegoThreadObj->BoolSend(bValue,MAILBOX_START);//Start Motors

	LegoThreadObj->WordSend(0,MAILBOX_A);//Z Plane + <---> -
	LegoThreadObj->WordSend(0,MAILBOX_B);//Angle 0 <---> -
	LegoThreadObj->WordSend(0,MAILBOX_C);//Image Plane  + <---> -
	LegoThreadObj->BoolSend(bValue,MAILBOX_START);// Move motors to 0 Position

	}




	utility<double> helper;
	//helper.matlabSaveVnlVector( dataPath + "TiePieTime" + acquisitionTag + ".mat", TiepieOut->GetTimeCh1(), "TiePieTime" + acquisitionTag );
	//helper.matlabSaveVnlVector( dataPath + "TiePieVoltage" + acquisitionTag  +  ".mat", TiepieOut->GetVoltageCh1(), "TiePieVoltage" + acquisitionTag );
	helper.matlabSaveVnlMatrix( dataPath + "TrakStarData" + acquisitionTag + ".mat", TrakstarOut->GetMeasures(), "TrakStarData" + acquisitionTag );
	//

	getchar();	


	Beep( 750, 300 );
	//getchar();	
	return 0;
}

