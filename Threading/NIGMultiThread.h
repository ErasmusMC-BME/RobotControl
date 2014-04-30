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
	vnl_vector<double> m_voltageCh1;
	vnl_vector<double> m_timeCh1;
	vnl_vector<double> m_CurrentMeasures;
	/*!
  	m_CurrentMeasures Current Time (m_CurrentMeasures[0]) ,  Position (m_CurrentMeasures[1...6]) and ImageFrameNr (m_CurrentMeasures[7])  values
	*/


public:
	
	vnl_vector<double>  GetTimeCh1()
	{
		return m_timeCh1;
	}
	vnl_vector<double>  GetVoltageCh1()
	{
		return m_voltageCh1;
	}
	void  SetTimeCh1(vnl_vector<double> timeCh1)
	{
		m_timeCh1=timeCh1;
	}
	void SetVoltageCh1(vnl_vector<double> voltageCh1 )
	{
		m_voltageCh1=voltageCh1;
	}

	vnl_vector<double>  * GetCurrentMeasures()
	{
		return &m_CurrentMeasures;
	}

	//void SetCurrentMeasures(vnl_vector<double> CurrentMeasures)
	//{
	//	 m_CurrentMeasures=CurrentMeasures;
	//}

	SharedObjects()
	{
			m_CurrentMeasures.set_size( 8 );
	}

	virtual ~SharedObjects()
	{
		// Release resources used by the critical section object.
	}

};
#endif


