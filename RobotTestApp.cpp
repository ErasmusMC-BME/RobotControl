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

#include <vtkImageCanvasSource2D.h>
#include <vtkAVIWriter.h>
#include "utility.h"
#include "itkImageToVTKImageFilter.h"
#include "itkVTKImageToImageFilter.h"
enum
{
	probeUp,
	ProbeDown,
	ProbeLeft,
	Proberight

}LegoCommand;


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

	// create sync  object	and start timer;
	SyncTimerObject * syncTimer = new SyncTimerObject();
	syncTimer->start();	
	
	// create threads 	
	

	// create TraKStar object
	TrakstarObjects * TrakstarOut;
	TrakstarThread<SharedObjects,TrakstarObjects> *TrakStarThreadObj;
	TrakStarThreadObj=TrakstarThread<SharedObjects,TrakstarObjects>::New(); //Create thread
	TrakStarThreadObj->SetInput(sharedobjs);
#if 0
	// initialize TraKStar object
	double df_TrakStar = 80; // (Hz)
	int recordLength_TrakStar = ceil( df_TrakStar * acquisitionTime );
	TrakStarThreadObj->Initialize("id", recordLength_TrakStar, df_TrakStar  );
	TrakStarThreadObj->SetSync(syncTimer);
	TrakStarThreadObj->Update(); // nothing yet (not sure if we use)
	TrakStarThreadObj->CreateRecorderThread();
	TrakStarThreadObj->StartRecorderThread();
	// wait for the measurement threads to finish
	TrakStarThreadObj->WaitUntilRecorderThreadIsDone();
	// retrieve and save data
	TrakstarOut = TrakStarThreadObj->GetOutput();
	vnl_matrix<double> _measures= TrakstarOut->GetMeasures();

#endif
//	utility<double> helper;
	//helper.matlabSaveVnlVector( dataPath + "TiePieTime" + acquisitionTag + ".mat", TiepieOut->GetTimeCh1(), "TiePieTime" + acquisitionTag );
	//helper.matlabSaveVnlVector( dataPath + "TiePieVoltage" + acquisitionTag  +  ".mat", TiepieOut->GetVoltageCh1(), "TiePieVoltage" + acquisitionTag );
//	helper.matlabSaveVnlMatrix( dataPath + "TrakStarData" + acquisitionTag + ".mat", TrakstarOut->GetMeasures(), "TrakStarData" + acquisitionTag );
//



	// create Lego object
	bool bValue=true;
	LegoThread<SharedObjects,LegoObjects> * self= new  LegoThread<SharedObjects,LegoObjects>();
	LegoObjects * LegoOut;
	LegoThread<SharedObjects,LegoObjects> *LegoThreadObj;
	LegoThreadObj=LegoThread<SharedObjects,LegoObjects>::New(); //Create thread

	
	LegoThreadObj->SetInput(sharedobjs);
	LegoThreadObj->Initialize();

	LegoThreadObj->SetSync(syncTimer);
	//LegoThreadObj->BoolSend(bValue,MAILBOX_INIT);//init PID;
	if(LegoThreadObj->isLegoFound())
	{



//	Wait(5000);
	LegoThreadObj->BoolSend(bValue,MAILBOX_RESET);//init PID;
	LegoThreadObj->WordSend(0,MAILBOX_A);//Z Plane + <---> -
	LegoThreadObj->WordSend(0,MAILBOX_B);//Angle 0 <---> -
	LegoThreadObj->WordSend(0,MAILBOX_C);//Image Plane  + <---> -
	LegoThreadObj->BoolSend(bValue,MAILBOX_START);//Start Motor


	Wait(1000);
	double Anglefactor=450/90;
	double Zfactor=26026/15;
	double Imagefactor=804/190;


	double zz,aa,bb;
	zz=-1*Zfactor;
	aa=-45*Anglefactor;
	bb=-45*Imagefactor;
	LegoThreadObj->WordSend(zz,MAILBOX_A);//Z Plane + <---> -
	LegoThreadObj->WordSend(aa,MAILBOX_B);//Angle 0 <---> -
	LegoThreadObj->WordSend(bb,MAILBOX_C);//Image Plane  + <---> -
	LegoThreadObj->BoolSend(bValue,MAILBOX_START);//Start Motor

	LegoThreadObj->WordSend(0,MAILBOX_A);//Z Plane + <---> -
	LegoThreadObj->WordSend(0,MAILBOX_B);//Angle 0 <---> -
	LegoThreadObj->WordSend(0,MAILBOX_C);//Image Plane  + <---> -
	LegoThreadObj->BoolSend(bValue,MAILBOX_START);//Start Motor
	Wait(3000);	
	LegoThreadObj->TextMessageRecieve(7,false);
	LegoThreadObj->TextMessageRecieve(MAILBOX_RECIEVE,false);
	LegoThreadObj->TextMessageRecieve(9,false);
	Wait(2000);
	//LegoThreadObj->TextMessageRecieve(MAILBOX_RECIEVE,true);
	}

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
	TiepieThreadObj->TestCalibrationDate();
	
	TiepieThreadObj->CreateRecorderThread();
	TiepieThreadObj->StartRecorderThread();
	TiepieThreadObj->WaitUntilRecorderThreadIsDone();
	TiepieThreadObj->GetOutput();
	
	
	
	LegoThreadObj->CreateRecorderThread();
	LegoThreadObj->StartRecorderThread();
	LegoThreadObj->WaitUntilRecorderThreadIsDone();
	LegoOut = LegoThreadObj->GetOutput();





#if 0
	OpenCVObjects * VideoOut;
	OpenCVThread<SharedObjects,OpenCVObjects> *OpenCVThreadObj;
	OpenCVThreadObj=OpenCVThread<SharedObjects,OpenCVObjects>::New(); //Create thread
	OpenCVThreadObj->SetInput(sharedobjs);
	OpenCVThreadObj->Initialize();
	OpenCVThreadObj->SetSync(syncTimer);
	OpenCVThreadObj->Update();
	OpenCVThreadObj->CreateRecorderThread();
	OpenCVThreadObj->StartRecorderThread();

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
	OpenCVThreadObj->WaitUntilRecorderThreadIsDone();


	getchar();	

	//utility<double> helper;
	//helper.matlabSaveVnlVector( dataPath + "TiePieTime" + acquisitionTag + ".mat", TiepieThreadObj->GetTimeCh1(), "TiePieTime" + acquisitionTag );
	//helper.matlabSaveVnlVector( dataPath + "TiePieVoltage" + acquisitionTag  +  ".mat", TiepieThreadObj->GetVoltageCh1(), "TiePieVoltage" + acquisitionTag );
	//TrakstarObjects * Out;

	// start threads
	TiepieThreadObj->StartRecorderThread();
	TrakStarThreadObj->StartRecorderThread();

	// wait for the measurement threads to finish
	TiepieThreadObj->WaitUntilRecorderThreadIsDone();
	TrakStarThreadObj->WaitUntilRecorderThreadIsDone();
	

	// retrieve and save data
	TiepieOut = TiepieThreadObj->GetOutput();
	TrakstarOut = TrakStarThreadObj->GetOutput();



	getchar();	
	
//	if( VideoThreadObj->isVideoTFound())
	{
		  std::cout << "TiePie started measuring at time: " <<  std::endl;
		//_timeVideo=sharedobjs->GetTimeVideo();
		//_VideoImage=sharedobjs->GetVideoImage();

		//std::cout << "_timeVideo  " << CommandNr << (*_timeVideo)[CommandNr]  << std::endl;
		std::vector<DuplicatorType::Pointer> *m_VideoImage;
	//	m_VideoImage=sharedobjs->GetVideoImage();
	
		ImageType::Pointer clonedImage = (*m_VideoImage)[0]->GetOutput();
		
		typedef itk::ImageToVTKImageFilter<ImageType> ImageConverterITKToVTK;
		ImageConverterITKToVTK::Pointer ConverterPtr= ImageConverterITKToVTK::New();
		ConverterPtr->SetInput((*m_VideoImage)[0]->GetOutput());
		ConverterPtr->Update();
		
		//maintest() ;
		ImageType::RegionType iRegion;
		iRegion = clonedImage->GetBufferedRegion();
		ImageType::SizeType	extractSize=iRegion.GetSize();;
		vtkSmartPointer<vtkImageData> source =
			vtkSmartPointer<vtkImageData>::New();
	
		source->SetScalarTypeToChar();
		source->SetNumberOfScalarComponents(1);
		source->SetExtent(0, extractSize[0], 0, extractSize[1], 0, 0);
		source->DeepCopy(ConverterPtr->GetOutput());
		source->Update();
	
		// Create AVI Writer
		/*vtkSmartPointer<vtkImageCanvasSource2D> source =
			vtkSmartPointer<vtkImageCanvasSource2D>::New();
		source->SetScalarTypeToUnsignedShort();
		source->SetNumberOfScalarComponents(3);
		source->SetExtent(0, extractSize[0], 0, extractSize[1], 0, 0);
		source->Update();*/
		vtkSmartPointer<vtkAVIWriter> writer = 
			vtkSmartPointer<vtkAVIWriter>::New();
		
		//writer->SetInput(source->GetOutput());
		writer->SetInput(source);
		writer->SetFileName("d:/test.avi");
		writer->SetRate(30);
		writer->Start();
	
		getchar();	
		for(unsigned int i = 0; i < 100; i++)
		{
			ConverterPtr->SetInput((*m_VideoImage)[i]->GetOutput());
			ConverterPtr->Update();
			source->SetScalarTypeToChar();
			source->SetNumberOfScalarComponents(1);
			source->SetExtent(0, extractSize[0], 0, extractSize[1], 0, 0);
			source->DeepCopy(ConverterPtr->GetOutput());
			source->Update();

			writer->SetInput(source);
			//writer->SetInput(ConverterPtr->GetOutput());
			//source->SetInput(ConverterPtr->GetOutput());
			//source->SetDrawColor(0,0,0,1); //black
			//source->FillBox(0, extractSize[0], 0, extractSize[1]); //clear image
			//source->SetDrawColor(255,0,0,1); //red
			//source->FillBox(i, 20, 10, 20);
			writer->Update();

			writer->Write();
		}
		writer->End();	
	}
	
	  std::cout << "TiePie ended measuring at time: " <<  std::endl;
	//vtkSmartPointer<vtkJPEGReader> reader = 
	//  vtkSmartPointer<vtkJPEGReader>::New();
	
#endif
	getchar();	


	Beep( 750, 300 );
	//getchar();	
	return 0;
}

int mainAlex()
{
	// user variables
	double acquisitionTime = 30; // (sec)
	std::string acquisitionTag = "010";
	std::string dataPath = "D:/Data/ExperimentalData/CalibrationThreeWirePhantom/InitExperiment_19-Jan-2014/";

	// create sharedobjs object
	SharedObjects * sharedobjs = new SharedObjects(); 

	// create sync  object	and start timer;
	SyncTimerObject * syncTimer = new SyncTimerObject();
	syncTimer->start();	


	// create Tiepie object
	TiepieObjects * TiepieOut;
	TiepieThread<SharedObjects,TiepieObjects> *TiepieThreadObj;
	TiepieThreadObj=TiepieThread<SharedObjects,TiepieObjects>::New(); //Create thread
	TiepieThreadObj->SetInput(sharedobjs);
	double df_Tiepie = 1000.0; // (Hz)
	double sensCh1_Tiepie = 1.0;
	dword recordLength_Tiepie = (dword) ceil( df_Tiepie * acquisitionTime );
	//TiepieThreadObj->PrepareCh1ForAcq( recordLength_Tiepie, &sensCh1_Tiepie, &df_Tiepie );
	//TiepieThreadObj->Initialize();
	TiepieThreadObj->Initialize("Wdd",  recordLength_Tiepie, sensCh1_Tiepie, df_Tiepie  );
	TiepieThreadObj->SetSync(syncTimer);

	
	// create TraKStar object
	TrakstarObjects * TrakstarOut;
	TrakstarThread<SharedObjects,TrakstarObjects> *TrakStarThreadObj;
	TrakStarThreadObj=TrakstarThread<SharedObjects,TrakstarObjects>::New(); //Create thread
	TrakStarThreadObj->SetInput(sharedobjs);
	
	// initialize TraKStar object
	double df_TrakStar = 80; // (Hz)
	int recordLength_TrakStar = ceil( df_TrakStar * acquisitionTime );
	//TrakStarThreadObj->SetNumSamples(recordLength_TrakStar);
	//TrakStarThreadObj->SetsamplingFreq(df_TrakStar);
	//TrakStarThreadObj->Initialize();
	TrakStarThreadObj->Initialize("id", recordLength_TrakStar, df_TrakStar  );
	TrakStarThreadObj->SetSync(syncTimer);



	// create Tiepie acquisition handles
	//Tiepiehandle = (HANDLE)CreateRecorderThreadex(NULL, 0, TiepieThread::ThreadStaticStartMeasurementCh1EntryPoint, TiepieThreadObj, CREATE_SUSPENDED, &Tiepieid);
	TiepieThreadObj->CreateRecorderThread();
	// create TraKStarTraKStar acquisition handles
	//Trakstarhandle = (HANDLE)CreateRecorderThreadex(NULL, 0, TrakstarThread::ThreadStaticRecordPositionDataEntryPoint, TrakStarThreadObj, CREATE_SUSPENDED, &Trakstarid);
	TrakStarThreadObj->CreateRecorderThread();

	// start threads
	TiepieThreadObj->StartRecorderThread();
	TrakStarThreadObj->StartRecorderThread();

	Beep( 750, 300 );

	// wait for the measurement threads to finish
	TiepieThreadObj->WaitUntilRecorderThreadIsDone();
	TrakStarThreadObj->WaitUntilRecorderThreadIsDone();
	

	// retrieve and save data
	// TiepieThreadObj->RetrieveCh1DataFromInstrument();
	TiepieOut = TiepieThreadObj->GetOutput();
	TrakstarOut = TrakStarThreadObj->GetOutput();

	utility<double> helper;
	helper.matlabSaveVnlVector( dataPath + "TiePieTime" + acquisitionTag + ".mat", TiepieOut->GetTimeCh1(), "TiePieTime" + acquisitionTag );
	helper.matlabSaveVnlVector( dataPath + "TiePieVoltage" + acquisitionTag  +  ".mat", TiepieOut->GetVoltageCh1(), "TiePieVoltage" + acquisitionTag );
//	helper.matlabSaveVnlMatrix( dataPath + "TrakStarData" + acquisitionTag + ".mat", TrakstarOut->Getmeasures(), "TrakStarData" + acquisitionTag );

	Beep( 750, 300 );
	Beep( 750, 300 );


	return 0;
}

