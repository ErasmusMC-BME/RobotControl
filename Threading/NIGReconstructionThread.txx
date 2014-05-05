// NXT++ test.cpp : Defines the entry point for the console application.
//



// Base Function definitions
template  <class Tin,class Tout>  ReconstructionThread<Tin,Tout>::ReconstructionThread( void )
{
	_bReconstructionReady = false;
	_bReconstructionFound = false;
}

template  <class Tin,class Tout>  ReconstructionThread<Tin,Tout>::~ReconstructionThread( void )
{
}

template <class Tin,class Tout>  ReconstructionThread<Tin,Tout> *  ReconstructionThread<Tin,Tout>::New( thread<Tin,Tout> * _thread=NULL )  // Create the object
{
	if(m_Reconstructionthread==NULL)
	{
		m_Reconstructionthread= new ReconstructionThread;
	}
	return (ReconstructionThread<Tin,Tout> * ) m_Reconstructionthread;
}

template  <class Tin,class Tout>  void  ReconstructionThread<Tin,Tout>::Initialize(const char *fmt, ...)
{
};

template  <class Tin,class Tout>  void ReconstructionThread<Tin,Tout>::ThreadEntryPoint( void )
{

	// This is the desired entry-point-function but to get
	// here we have to use a 2 step procedure involving
	// the ThreadStaticEntryPoint() function.
	// should be a defined var
	
	_endthread();
}

