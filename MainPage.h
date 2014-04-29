/* This header file is not included in the project for programming reasons, 
   and thus does not contain any code in C++ language.
   This header file solely exists to have a special place for documentation 
   generated from the source and header files by Doxygen.
*/

/*!
	\mainpage Multithreading Toolbox  - Application Programming Interface (API)
	\section Multithreading_contents Contents

	-	\ref Multithreading_intro  "Introduction"
	-	\ref Multithreading_usage  "Basic Usage"
	-	\ref Multithreading_Input  "Input Class"
	-	\ref Multithreading_Output "Output Class"
	-	\ref Multithreading_Current_restrictions  "Restrictions"
	-	\ref Multithreading_use  "Details of use"
	-	\ref Multithreading_SimpleSequence  "Sequence Diagram of simple interface"
	-	\ref Multithreading_Example  "Example "
	-	\ref Multithreading_Building  "Building Application using Cmake "
	-	\ref Multithreading_links  "Related Documents"
	-	\ref Multithreading_Contacts  "Contacts"
	*/

/*!
	\page Multithreading_intro

	\section Multithreading_intro Multithreading Class information 
	<p align=justify>
	\anchor image1a
	\image html Robot.png "Usage Robot"
	In this NIG Project the overall goal is to develop a software tool which enables the control of the robotic setup in order to control the US probe. 
	The software needs to interact with the motor controller (Lego Mindstorms) , with an electromagnetic position sensing system with two sensors (Ascension Trakstar),
	A Tiepie Box for getting an ECG signal, and an Imaging Capture Device to Grab images from the Philips Ultrasound Machine.
	To control all devices simultaneously a Multithreading base class is developed


	The Multithreading-Class is a Class to use multiple threads. .
	Multithreading-Class can be easily interfaced to any Windows-based program written or C++. The Multithreading-Class was developed by BME in C++ 
	(Microsoft Visual C++ 10.0) .  

	\section Example
	 we need several threads to control the demonstrator 
	 After initialisation and calibration of the system we start the threads
	 
	
	
	

	\anchor image1
	\image html Overview1.png "Usage Example"
 <BR>
	For synchronization we use the SyncTimerObject 
	this object exist of a timer and a number of "CriticalSection" objects 
	with those CriticalSection objects  we can let on thread wait until another thread is ready filling a data block
	Back to the \ref Multithreading_contents "Main Page"
*/



/*!

	\page Multithreading_usage
	\section Multithreading_usage Basic use 

	<p align=justify>
	1.	Create the data objects (Timer,Input/output data ) <BR>
	2.	Create/initialize the threads <BR>
	3.	Start the threads <BR> 
	4.	Wait until the threads are finished <BR> 
	5.	retrieve the data from the output objects <BR>
	7.	possibly feed back position data restart threads<BR>

	<BR>
	Back to the \ref Multithreading_contents "Main Page"

 
*/
/*!
	\page Multithreading_Current_restrictions
	\section Multithreading_Current_restrictions Restrictions
	<p align=justify>

	Current restrictions

	<BR>
	Back to the \ref Multithreading_contents "Main Page"

*/
/*!
	\page Multithreading_Input
	\section Multithreading_Building Used Input Class
	<BR>
	The Base class is Templated so any type of class can be used as input, for this project we give all the threads the same input class <BR>
	This Class is used to Pass all commonly used data to all threads

	the class can be used as follows
	m_InputData->SharedObjects::SetTimeVideo(&_timeVideo);


	Back to the \ref Multithreading_contents "Main Page"
*/
/*!
	\page Multithreading_Output
	\section Multithreading_Building Used Output Class
	<BR>
	The Base class is Templated so any type of class can be used as Output, <BR>
	
	the class can be used as follows
	
	m_OutputData->TiepieObject::SetTimeCh1(_timeCh1);


	Back to the \ref Multithreading_contents "Main Page"
*/

/*!
	\page Multithreading_use
	\section Multithreading_use Details of use 
	<p align=justify>
		
	\code
	Details of use 
	Public functions: 
	1. create the data objects

	2. create/initialize the Multithreading Object


	TemplatedThread<InputObject,OutputObject> *_DummyThread;
	_DummyThread=TemplatedThread<InputObject,OutputObject>::New(); //Create thread


	double df_Tiepie = 1000.0; // (Hz)
	double sensCh1_Tiepie = 1.0;
	dword recordLength_Tiepie = (dword) ceil( df_Tiepie * acquisitionTime );

	_DummyThread->Initialize("Wdd",  recordLength_Tiepie, sensCh1_Tiepie, df_Tiepie  );
	_DummyThread->SetSync(syncTimer);
	_DummyThread->SetInput(sharedobjs);






	3. start the Multythread 

	
	4. retrieve the outputs (contours/markers) 
	
			..... Destruct Objects
  \endcode

	<BR>
	<BR>
	Back to the \ref Multithreading_contents "Main Page"
*/
/*!
	\page Multithreading_SimpleSequence  
	\section Multithreading_SimpleSequence  Time Diagram 
	<p align=justify>
	\anchor image2
	\image html Overview2.png "Time Diagram "
		
	<BR>
	Back to the \ref Multithreading_contents "Main Page"
*/


/*!
	\page Multithreading_Example  
	\section Multithreading_Detection_Example  Example
	<p align=justify>
			
	<BR>
	Back to the \ref Multithreading_contents "Main Page"
*/
	
/*!
	\page Multithreading_Building
	\section Multithreading_Building Building Application using Cmake 

	-	\ref Multithreading_CMakeintro  "Introduction"
	-	\ref Multithreading_CMakeBase  "Basic Packages"
	-	\ref Multithreading_Lego       "Lego Packages"
	-	\ref Multithreading_Ascension  "Ascension Packages"
	-	\ref Multithreading_VXL  "VXL Packages"
	-	\ref Multithreading_Linking  "Linking settings"

	<BR>

	Back to the \ref Multithreading_contents "Main Page"
*/
/*!
	\page Multithreading_links
	\section Multithreading_links Related documents

	a.	Matrix Algebra on GPU and Multicore Architectures <BR>
	http://icl.cs.utk.edu/magma/index.html <BR>
	b.	Efficient Algorithms for K-Means Clustering <BR>
	http://www.cs.umd.edu/~mount/Projects/KMeans <BR>
	c.	ANN: A Library for Approximate Nearest Neighbor Searching <BR>
	http://www.cs.umd.edu/~mount/ANN <BR>
	d.	Boost provides free peer-reviewed portable C++ source libraries <BR>
	http://www.boost.org <BR>
	e.	The Image-Guided Surgery Toolkit <BR>
	http://www.igstk.org <BR>
	f.	VXL (the Vision-something-Libraries) <BR>
	http://vxl.sourceforge.net <BR>
	g.	nxtOSEK/JSP ANSI C/C++ with OSEK/μITRON RTOS for LEGO MINDSTORMS NXT <BR>
	http://lejos-osek.sourceforge.net/ <BR>
	h.	dbworth / nxt-plus-plus <BR>
	https://github.com/dbworth/nxt-plus-plus <BR>
	i.	FREE SOFTWARE FOR ASCENSION’S 3D GUIDANCE TRACKERS <BR>
	http://www.ascension-tech.com/ <BR>
	j.	elastix: a toolbox for rigid and nonrigid registration of images. <BR>
	http://elastix.isi.uu.nl/index.php <BR>
	k.	Practical command line tools based on the ITK <BR>
	https://github.com/ITKTools/ITKTools <BR>
	l.	Open Source Computer Vision Library <BR>
	http://opencv.org/ <BR>
	m.	Open Computing Language <BR>
	http://nl.wikipedia.org/wiki/OpenCL <BR>
	n.	specification for parallel programming <BR>
	http://openmp.org/wp/ <BR>



	<BR>

	Back to the \ref Multithreading_contents "Main Page"
*/

/*!
	\page Multithreading_CMakeintro
	some common cmake settings are:
	\code
	CMAKE_MINIMUM_REQUIRED(VERSION 2.8 FATAL_ERROR) 
	IF(COMMAND CMAKE_POLICY) CMAKE_POLICY(SET CMP0003 NEW) ENDIF(COMMAND CMAKE_POLICY) 

	# Kill the anoying MS VS warning about non-safe functions.They hide real warnings. 
	ADD_DEFINITIONS(/D_SCL_SECURE_NO_DEPRECATE /D_CRT_SECURE_NO_DEPRECATE /D_CRT_TIME_FUNCTIONS_NO_DEPRECATE) 
	\endcode

	Back to the \ref Multithreading_Building "Cmake Page"
*/

/*!
	\page Multithreading_CMakeBase
	The needed standard packages for building this project are:
	\code
	FIND_PACKAGE (ITK REQUIRED) 
	FIND_PACKAGE (VTK REQUIRED)  
	FIND_PACKAGE (OpenCV REQUIRED ) 
	
	INCLUDE( ${USE_ITK_FILE} )  
	INCLUDE( ${USE_VTK_FILE} ) 
	\endcode
	Back to the \ref Multithreading_Building "Cmake Page"
*/

/*!
	\page Multithreading_Lego

	\code
	LINK_DIRECTORIES ($ENV{BME_BASE_DIR}/NXT++.07/bin32/${OutDir}) 
	LINK_DIRECTORIES ($ENV{BME_BASE_DIR}/NXT++.07/Lib) 
	INCLUDE_DIRECTORIES($ENV{BME_BASE_DIR}/NXT++.07/include)
	\endcode
	Back to the \ref Multithreading_Building "Cmake Page"
*/
/*!
	\page Multithreading_Ascension

	\code
	LINK_DIRECTORIES ($ENV{BME_BASE_DIR}/Ascension/Lib) 
	INCLUDE_DIRECTORIES($ENV{BME_BASE_DIR}/Ascension/Include)
	\endcode

	Back to the \ref Multithreading_Building "Cmake Page"
*/

/*!
	\page Multithreading_VXL

	\code
	INCLUDE_DIRECTORIES( $ENV{BME_BASE_DIR}/vxl-1.14.0/Bin32/vcl )
	INCLUDE_DIRECTORIES( $ENV{BME_BASE_DIR}/vxl-1.14.0/Bin32/core )
	INCLUDE_DIRECTORIES( $ENV{BME_BASE_DIR}/vxl-1.14.0/core )
	INCLUDE_DIRECTORIES( $ENV{BME_BASE_DIR}/vxl-1.14.0/vcl )
	LINK_DIRECTORIES( $ENV{BME_BASE_DIR}/vxl-1.14.0/Bin32/Lib/${OutDir} )
	\endcode

	Back to the \ref Multithreading_Building "Cmake Page"
*/

/*!
	\page Multithreading_Linking

	\code
	INCLUDE_DIRECTORIES( $ENV{ALEX_COMMON_DIR} )

	ADD_EXECUTABLE( CombinedApp ${CombinedAppFiles}   ${HW6_HDRS} ${HW6_SRCS} ${HW6_cu})

	TARGET_LINK_LIBRARIES ( CombinedApp ATC3DG NXT++ fantom vnl vcl ${ITK_LIBRARIES} ${VTK_LIBRARIES} ${OpenCV_LIBS} ${OpenCL_LIBS} )
	\endcode

	Back to the \ref Multithreading_Building "Cmake Page"
*/




/*!
	\page Multithreading_Contacts  
	\code
	Gerard v Burken, B.Sc.
	Biomedical Engineering
	Room Ee23.12, Thoraxcenter, Erasmus MC

	Postbus 2040, 3000 CA Rotterdam
	Bezoekadres: Wytemaweg 80, 3015 CN Rotterdam, kamer Ee-2312
	E g.vanburken@erasmusmc.nl | T 010 704 40 38 | F 010 704 47 20

	www.erasmusmc.nl/thoraxcenterbme 
	\endcode
			
	<BR>
	Back to the \ref Multithreading_contents "Main Page"
*/



