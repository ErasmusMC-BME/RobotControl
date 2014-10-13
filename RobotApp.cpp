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

#define USEOPENCVTHREAD
//#define USELEGOTHREAD
//#define USEVIEWINGTHREAD
//#define USETIEPIETHREAD
//#define USERECONSTRUCTIONTHREAD
#define USETRAKSTARTHREAD
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
#include "NIGReconstructionThread.h"
#include "NIGViewingThread.h"
#include <stdlib.h> 
#include <vtkSmartPointer.h>

#include "utility.h"
#include "itkImageToVTKImageFilter.h"
#include "itkVTKImageToImageFilter.h"

#include <stdlib.h>


template <class Tin,class Tout>  OpenCVThread<Tin,Tout> *  OpenCVThread<Tin,Tout>::m_OpenCVthread=0;
template <class Tin,class Tout>  TrakstarThread<Tin,Tout> *  TrakstarThread<Tin,Tout>::m_Trakstarthread=0;
template <class Tin,class Tout>  LegoThread<Tin,Tout> *  LegoThread<Tin,Tout>::m_Legothread=0;
template <class Tin,class Tout>  ReconstructionThread<Tin,Tout> *  ReconstructionThread<Tin,Tout>::m_Reconstructionthread=0;
template <class Tin,class Tout>  ViewingThread<Tin,Tout> * ViewingThread<Tin,Tout>::m_Viewingthread=0;

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
	if( TrakStarThreadObj->isTrakstarFound())
	{
		TrakStarThreadObj->SetSync(syncTimer);
		TrakStarThreadObj->CreateRecorderThread();
	}
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
  if(LegoThreadObj->isLegoFound())
  {
 		LegoThreadObj->SetSync(syncTimer);	//
	  //LegoThreadObj->Calibrate();

		LegoThreadObj->BoolSend(bValue,MAILBOX_RESET);//init PID; start PID without Calibration, reset motor position values
		LegoThreadObj->WordSend(0,MAILBOX_A);//Z Plane + <---> -
		LegoThreadObj->WordSend(0,MAILBOX_B);//Angle 0 <---> -
		LegoThreadObj->WordSend(0,MAILBOX_C);//Image Plane  + <---> -
		LegoThreadObj->CreateRecorderThread();
	}
#endif	

#ifdef USERECONSTRUCTIONTHREAD
	// create Reconstruction object
	ReconstructionObjects * ReconstructionOut;
	ReconstructionThread<SharedObjects,ReconstructionObjects> *ReconstructionThreadObj;
	ReconstructionThreadObj=ReconstructionThread<SharedObjects,ReconstructionObjects>::New(); //Create thread
	ReconstructionThreadObj->SetInput(sharedobjs);
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



	//ImageType::Pointer clonedImage = (*m_VideoImage)[0]->GetOutput();

	//typedef itk::ImageToVTKImageFilter<ImageType> ImageConverterITKToVTK;
	//ImageConverterITKToVTK::Pointer ConverterPtr= ImageConverterITKToVTK::New();
	//ConverterPtr->SetInput((*m_VideoImage)[0]->GetOutput());
	//ConverterPtr->Update();


#ifdef USEVIEWINGTHREAD
	// createViewing object
	//	 ViewingnObjects *  ViewingOut;
	ViewingThread<SharedObjects,ViewingObjects> * ViewingThreadObj;
	ViewingThreadObj=ViewingThread<SharedObjects,ViewingObjects>::New(); //Create thread
	ViewingThreadObj->SetInput(sharedobjs);
	// initialize OpenCV object
	ViewingThreadObj->Initialize();
	ViewingThreadObj->SetSync(syncTimer);
	ViewingThreadObj->CreateRecorderThread();
#endif
#ifdef USEVIEWINGTHREAD
	ViewingThreadObj->StartRecorderThread();
#endif
	int cnt=0;
		int val=0;
#ifdef USEOPENCVTHREAD
	do // Start of while loop
	{



#ifdef USELEGOTHREAD
	

		if(LegoThreadObj->isLegoFound())
		{
			if (val%2)
			{
				(*m_CurrentMeasures)[1]=-(val)%90;
				(*m_CurrentMeasures)[2]=-(val)%90;
				(*m_CurrentMeasures)[3]=-(val)%5;

			}
			else
			{
				(*m_CurrentMeasures)[1]=(val)%90;
				(*m_CurrentMeasures)[2]=(val)%90;
				(*m_CurrentMeasures)[3]=(val)%5;

			}
	
			val +=2;
			Wait(2000);
		}
#endif

		if( OpenCVThreadObj->isOpenCVFound())
		{
			VideoOut=OpenCVThreadObj->GetOutput();
			_timeVideo=VideoOut->GetTimeVideo();
			_VideoImage=VideoOut->GetVideoImage();



			utility<double> helper;
			int ActualFrameNr =*OpenCVThreadObj->GetOutput()->GetActualFrameNr();
			if(ActualFrameNr==40)
			{

#ifdef USETRAKSTARTHREAD
				if( TrakStarThreadObj->isTrakstarFound())
				{
					TrakStarThreadObj->WaitUntilRecorderThreadIsDone();
					// retrieve and save data
					TrakstarOut = TrakStarThreadObj->GetOutput();
					std::vector<vnl_matrix<double>> positionMeasurements = TrakstarOut->GetMeasures();
				  int   NumSensors=	TrakStarThreadObj->GetNumSensors();

					// change the data saving into something more generic (Alex 01-May)
					if(NumSensors) helper.matlabSaveVnlMatrix( dataPath + "TrakStarDataCh1_" + acquisitionTag + ".mat", positionMeasurements[0], "TrakStarDataCh1" + acquisitionTag );
					if(NumSensors>1) helper.matlabSaveVnlMatrix( dataPath + "TrakStarDataCh2_" + acquisitionTag + ".mat", positionMeasurements[1], "TrakStarDataCh2" + acquisitionTag );
					if(NumSensors>2) helper.matlabSaveVnlMatrix( dataPath + "TrakStarDataCh3_" + acquisitionTag + ".mat", positionMeasurements[2], "TrakStarDataCh3" + acquisitionTag );

				}
#endif


				WriterType::Pointer writer = WriterType::New();
				writer->SetFileName("d:\\cam24.tif");
				ImageType::Pointer clonedImage = (*_VideoImage)[0]->GetOutput();
				writer->SetInput(clonedImage);
				writer->Update();

				helper.matlabSaveVnlVector( dataPath + "VideoTime" + acquisitionTag + ".mat", *_timeVideo, "VideoTime" + acquisitionTag );
			}
		}
		cnt++;
	} while (1);//while (cnt<5);
#endif
	#ifdef USELEGOTHREAD
//	LegoThreadObj->Close(  );
	#endif
	return 0;
	// wait for the measurement threads to finish


#ifdef USETRAKSTARTHREAD
	if( TrakStarThreadObj->isTrakstarFound())
	{
		TrakStarThreadObj->WaitUntilRecorderThreadIsDone();
		// retrieve and save data
		TrakstarOut = TrakStarThreadObj->GetOutput();
		std::vector<vnl_matrix<double>> positionMeasurements = TrakstarOut->GetMeasures();
  
		// change the data saving into something more generic (Alex 01-May)
		helper.matlabSaveVnlMatrix( dataPath + "TrakStarDataCh1_" + acquisitionTag + ".mat", positionMeasurements[0], "TrakStarDataCh1" + acquisitionTag );
		helper.matlabSaveVnlMatrix( dataPath + "TrakStarDataCh2_" + acquisitionTag + ".mat", positionMeasurements[1], "TrakStarDataCh2" + acquisitionTag );
		helper.matlabSaveVnlMatrix( dataPath + "TrakStarDataCh3_" + acquisitionTag + ".mat", positionMeasurements[2], "TrakStarDataCh3" + acquisitionTag );
	}
#endif

#ifdef USETIEPIETHREAD
	TiepieThreadObj->WaitUntilRecorderThreadIsDone();
	//helper.matlabSaveVnlVector( dataPath + "TiePieTime" + acquisitionTag + ".mat", TiepieOut->GetTimeCh1(), "TiePieTime" + acquisitionTag );
	//helper.matlabSaveVnlVector( dataPath + "TiePieVoltage" + acquisitionTag  +  ".mat", TiepieOut->GetVoltageCh1(), "TiePieVoltage" + acquisitionTag );
#endif


#ifdef USEOPENCVTHREAD
OpenCVThreadObj->WaitUntilRecorderThreadIsDone();
#endif

#ifdef USELEGOTHREAD
val=0;

do 
{
	if(LegoThreadObj->isLegoFound())
	{

		//	(*m_CurrentMeasures)[1]=(val)%90;
		//(*m_CurrentMeasures)[2]=(val)%90;
			(*m_CurrentMeasures)[3]=(val)%5;
		val +=2;
		Wait(5000);
	}
} while (1);

LegoThreadObj->WaitUntilRecorderThreadIsDone();
LegoOut = LegoThreadObj->GetOutput();

#if 0
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
Beep( 750, 300 );
#endif
#endif
  std::cin.ignore();
	return 0;
}

