// NXT++ test.cpp : Defines the entry point for the console application.
//
//LEGO
//#include "NXT++.h"
#ifndef NIGMULTITHREAD_H
#define NIGMULTITHREAD_H

#include "Timer.h"
#include <vnl/vnl_vector.h>

#include "itkImportImageFilter.h"
#include "itkImageFileWriter.h"
#include "itkImageDuplicator.h"

typedef itk::RGBPixel<char>         PixelType;

//typedef char         PixelType;

typedef itk::ImportImageFilter<PixelType, 2> ImportFilterType;
typedef itk::Image<PixelType, 2>             ImageType;
typedef itk::ImageFileWriter<ImageType>      WriterType;
typedef itk::SizeValueType                   FrameOffsetType;
typedef itk::ImageDuplicator< ImageType >	 DuplicatorType;     

class SharedObjects
{
private:
	vnl_vector<double> _voltageCh1;
	vnl_vector<double> _timeCh1;

public:
	
	vnl_vector<double>  GetTimeCh1()
	{
		return _timeCh1;
	}
	vnl_vector<double>  GetVoltageCh1()
	{
		return _voltageCh1;
	}
	void  SetTimeCh1(vnl_vector<double> timeCh1)
	{
		_timeCh1=timeCh1;
	}
	void SetVoltageCh1(vnl_vector<double> voltageCh1 )
	{
		_voltageCh1=voltageCh1;
	}


	SharedObjects()
	{
	}

	virtual ~SharedObjects()
	{
		// Release resources used by the critical section object.
	}

};
#endif


