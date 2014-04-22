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
#include <process.h>    /* _beginthread, _endthread */


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

#if 0
int maintest() 
{

	SharedObjects * sharedobjs = new SharedObjects();
    // create Lego object
    LegoThread *LegoThreadObj = new LegoThread(sharedobjs);
	// initialize Tiepie object
	Timer t;
	t.start();

	LegoThreadObj->SetTimer(&t);
	LegoThreadObj->init();
	bool bValue=true;
//	LegoThreadObj->BoolSend(bValue,MAILBOX_INIT);//init PID;
	LegoThreadObj->BoolSend(bValue,MAILBOX_RESET);//init PID;
	LegoThreadObj->WordSend(0,MAILBOX_A);//Z Plane + <---> -
	LegoThreadObj->WordSend(0,MAILBOX_B);//Angle 0 <---> -
	LegoThreadObj->WordSend(0,MAILBOX_C);//Image Plane  + <---> -
	LegoThreadObj->BoolSend(bValue,MAILBOX_START);//Start Motor
	double Anglefactor=450/90;
	double Zfactor=26026/15;
	double Imagefactor=804/190;
	while(1)
	{	

		float z = 0;
		float a = 0;
		float b;
		std::cout << "Give the hight you want the probe to go to" << std::endl;
		std::cout << "minimum: -7.5 cm, maximum: 7.5 cm: " << std::endl;
		std::cin>> z;
		if (z < -7.5 || z > 7.5)
		{
			std::cout << "hight is not in range" << std::endl;
			break;
		}
		std::cout << "Give the angle you want the probe to go to" << std::endl;
		std::cout << "minimum: -90 degrees, maximum: 90 degrees: " << std::endl;
		std::cin>> a;
		if (a < -90 || a > 90)
		{
			std::cout << "angle is not in range" << std::endl;
			break;
		}
		std::cout << "Give the angle you want the image plane to go to" << std::endl;
		std::cout << "minimum: 0 degree, maximum: 200 degree: " << std::endl;
		std::cin>> b;
		if (b < -90 || b > 90)
		{
			std::cout << "angle is not in range" << std::endl;
		}
		Wait(500);
		double zz,aa,bb;
		zz=z*Zfactor;
		aa=a*Anglefactor;
		bb=b*Imagefactor;
		LegoThreadObj->WordSend(zz,MAILBOX_A);//Z Plane + <---> -
		LegoThreadObj->WordSend(aa,MAILBOX_B);//Angle 0 <---> -
		LegoThreadObj->WordSend(bb,MAILBOX_C);//Image Plane  + <---> -
		LegoThreadObj->BoolSend(bValue,MAILBOX_START);//Start Motor

	} 

	return 0;
}
#endif
template <class Tin,class Tout>  OpenCVThread<Tin,Tout> *  OpenCVThread<Tin,Tout>::m_OpenCVthread=0;
template <class Tin,class Tout>  TrakstarThread<Tin,Tout> *  TrakstarThread<Tin,Tout>::m_Trakstarthread=0;
template <class Tin,class Tout>  LegoThread<Tin,Tout> *  LegoThread<Tin,Tout>::m_Legothread=0;

template <class Tin,class Tout>  thread<Tin,Tout> *  thread<Tin,Tout>::m_thread=0;

int  mainCuda(int argc, char **argv);
int main()
{
	int argc=1; 
	char argss[4][12]={"probeUp","ProbeDown","ProbeLeft","Proberight"};
	char **argv=(char **)argss;
		//maintest() ;
  //   mainCuda(argc, argv) ;
	// user variables
	double acquisitionTime = 30; // (sec)
	std::string acquisitionTag = "010";
	//std::string dataPath = "D:/Data/ExperimentalData/CalibrationThreeWirePhantom/InitExperiment_19-Jan-2014/";
	std::string dataPath = "D:/Data/";
	int CommandNr=0;
	char cmds[4][12]={"probeUp","ProbeDown","ProbeLeft","Proberight"};//debug

	vnl_vector<double> *_timeVideo;
	std::vector<DuplicatorType::Pointer> *_VideoImage;



// create sharedobjs object
	SharedObjects * sharedobjs = new SharedObjects(); 

	// create sync  object	and start timer;
	SyncTimerObject * syncTimer = new SyncTimerObject();
	syncTimer->start();	
	
	// create threads 	
	

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
	TiepieThreadObj->Update();
	TiepieThreadObj->TestCalibrationDate();
	
	TiepieThreadObj->_BeginThread();
	TiepieThreadObj->_ResumeThread();
	TiepieThreadObj->_WaitForSingleObject();
	TiepieThreadObj->GetOutput();



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
	TrakStarThreadObj->Update();



	OpenCVObjects * VideoOut;
	OpenCVThread<SharedObjects,OpenCVObjects> *OpenCVThreadObj;
	OpenCVThreadObj=OpenCVThread<SharedObjects,OpenCVObjects>::New(); //Create thread
	OpenCVThreadObj->SetInput(sharedobjs);
	OpenCVThreadObj->Initialize();
	OpenCVThreadObj->SetSync(syncTimer);
	OpenCVThreadObj->Update();
	OpenCVThreadObj->_BeginThread();
	OpenCVThreadObj->_ResumeThread();

	do 
	{
		if( OpenCVThreadObj->isOpenCVFound())
		{
			VideoOut=OpenCVThreadObj->GetOutput();
			_timeVideo=VideoOut->GetTimeVideo();
			_VideoImage=VideoOut->GetVideoImage();
		}
	} while (1);
	OpenCVThreadObj->_WaitForSingleObject();



	getchar();	

	//utility<double> helper;
	//helper.matlabSaveVnlVector( dataPath + "TiePieTime" + acquisitionTag + ".mat", TiepieThreadObj->GetTimeCh1(), "TiePieTime" + acquisitionTag );
	//helper.matlabSaveVnlVector( dataPath + "TiePieVoltage" + acquisitionTag  +  ".mat", TiepieThreadObj->GetVoltageCh1(), "TiePieVoltage" + acquisitionTag );
	//TrakstarObjects * Out;

	// start threads
	TiepieThreadObj->_ResumeThread();
	TrakStarThreadObj->_ResumeThread();

	// wait for the measurement threads to finish
	TiepieThreadObj->_WaitForSingleObject();
	TrakStarThreadObj->_WaitForSingleObject();
	

	// retrieve and save data
	TiepieOut = TiepieThreadObj->GetOutput();
	TrakstarOut = TrakStarThreadObj->GetOutput();


	// create Lego object
	LegoThread<SharedObjects,LegoObjects> * self= new  LegoThread<SharedObjects,LegoObjects>();
	LegoObjects * LegoOut;
	LegoThread<SharedObjects,LegoObjects> *LegoThreadObj;
	LegoThreadObj=LegoThread<SharedObjects,LegoObjects>::New(); //Create thread

	LegoThreadObj->SetInput(sharedobjs);
	LegoThreadObj->Initialize();
	LegoThreadObj->SetSync(syncTimer);
	LegoThreadObj->Update();
	LegoThreadObj->_BeginThread();
	LegoThreadObj->_ResumeThread();
	LegoThreadObj->_WaitForSingleObject();
	LegoOut = LegoThreadObj->GetOutput();


#if 0
	
	// initialize Video
	//VideoThreadObj->SetTimer(&t);
	//VideoThreadObj->SetNumSamples(150);
	//VideoThreadObj->Initialize();


	// initialize Lego
	LegoThreadObj->SetTimer(&t);
	LegoThreadObj->Initialize();
	bool bValue=true;
	//LegoThreadObj->BoolSend(bValue,MAILBOX_INIT);//init PID;
	LegoThreadObj->BoolSend(bValue,MAILBOX_RESET);//init PID;
	LegoThreadObj->WordSend(0,MAILBOX_A);//Z Plane + <---> -
	LegoThreadObj->WordSend(0,MAILBOX_B);//Angle 0 <---> -
	LegoThreadObj->WordSend(0,MAILBOX_C);//Image Plane  + <---> -
	LegoThreadObj->BoolSend(bValue,MAILBOX_START);//Start Motor


	Wait(4000);
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

	Wait(4000);
	zz=1*Zfactor;
	aa=45*Anglefactor;
	bb=45*Imagefactor;
	LegoThreadObj->WordSend(zz,MAILBOX_A);//Z Plane + <---> -
	LegoThreadObj->WordSend(aa,MAILBOX_B);//Angle 0 <---> -
	LegoThreadObj->WordSend(bb,MAILBOX_C);//Image Plane  + <---> -
	LegoThreadObj->BoolSend(bValue,MAILBOX_START);//Start Motor

	Wait(4000);

	zz=2*Zfactor;
	aa=90*Anglefactor;
	bb=90*Imagefactor;
	LegoThreadObj->WordSend(zz,MAILBOX_A);//Z Plane + <---> -
	LegoThreadObj->WordSend(aa,MAILBOX_B);//Angle 0 <---> -
	LegoThreadObj->WordSend(bb,MAILBOX_C);//Image Plane  + <---> -
	LegoThreadObj->BoolSend(bValue,MAILBOX_START);//Start Motor




	
	// Start video thread 
	//Videohandle = (HANDLE)_beginthreadex(NULL, 0, VideoThread::ThreadStaticVideoCommandEntryPoint, VideoThreadObj, CREATE_SUSPENDED, &Videoid);
	//ResumeThread( Videohandle );








	// initialize Tiepie object
	TiepieThreadObj->SetTimer(&t);
	TiepieThreadObj->Initialize();
	double df_Tiepie = 1000.0; // (Hz)
	double sensCh1_Tiepie = 1.0;
	dword recordLength_Tiepie = (dword) ceil( df_Tiepie * acquisitionTime );

	TiepieThreadObj->PrepareCh1ForAcq( recordLength_Tiepie, &sensCh1_Tiepie, &df_Tiepie );

	// initialize TraKStar object
	double df_TrakStar = 80; // (Hz)
	int recordLength_TrakStar = (dword) ceil( df_TrakStar * acquisitionTime );
	TrakStarThreadObj->SetNumSamples(recordLength_TrakStar);
	TrakStarThreadObj->SetsamplingFreq(df_TrakStar);
	TrakStarThreadObj->SetTimer(&t);
	TrakStarThreadObj->Initialize();


	// initialize Lego
	LegoThreadObj->SetTimer(&t);
	LegoThreadObj->Initialize();
	bool bValue=true;
	//	LegoThreadObj->BoolSend(bValue,MAILBOX_INIT);//init PID;
	LegoThreadObj->BoolSend(bValue,MAILBOX_RESET);//init PID;
	LegoThreadObj->WordSend(0,MAILBOX_A);//Z Plane + <---> -
	LegoThreadObj->WordSend(0,MAILBOX_B);//Angle 0 <---> -
	LegoThreadObj->WordSend(0,MAILBOX_C);//Image Plane  + <---> -
	LegoThreadObj->BoolSend(bValue,MAILBOX_START);//Start Motor





	// start threads 

	// Start video thread 
	Videohandle = (HANDLE)_beginthreadex(NULL, 0, VideoThread::ThreadStaticVideoCommandEntryPoint, VideoThreadObj, CREATE_SUSPENDED, &Videoid);
	ResumeThread( Videohandle );


	// create Tiepie acquisition handles
	Tiepiehandle = (HANDLE)_beginthreadex(NULL, 0, TiepieThread::ThreadStaticStartMeasurementCh1EntryPoint, TiepieThreadObj, CREATE_SUSPENDED, &Tiepieid);

	// create TraKStarTraKStar acquisition handles
	Trakstarhandle = (HANDLE)_beginthreadex(NULL, 0, TrakstarThread::ThreadStaticRecordPositionDataEntryPoint, TrakStarThreadObj, CREATE_SUSPENDED, &Trakstarid);

	// start threads
	ResumeThread( Tiepiehandle );
	ResumeThread( Trakstarhandle );

	Beep( 750, 300 );

	// wait for the measurement thre	ads to finish
	WaitForSingleObject(Tiepiehandle, INFINITE);
	WaitForSingleObject(Trakstarhandle, INFINITE);
	sharedobjs->StartTiepieSync();
	sharedobjs->StopTiepieSync();
	getchar();	
	// retrieve and save data
	TiepieThreadObj->RetrieveCh1DataFromInstrument();
	utility<double> helper;
	helper.matlabSaveVnlVector( dataPath + "TiePieTime" + acquisitionTag + ".mat", TiepieThreadObj->GetTimeCh1(), "TiePieTime" + acquisitionTag );
	helper.matlabSaveVnlVector( dataPath + "TiePieVoltage" + acquisitionTag  +  ".mat", TiepieThreadObj->GetVoltageCh1(), "TiePieVoltage" + acquisitionTag );
	//helper.matlabSaveVnlMatrix( dataPath + "TrakStarData" + acquisitionTag + ".mat", TrakStarThreadObj->Getmeasures(), "TrakStarData" + acquisitionTag );

	Beep( 750, 300 );
	Beep( 750, 300 );


	if( VideoThreadObj->isVideoTFound())
	{
		_timeVideo=sharedobjs->GetTimeVideo();
		_VideoImage=sharedobjs->GetVideoImage();

		//std::cout << "_timeVideo  " << CommandNr << (*_timeVideo)[CommandNr]  << std::endl;
		std::vector<DuplicatorType::Pointer> *m_VideoImage;
		m_VideoImage=sharedobjs->GetVideoImage();
		WriterType::Pointer writer = WriterType::New();
		writer->SetFileName("d:\\cam23.tif");
		ImageType::Pointer clonedImage = (*m_VideoImage)[0]->GetOutput();
		writer->SetInput(clonedImage);
		writer->Update();
		utility<double> helper;
		helper.matlabSaveVnlVector( dataPath + "VideoTime" + acquisitionTag + ".mat", *_timeVideo, "VideoTime" + acquisitionTag );

	}
	// create Lego  handles
	//LegoThreadObj->SetCommand(CommandNr);
	//Legohandle = (HANDLE)_beginthreadex(NULL, 0, LegoThread::ThreadStaticLegoCommandEntryPoint, LegoThreadObj, CREATE_SUSPENDED, &Legoid);
	double Anglefactor=450/90;
	double Zfactor=26026/15;
	double Imagefactor=804/190;
	float z = 0.4;
	float a = -10;
	float b=-10;
	double zz,aa,bb;
	zz=z*Zfactor;
	aa=a*Anglefactor;
	bb=b*Imagefactor;
	LegoThreadObj->WordSend(zz,MAILBOX_A);//Z Plane + <---> -
	LegoThreadObj->WordSend(aa,MAILBOX_B);//Angle 0 <---> -
	LegoThreadObj->WordSend(bb,MAILBOX_C);//Image Plane  + <---> -
	LegoThreadObj->BoolSend(bValue,MAILBOX_START);//Start Motor
#endif
	
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
	

#if 0

	do 
	{


		sharedobjs->StartSync();

		// create Tiepie acquisition handles
		Tiepiehandle = (HANDLE)_beginthreadex(NULL, 0, TiepieThread::ThreadStaticStartMeasurementCh1EntryPoint, TiepieThreadObj, CREATE_SUSPENDED, &Tiepieid);

		// create TraKStar acquisition handles
		Trakstarhandle = (HANDLE)_beginthreadex(NULL, 0, TrakstarThread::ThreadStaticRecordPositionDataEntryPoint, TrakStarThreadObj, CREATE_SUSPENDED, &Trakstarid);

		// create Lego  handles
		LegoThreadObj->SetCommand(CommandNr);
		Legohandle = (HANDLE)_beginthreadex(NULL, 0, LegoThread::ThreadStaticLegoCommandEntryPoint, LegoThreadObj, CREATE_SUSPENDED, &Legoid);

		//std::cout << "Start Time ResumeThread Tiepiehandle: " << t.getElapsedTimeInSec() << std::endl;
		// start threads

		ResumeThread( Tiepiehandle );

		// std::cout << "Start Time ResumeThread Trakstarhandle: " << t.getElapsedTimeInSec() << std::endl;
		ResumeThread( Trakstarhandle );
		// std::cout << "Start Time ResumeThread Legohandle: " << t.getElapsedTimeInSec() << std::endl;
		ResumeThread( Legohandle );
		// Beep( 750, 300 );


		// wait for the measurement threads to finish
		//std::cout << "Start Time WaitForSingleObject Tiepiehandle: " << t.getElapsedTimeInSec() << std::endl;
		WaitForSingleObject(Tiepiehandle, INFINITE);
		//std::cout << "Start Time WaitForSingleObject Trakstarhandle: " << t.getElapsedTimeInSec() << std::endl;
		WaitForSingleObject(Trakstarhandle, INFINITE);
		// std::cout << "Start Time WaitForSingleObject Legohandle: " << t.getElapsedTimeInSec() << std::endl;
		WaitForSingleObject(Legohandle, INFINITE);
		//Sleep(2000);  
		TiepieThreadObj->RetrieveCh1DataFromInstrument();
		// WaitForSingleObject(Legohandle, INFINITE);
		// retrieve and save data
		utility<double> helper;
		helper.matlabSaveVnlVector( dataPath + "TiePieTime" + acquisitionTag + cmds[CommandNr] + ".mat", TiepieThreadObj->GetTimeCh1(), "TiePieTime" + acquisitionTag );
		helper.matlabSaveVnlVector( dataPath + "TiePieVoltage" + acquisitionTag + cmds[CommandNr] +  ".mat", TiepieThreadObj->GetVoltageCh1(), "TiePieVoltage" + acquisitionTag );
		helper.matlabSaveVnlMatrix( dataPath + "TrakStarData" + acquisitionTag + cmds[CommandNr] + ".mat", TrakStarThreadObj->Getmeasures(), "TrakStarData" + acquisitionTag );

		// std::cout << "Start Time WaitForSingleObject Legohandle: " << t.getElapsedTimeInSec() << std::endl;
		CommandNr++;
		CommandNr=CommandNr%999;

		if( VideoThreadObj->isVideoTFound())
		{
			_timeVideo=sharedobjs->GetTimeVideo();
			_VideoImage=sharedobjs->GetVideoImage();

			//std::cout << "_timeVideo  " << CommandNr << (*_timeVideo)[CommandNr]  << std::endl;
			if(CommandNr==30)
			{
				std::vector<DuplicatorType::Pointer> *m_VideoImage;
				m_VideoImage=sharedobjs->GetVideoImage();
				WriterType::Pointer writer = WriterType::New();
				writer->SetFileName("d:\\cam23.tiff");
				ImageType::Pointer clonedImage = (*m_VideoImage)[0]->GetOutput();
				writer->SetInput(clonedImage);
				writer->Update();
				utility<double> helper;
				helper.matlabSaveVnlVector( dataPath + "VideoTime" + acquisitionTag + ".mat", *_timeVideo, "VideoTime" + acquisitionTag );
			}

		}
	} while (1);
	#endif
getchar();	


	Beep( 750, 300 );
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
	TiepieThreadObj->Update();
	
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
	TrakStarThreadObj->Update();



	// create Tiepie acquisition handles
	//Tiepiehandle = (HANDLE)_beginthreadex(NULL, 0, TiepieThread::ThreadStaticStartMeasurementCh1EntryPoint, TiepieThreadObj, CREATE_SUSPENDED, &Tiepieid);
	TiepieThreadObj->_BeginThread();
	// create TraKStarTraKStar acquisition handles
	//Trakstarhandle = (HANDLE)_beginthreadex(NULL, 0, TrakstarThread::ThreadStaticRecordPositionDataEntryPoint, TrakStarThreadObj, CREATE_SUSPENDED, &Trakstarid);
	TrakStarThreadObj->_BeginThread();

	// start threads
	TiepieThreadObj->_ResumeThread();
	TrakStarThreadObj->_ResumeThread();

	Beep( 750, 300 );

	// wait for the measurement threads to finish
	TiepieThreadObj->_WaitForSingleObject();
	TrakStarThreadObj->_WaitForSingleObject();
	

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

