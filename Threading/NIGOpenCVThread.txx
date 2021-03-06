// NXT++ test.cpp : Defines the entry point for the console application.
//


ImageType::Pointer itkImageFromBuffer( IplImage* opencvIO, void* buffer, size_t bufferSize )
{
	// Set up for incoming image
	ImageType::RegionType region;
	ImageType::SizeType   size;
	ImageType::IndexType  start;
	//IPL_DEPTH_32F
	int  nChannels= opencvIO->nChannels;;      /* Ignored by OpenCV */

	int depth = opencvIO->depth;
	size[0] = opencvIO->width;
	size[1] = opencvIO->height;
	//std::cout << "OpenCVVideoIO::Read (from camera)..." << nChannels <<std::endl;
	start.Fill(0);
	region.SetIndex(start);
	region.SetSize(size);
	ImageType::PointType   origin;
	ImageType::SpacingType space;
	origin.Fill(0.0);
	space.Fill(1.0);  // May need fixing

	// Use itkImportImageFilter to create an ITK image
	ImportFilterType::Pointer importFilter = ImportFilterType::New();
	importFilter->SetRegion(region);
	importFilter->SetOrigin(origin);
	importFilter->SetSpacing(space);
	importFilter->SetImportPointer(reinterpret_cast<PixelType *>(buffer), bufferSize, false);
	importFilter->Update();
	ImageType::Pointer frame = importFilter->GetOutput();

	return frame;
}


template  <class Tin,class Tout>  void  OpenCVThread<Tin,Tout>::Initialize(const char *fmt, ...)
{
	thread<Tin,Tout>::Initialize( );

	m_OutputData = new Tout;
	vnl_vector<double> * m_CurrentMeasures=m_InputData->GetCurrentMeasures();

	(*m_CurrentMeasures)[7]=_ActualFrameNr;
	_bOpenCVFound=false;
	unsigned int recordLength_Video=_numSamples;	

	_timeVideo.set_size( recordLength_Video );
	m_OutputData->OpenCVObjects::SetTimeVideo(&_timeVideo);
	m_OutputData->OpenCVObjects::SetActualFrameNr(&_ActualFrameNr);

	DuplicatorType::Pointer	dummyPlanetype;
	_VideoImage.assign(recordLength_Video,dummyPlanetype); 
	for (int i=0;i<recordLength_Video;i++)
	{
		_timeVideo[i]  = 0.0;   
		_VideoImage[i] = DuplicatorType::New();   
	}
	m_OutputData->OpenCVObjects::SetVideoImage(&_VideoImage);
	CvCapture* capture ;
	capture = cvCaptureFromCAM( 0 );
	m_OutputData->OpenCVObjects::SetCapture(capture);
	//	if( capture )
	//{

	//		for(;;)
	//	{
	//		//std:: cout << "camera awake" << std::endl;
	//		IplImage* iplImg = cvQueryFrame( capture );
	//		cv::Mat  frame = iplImg;
	//		if( !frame.empty())
	//		{
	//		break;
	//		}
	//	}
	//}


	if(fmt)
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
	}
};


// Function definitions
template  <class Tin,class Tout>  OpenCVThread<Tin,Tout>::OpenCVThread( void )
{
	_bOpenCVFound = false;
	_bOpenCVReady = false;
	_numSamples = 100;
	_ActualFrameNr=0;

}
template  <class Tin,class Tout>  OpenCVThread<Tin,Tout>::~OpenCVThread( void )
{

}


template <class Tin,class Tout>  OpenCVThread<Tin,Tout> *  OpenCVThread<Tin,Tout>::New( thread<Tin,Tout> * _thread=NULL )  // Create the object
{
	if(m_OpenCVthread==NULL)
	{
		m_OpenCVthread= new OpenCVThread;
	}
	return (OpenCVThread<Tin,Tout> * ) m_OpenCVthread;
}



template  <class Tin,class Tout>  void OpenCVThread<Tin,Tout>::ThreadEntryPoint( void )
{
unsigned int recordLength_Video=_numSamples;	


	CvCapture* capture = 0;

	cv::Mat frame, frameCopy, image;
	IplImage* frame1;

//	capture = cvCaptureFromCAM( 0 ); //0=default, -1=any camera, 1..99=your camera

	capture=m_OutputData->OpenCVObjects::GetCapture();

	if(!capture)std:: cout << "No camera detected" << std::endl;
	//cvNamedWindow("result",CV_WINDOW_AUTOSIZE);
	std:: cout << "camera init" << std::endl;
	//HWND hwnd = (HWND)cvGetWindowHandle("result");
	
	if( capture )
	{


		Timer * m_timer= (Timer *) m_SyncObject;
		IplImage* iplImg = cvQueryFrame( capture );
		//
		std:: cout << "camera detected" << std::endl;


		for(;;)
		{
			//std:: cout << "camera awake" << std::endl;
			IplImage* iplImg = cvQueryFrame( capture );
			frame = iplImg;
			if( !frame.empty())
			{
			
				itk::SizeValueType  camBufferSize = iplImg->imageSize/iplImg->nChannels;
				PixelType  * camBuffer = (PixelType  * ) iplImg->imageData ;

				ImageType::Pointer cameraFrame = itkImageFromBuffer( iplImg,camBuffer, camBufferSize);

				_VideoImage[_ActualFrameNr]->SetInputImage(cameraFrame);    
				_VideoImage[_ActualFrameNr]->Update();  
	 
					//ImageConverterITKToVTK::Pointer ImageConverterITKToVTKPtr=m_InputData->GetImageConverterITKToVTKPtr();
				//ImageConverterITKToVTKPtr->SetInput(_VideoImage[_ActualFrameNr]->GetOutput());
				//ImageConverterITKToVTKPtr->Update();
				vnl_vector<double> * m_CurrentMeasures=m_InputData->GetCurrentMeasures();
				(*m_CurrentMeasures)[7]=_ActualFrameNr;

					_timeVideo[_ActualFrameNr] = m_timer->getElapsedTimeInSec();
				_bOpenCVFound=true;
				_bOpenCVReady = true;
				cvShowImage( "result", iplImg );
				if( cv::waitKey( 10 ) >= 0 )
					cvReleaseCapture( &capture );

				_ActualFrameNr++;
				_ActualFrameNr=_ActualFrameNr%recordLength_Video;

			}

			else
			{
				_bOpenCVFound = false;
				_bOpenCVReady = false;
				//std::cerr << "OpenCV Capture failed." << std::endl;
			}
		}

		cv::waitKey(0);

		cvDestroyWindow("result");

		return ;
	}



#if 0


	CvCapture* capture = cvCaptureFromCAM( CV_CAP_ANY );
	if(capture)
	{
		Timer * m_timer= (Timer *) m_SyncObject;

		// Create a window in which the captured images will be presented
		cvNamedWindow( "mywindow", CV_WINDOW_AUTOSIZE );
		// Show the image captured from the camera in the window and repeat
		IplImage* frame = cvQueryFrame( capture );
		if(frame)
		{
			itk::SizeValueType  camBufferSize = frame->imageSize;
			PixelType  * camBuffer = (PixelType  * ) frame->imageData ;
			std::cout << "OpenCVVideoIO::Read (from camera)..." << camBufferSize <<std::endl;
			//int Index=0;

			while ( 1 ) {
				// Get one frame
				IplImage* frame = cvQueryFrame( capture );
#if 0
				if(frame)
				{


					PixelType  * camBuffer = (PixelType  * ) frame->imageData ;
					ImageType::Pointer cameraFrame = itkImageFromBuffer( frame,camBuffer, camBufferSize);

					_VideoImage[_ActualFrameNr]->SetInputImage(cameraFrame);    
					_VideoImage[_ActualFrameNr]->Update();  


					//ImageConverterITKToVTK::Pointer ImageConverterITKToVTKPtr=m_InputData->GetImageConverterITKToVTKPtr();
					//ImageConverterITKToVTKPtr->SetInput(_VideoImage[_ActualFrameNr]->GetOutput());
					//ImageConverterITKToVTKPtr->Update();
					vnl_vector<double> * m_CurrentMeasures=m_InputData->GetCurrentMeasures();
					(*m_CurrentMeasures)[7]=_ActualFrameNr;

					//ImageType::Pointer clonedImage = PlaneImage[Index]->GetModifiableOutput();


					_timeVideo[_ActualFrameNr] = m_timer->getElapsedTimeInSec();
					_bOpenCVFound=true;
				}
				//std::cout << "_timeVideo" << Index << _timeVideo[Index]  << std::endl;

				if ( !frame ) {
					fprintf( stderr, "ERROR: frame is null...\n" );
					getchar();
					break;
				}
				//std::vector<DuplicatorType::Pointer> *m_VideoImage;
				//m_VideoImage=m_OutputData->GetVideoImage();
			/*	WriterType::Pointer writer = WriterType::New();
				writer->SetFileName("d:\\cam24.tiff");
				ImageType::Pointer clonedImage = (*m_VideoImage)[0]->GetOutput();
				writer->SetInput(clonedImage);
				writer->Update();*/

				//frame->imageData; 
#endif
				cvShowImage( "mywindow", frame );
				// Do not release the frame!
				//If ESC key pressed, Key=0x10001B under OpenCV 0.9.7(linux version),
				//remove higher bits using AND operator
				if ( (cvWaitKey(10) & 255) == 27 ) break;
				_ActualFrameNr++;
				_ActualFrameNr=_ActualFrameNr%recordLength_Video;
			}
		}
		else
		{
			_bOpenCVFound = false;
			_bOpenCVReady = false;
			 std::cerr << "OpenCV Capture failed." << std::endl;
		}
	}
#endif

	// Release the capture device housekeeping
	cvReleaseCapture( &capture );
	cvDestroyWindow( "mywindow" );
	_endthread();
	return ;
}


