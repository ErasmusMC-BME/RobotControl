// NXT++ test.cpp : Defines the entry point for the console application.
//
//LEGO 
//#define USEOPENCVTHREAD
#define USELEGOTHREAD
//#define USETIEPIETHREAD
//#define USETRAKSTARTHREAD
//#define USERECONSTRUCTIONTHREAD
//TiePie
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
#include "NIGReconstructionThread.h"

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
template <class Tin,class Tout>  ReconstructionThread<Tin,Tout> *  ReconstructionThread<Tin,Tout>::m_Reconstructionthread=0;

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
#if 0
	Comm::NXTComm comm;
  bool _bLegoFound;
	bool bValue =true;
	if(NXT::Open(&comm)) //initialize the NXT and continue if it succeeds
	{
		
		std::string name="receiverFromPC1";
		_bLegoFound=true;
		NXT::StartProgram(&comm,name);
		std::cout << "NXT Connection made with USB " << std::endl;
	
	}
	else
	{
		if(NXT::OpenBT(&comm)) //initialize the NXT and continue if it succeeds
		{
			std::string name="receiverFromPC1";
			_bLegoFound=true;
			NXT::StartProgram(&comm,name);
			std::cout << "NXT Connection made with BT" << std::endl;
		
		}

		_bLegoFound = false;
		std::cout << "NXT connection failed" << std::endl;

	
	}



	if(_bLegoFound)
	{

	//	 NXT::Motor::GetRotationCount(&comm, 0);
		int inbox=MAILBOX_RESET;
		int Value=4;
		//ViInt8 responseBuffer[] = { 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 };
		ViUInt8 directCommandBuffer[] = { 0x13,17, 7, true ,NULL};
	//	ViUInt8 directCommandBuffer[] = {0x09, 5,2,1};
		ViUInt8 responseBuffer[] = {2,2,2,2,2,2,2};
		ViUInt8 responseBuffera[] = {2,2,2,2,2,2,2};
		ViUInt8 responseBuffer1[] = {2,2,2,2,2};


		comm.SendDirectCommand( true, reinterpret_cast< ViByte* >( directCommandBuffer ), sizeof( directCommandBuffer ),
			reinterpret_cast< ViByte* >( responseBuffer ), sizeof( responseBuffer ));
		comm.Close();
		comm.Open();
		comm.SendDirectCommand( true, reinterpret_cast< ViByte* >( directCommandBuffer ), sizeof( directCommandBuffer ),
				reinterpret_cast< ViByte* >( responseBuffer ), sizeof( responseBuffer ));
		comm.Close();
		comm.Open();



	for(int j = 0; j <  sizeof( responseBuffer ); j++)
		std::cout << (int)responseBuffer[j] << "\n";
	//std::cout << (char *)responseBuffer[4] << "\n";
	ViUInt8 directCommandBuffer1[] = { 0x13,18, 8, false };
	comm.SendDirectCommand( true, reinterpret_cast< ViByte* >( directCommandBuffer1 ), sizeof( directCommandBuffer1 ),
		reinterpret_cast< ViByte* >( responseBuffer1 ), sizeof( responseBuffer1 ));


	
	//  responseBuffer[j] =1;

			comm.Close();
			return 1;
	/*	
	Byte O: OxOO or OxBO 
	Byte 1: Ox13 
	Byte 2: Remote lnbox number  (O - 9) 
	Byte 3: Local lnbox number (O - 9) 
	Byte 4: Remove? (Boolean; TRUE (non-zero) value clears message from Remote lnbox)  

	Return package:  
	Byte O: OxO2        
	Byte 1: Ox13 
	Byte 2: Status Byte 
	Byte 3: Local lnbox number  (O - 9)
	Byte 4: Message size 
	Byte 5 - 63: Message data (padded) 
	*/


		ViUInt8* directBoolCommandBuffer = NULL;
		directBoolCommandBuffer  = new ViUInt8[5];
		directBoolCommandBuffer[0] = 0x09;
		directBoolCommandBuffer[1] = inbox-1; // Mailbox number -1 (e.g. for mailbox 1 use Ox00 
		directBoolCommandBuffer[2] = 2; // this is the datalength: 1 byte + a 1 byte terminator 
		directBoolCommandBuffer[3] = bValue;
		directBoolCommandBuffer[4] = NULL;// terminator
	
	//	comm.SendDirectCommand( false, reinterpret_cast< ViByte* >( directBoolCommandBuffer ),5,NULL,0);
	
		comm.SendDirectCommand( true, reinterpret_cast< ViByte* >( directBoolCommandBuffer ), 5,reinterpret_cast< ViByte* >( responseBuffer1 ), sizeof( responseBuffer1 ));
	//	for(int j = 0; j <  sizeof( responseBuffer ); j++)
		//	std::cout << (int)responseBuffer[j] << "\n";

		ViUInt8 directCommandBufferGetStatus[] = { 0x0E, 0};
		ViUInt8 directCommandBufferLsRead[] = { 0x10, 0};
		ViUInt8 directCommandBufferLsWrite[] = { 0x0F, 0,0x02, 0x01, 0x02, 0x42 };


		// Send the direct command to the NXT.
		comm.SendDirectCommand( true, reinterpret_cast< ViByte* >( directCommandBufferGetStatus ), sizeof( directCommandBufferGetStatus ),
			reinterpret_cast< ViByte* >( responseBuffer ), sizeof( responseBuffer ));
		comm.SendDirectCommand( true, reinterpret_cast< ViByte* >( directCommandBufferLsWrite ), sizeof( directCommandBufferLsWrite ),
			reinterpret_cast< ViByte* >( responseBuffer ), sizeof( responseBuffer ));
		comm.SendDirectCommand( true, reinterpret_cast< ViByte* >( directCommandBufferLsRead ), sizeof( directCommandBufferLsRead ),
			reinterpret_cast< ViByte* >( responseBuffer ), sizeof( responseBuffer ));
		for(int j = 0; j <  sizeof( responseBuffer ); j++)
			std::cout << (int)responseBuffer[j] << "\n";


		comm.Close();

			Value=4;

		}
#endif
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



	double Anglefactor=450/90;
	double Zfactor=26026/15;
	double Imagefactor=804/190;
	if(LegoThreadObj->isLegoFound())
	{
		//do // clear buffer
		//{
		//} while (LegoThreadObj->BoolRecieve(9,true));

  bool bout=false;
	//	LegoThreadObj->BoolRecieve(9,true);
		double zz,aa,bb;
		zz=-0.31*Zfactor;  // corresponds to 1 CM (-7.5.. +7.5)
		aa=-10*Anglefactor; // corresponds to -45 deg (-90.. +90)
		bb=-10*Imagefactor; // corresponds to -45 deg  (-90.. +90)
		LegoThreadObj->WordSend(zz,MAILBOX_A);//Z Plane + <---> -
		LegoThreadObj->WordSend(aa,MAILBOX_B);//Angle 0 <---> -
		LegoThreadObj->WordSend(bb,MAILBOX_C);//Image Plane  + <---> -
		LegoThreadObj->BoolSend(bValue,MAILBOX_START);//Start Motors
		do 
		{
		} while (!LegoThreadObj->BoolRecieve(9,true));
		bout=LegoThreadObj->BoolRecieve(9,true);
		int bytesRead =LegoThreadObj->GetRotationCount( 0);
		bytesRead =LegoThreadObj->LSGetStatus( 0);
		LegoThreadObj->WordSend(0,MAILBOX_A);//Z Plane + <---> -
		LegoThreadObj->WordSend(0,MAILBOX_B);//Angle 0 <---> -
		LegoThreadObj->WordSend(0,MAILBOX_C);//Image Plane  + <---> -
		LegoThreadObj->BoolSend(bValue,MAILBOX_START);// Move motors to 0 Position
		bout=LegoThreadObj->BoolRecieve(9,true);
		bytesRead =LegoThreadObj->LSGetStatus( 0);
		LegoThreadObj->WaitForNxtDone(false);
		do 
		{
		} while (!LegoThreadObj->BoolRecieve(9,true));

	 bytesRead =LegoThreadObj->GetRotationCount( 0);
	 LegoThreadObj->Close(  );

	return 0;
		
		//LegoThreadObj->WaitForRotationIdle();





		//LegoThreadObj->StartRecorderThread();
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
#if 0

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

