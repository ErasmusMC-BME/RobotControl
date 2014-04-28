// NXT++ test.cpp : Defines the entry point for the console application.
//



// Base Function definitions
template  <class Tin,class Tout>  LegoThread<Tin,Tout>::LegoThread( void )
{
	_bLegoReady = false;
	_bLegoFound = false;
}

template  <class Tin,class Tout>  LegoThread<Tin,Tout>::~LegoThread( void )
{
}

template <class Tin,class Tout>  LegoThread<Tin,Tout> *  LegoThread<Tin,Tout>::New( thread<Tin,Tout> * _thread=NULL )  // Create the object
{
	if(m_Legothread==NULL)
	{
		m_Legothread= new LegoThread;
	}
	return (LegoThread<Tin,Tout> * ) m_Legothread;
}

template  <class Tin,class Tout>  void  LegoThread<Tin,Tout>::Initialize(const char *fmt, ...)
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
};

template  <class Tin,class Tout>  void LegoThread<Tin,Tout>::ThreadEntryPoint( void )
{
	ExecuteCommand(); 
	// This is the desired entry-point-function but to get
	// here we have to use a 2 step procedure involving
	// the ThreadStaticEntryPoint() function.

}

template  <class Tin,class Tout>  void LegoThread<Tin,Tout>::Update( void )
{

}


template  <class Tin,class Tout> void LegoThread<Tin,Tout>::Initialize()
{  
	init();
	//calibration();
}

template  <class Tin,class Tout> void LegoThread<Tin,Tout>::Calibrate()
{  
	calibration();
}
template  <class Tin,class Tout> void LegoThread<Tin,Tout>::ExecuteCommand()
{  
	float z = 1;
	float a = 0;
	float b=  0;
	move_relative(z, a, b);
	//Time = m_timer->getElapsedTimeInSec();
	//std::cout << "End Time Lego: " << Time << std::endl;
	//std::cout << "LegoCommand: " << _LegoCommand << std::endl;
	_endthread();
}


