// NXT++ test.cpp : Defines the entry point for the console application.
//

#ifndef OpenCVThread_H
#define OpenCVThread_H
#include <iostream>
#include <fstream>
#include <iostream>
#include <iomanip>

#include <sys/stat.h>
#include <windows.h>
#include <time.h>
#include <snmp.h>
#include <conio.h>
#include <stdio.h>
#include "winsock.h"
#include <math.h>

#include <vector>
#include <string>
#include <map>
#include <list>
#include <vector>
#include <process.h>    /* CreateRecorderThread, _endthread */

#include "Timer.h"
#include "thread.h"
#include "NIGMultiThread.h"
#include "itkImageDuplicator.h"
#include "vnl/vnl_vector.h"
#include <cv.h>
#include "highgui.h" 
class SharedObjects;

/*!
 * \brief OpenCVObjects  Class for the OpenCV Video thread 
 * this class contains a vnl_vector list to store the time info per frame,
 * a vnl_vector list to store the Frame Data
 * and the actual Frame number
 * the data is stored as a ring buffer
 * Remarks
 *
 */
class OpenCVObjects
{
private:
	/*!
	 * \brief  List to store the time per frame
	 *
	 * Remarks
	 *
	 */
	vnl_vector<double> *_timeVideo;
	/*!
	 * \brief  List to store the frame Imagedata can be used as a ringbuffer 0.. (m_numSamples-1)
	 *
	 * Remarks
	 *
	 */
	int *_ActualFrameNr;
	/*!
	 * \brief the actual frame number 0.. (m_numSamples-1)
	 *
	 * Remarks
	 *
	 */
	std::vector<DuplicatorType::Pointer> *_VideoImage;

public:

	/*!
	 * \brief  Returns the List to store the time per frame
	 *
	 * Remarks
	 *
	 */

	vnl_vector<double>  * GetTimeVideo()
	{
		return _timeVideo;
	}
	/*!
	 * \brief  sets the List to store the time per frame
	 *
	 * Remarks
	 *
	 */


	void SetTimeVideo(vnl_vector<double> *timeVideo )
	{
		_timeVideo=timeVideo;
	}

	/*!
	 * \brief returns the actual frame number 0.. (m_numSamples-1)
	 *
	 * Remarks
	 *
	 */

	int  * GetActualFrameNr()
	{
		return _ActualFrameNr;
	}
	/*!
	 * \brief sets the actual frame number 0.. (m_numSamples-1)
	 *
	 * Remarks
	 *
	 */

	void SetActualFrameNr(int *ActualFrameNr )
	{
		_ActualFrameNr=ActualFrameNr;
	}

	/*!
	 * \brief  Returns the List to store the frame Imagedata, can be used as a ringbuffer 0.. (m_numSamples-1)
	 *
	 * Remarks
	 *
	 */

	std::vector<DuplicatorType::Pointer> * GetVideoImage()
	{
		return _VideoImage;
	}

	/*!
	 * \brief  Sets the List to store the frame Imagedata, can be used as a ringbuffer 0.. (m_numSamples-1)
	 *
	 * Remarks
	 *
	 */
	void SetVideoImage(std::vector<DuplicatorType::Pointer> *videoImage )
	{
		_VideoImage=videoImage;
	}

	OpenCVObjects()
	{
	}

	virtual ~OpenCVObjects()
	{
		// Release resources used by the critical section object.
	}

};



ImageType::Pointer itkImageFromBuffer( IplImage* opencvIO, void* buffer, size_t bufferSize );


template  <class Tin,class Tout>  class OpenCVThread:public thread<Tin,Tout>
{
private:
	static OpenCVThread *m_OpenCVthread;
	bool _bOpenCVFound;
	bool _bOpenCVReady;
	int _numSamples;
	int _ActualFrameNr;
	int _VideoCommand;
	vnl_vector<double>  _timeVideo;
	std::vector<DuplicatorType::Pointer> _VideoImage;
public:
	OpenCVThread();
	~OpenCVThread(void);
	static OpenCVThread<Tin,Tout> * New(thread<Tin,Tout> * _thread=NULL);
	virtual void Initialize(const char *fmt=NULL, ...);
	virtual void ThreadEntryPoint();
	bool isOpenCVFound() {	return _bOpenCVFound;}

private:
};


#include "NIGOpenCVThread.txx"
#endif