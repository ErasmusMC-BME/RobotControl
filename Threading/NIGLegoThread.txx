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
	bool bValue =true;
	if(NXT::Open(&comm)) //initialize the NXT and continue if it succeeds
	{
		std::string name="receiverFromPC1\n";
		_bLegoFound=true;
		NXT::StartProgram(&comm,name);
		std::cout << "NXT Connection made with USB " << std::endl;
		return;
	}
	else
	{
		if(NXT::OpenBT(&comm)) //initialize the NXT and continue if it succeeds
		{
			std::string name="receiverFromPC1\n";
			_bLegoFound=true;
			NXT::StartProgram(&comm,name);
			std::cout << "NXT Connection made with BT" << std::endl;
			return;
		}
	
		_bLegoFound = false;
		std::cout << "NXT connection failed" << std::endl;
		return ;
	}
};

template  <class Tin,class Tout>  void LegoThread<Tin,Tout>::ThreadEntryPoint( void )
{

	// This is the desired entry-point-function but to get
	// here we have to use a 2 step procedure involving
	// the ThreadStaticEntryPoint() function.
	// should be a defined var
	double Anglefactor=450/90;
	double Zfactor=26026/15;
	double Imagefactor=804/190;
	do
	{
				DOUBLE_POSITION_ANGLES_RECORD record;
				vnl_vector<double> * m_CurrentMeasures=m_InputData->GetCurrentMeasures();
				record.x = (*m_CurrentMeasures)[1] ;
				record.y = (*m_CurrentMeasures)[2] ;
				record.z = (*m_CurrentMeasures)[3] ;
				record.a = (*m_CurrentMeasures)[4] ;
				record.e = (*m_CurrentMeasures)[5] ;
				record.r = (*m_CurrentMeasures)[6] ;

				bool bValue=true;
				double zz,aa,bb;
				zz=record.z*Zfactor;  // corresponds to 1 CM (-7.5.. +7.5)
				aa=record.y*Anglefactor; // corresponds to -45 deg (-90.. +90)
				bb=record.x*Imagefactor; // corresponds to -45 deg  (-90.. +90)
				WordSend(zz,MAILBOX_A);//Z Plane + <---> -
				WordSend(aa,MAILBOX_B);//Angle 0 <---> -
				WordSend(bb,MAILBOX_C);//Image Plane  + <---> -
				BoolSend(bValue,MAILBOX_START);//Start Motors
				WaitForRotationIdle();
			//	TextMessageRecieve(MAILBOX_RECIEVE,false);


 	} while (1);
	_endthread();
}


template  <class Tin,class Tout> void LegoThread<Tin,Tout>::WaitForRotationIdle()
{ 
	// workaround the wait until done test 
	// in the PID the  control order is motor B , Motor C , Motor A
	// should be a message test 
	if(isLegoFound())
	{
		double  tacho;
		double oldtacho=tacho =-10000.0;
		do
		{
			oldtacho=tacho;
			Wait(100);
			tacho=NXT::Motor::GetRotationCount(&comm,1); //controll motor B
		
		}	while (tacho!=oldtacho);
		oldtacho=tacho =-10000.0;
		do
		{
			oldtacho=tacho;
			Wait(100);
			tacho=NXT::Motor::GetRotationCount(&comm,2); //controll motor C
		
		}	while (tacho!=oldtacho);
		oldtacho=tacho =-10000.0;
		do
		{
			oldtacho=tacho;
			Wait(100);
			tacho=NXT::Motor::GetRotationCount(&comm,0); //controll motor A
		
		}	while (tacho!=oldtacho);
		oldtacho=tacho =-10000.0;
	}
}

template  <class Tin,class Tout> void LegoThread<Tin,Tout>::Calibrate()
{  
	bool bValue=true;
	if(isLegoFound())
	{
		BoolSend(bValue,MAILBOX_INIT);//init PID;
		WordSend(0,MAILBOX_A);//Z Plane + <---> -
		WordSend(0,MAILBOX_B);//Angle 0 <---> -
		WordSend(0,MAILBOX_C);//Image Plane  + <---> -
		BoolSend(bValue,MAILBOX_START);//Start Motor
		WaitForRotationIdle();
	}
}




template  <class Tin,class Tout> void LegoThread<Tin,Tout>::TextMessageSend(std::string message, int inbox)
{
	
	//Message  data is treated  as a string; it must include null termination to be accepted.  
	//Accordingly, message  size must include the null termination byte. Message  size must be capped at 
	//59 for all message packets to be legal on USB! 
	int length = message.size();
	const char *charmessage = message.c_str();
	ViUInt8* directTextCommandBuffer = NULL;
	directTextCommandBuffer  = new ViUInt8[3+length+1];
	directTextCommandBuffer[0] = 0x09;
	directTextCommandBuffer[1] = inbox-1;	// Mailbox number -1 (e.g. for mailbox 1 use Ox00 
	directTextCommandBuffer[2] = length+1;	// Byte 3: Message size 
	for(int i = 3; i < 3+length; i++)		// Byte 4 - N: Message data, where N = Message size + 3 
		directTextCommandBuffer[i] = charmessage[i-3];
	directTextCommandBuffer[length+3] = NULL;
	comm.SendDirectCommand( false, reinterpret_cast< ViByte* >( directTextCommandBuffer ),sizeof( directTextCommandBuffer ),NULL,0);

}


#define LOUPPERBYTE(w)           ((BYTE)((((DWORD_PTR)(w)) >> 16) & 0xff))
#define HIUPPERBYTE(w)           ((BYTE)((((DWORD_PTR)(w)) >> 24) & 0xff))

template  <class Tin,class Tout> void LegoThread<Tin,Tout>::WordSend(int Value, int inbox)
{
	ViUInt8* directLongCommandBuffer = NULL;
//	ViUInt8 responseBuffer[64];
	directLongCommandBuffer  = new ViUInt8[8];
	directLongCommandBuffer[0] = 0x09;
	directLongCommandBuffer[1] = inbox-1; // Mailbox number -1 (e.g. for mailbox 1 use Ox00 
	directLongCommandBuffer[2] = 0x05;	// this is the datalength: 1 byte + a 1 byte terminator 
	directLongCommandBuffer[3] = LOBYTE(Value);	// the LSB 
	directLongCommandBuffer[4] = HIBYTE(Value);
	directLongCommandBuffer[5] = LOUPPERBYTE(Value);
	directLongCommandBuffer[6] = HIUPPERBYTE(Value);// the MSB 
	directLongCommandBuffer[7] = NULL;	// terminator
	//comm.SendDirectCommand( true, reinterpret_cast< ViByte* >( directLongCommandBuffer ), 8,reinterpret_cast< ViByte* >( responseBuffer ), sizeof( responseBuffer ));
	comm.SendDirectCommand( false, reinterpret_cast< ViByte* >( directLongCommandBuffer ), 8,NULL,0);
	//for(int j = 0; j <  sizeof( responseBuffer )/8; j++)
	//	std::cout << (int)responseBuffer[j] << "\n";


}
template  <class Tin,class Tout> void LegoThread<Tin,Tout>::BoolSend(bool bValue, int inbox)
{
	ViUInt8* directBoolCommandBuffer = NULL;
	directBoolCommandBuffer  = new ViUInt8[5];
	directBoolCommandBuffer[0] = 0x09;
	directBoolCommandBuffer[1] = inbox-1; // Mailbox number -1 (e.g. for mailbox 1 use Ox00 
	directBoolCommandBuffer[2] = 2; // this is the datalength: 1 byte + a 1 byte terminator 
	directBoolCommandBuffer[3] = bValue;
	directBoolCommandBuffer[4] = NULL;// terminator
	comm.SendDirectCommand( false, reinterpret_cast< ViByte* >( directBoolCommandBuffer ),5,NULL,0);
}

template  <class Tin,class Tout> std::string LegoThread<Tin,Tout>::TextMessageRecieve(int mailbox, bool remove)
{
//BYTE loByte = 17;
//BYTE hiByte = 45;
	ViUInt8 directCommandBuffer[] = { 0x13, mailbox-1, mailbox-1, remove };
	ViUInt8 responseBuffer[64];
	for(int i = 0; i < 64; i++)
		responseBuffer[i] = 0x00;


	// Send the direct command to the NXT.
	comm.SendDirectCommand( true, reinterpret_cast< ViByte* >( directCommandBuffer ), sizeof( directCommandBuffer ),
		reinterpret_cast< ViByte* >( responseBuffer ), sizeof( responseBuffer ));
	for(int j = 0; j <  sizeof( responseBuffer )/8; j++)
		std::cout << (int)responseBuffer[j] << "\n";

	std::string out=(char *)responseBuffer;

//WORD wordVal = MAKEWORD(loByte, hiByte);
	return out;
}

template  <class Tin,class Tout> int LegoThread<Tin,Tout>::WordRecieve(int mailbox, bool remove)
{
	return 0;
>>>>>>> b8ca8d0a1dc11d9ffb9cd1d269878de5b61bf6a6
}

template  <class Tin,class Tout> bool LegoThread<Tin,Tout>::BoolRecieve(int mailbox, bool remove)
{
	ViUInt8 directCommandBuffer[] = { 0x13, mailbox-1, 0x00, remove };
	ViUInt8* directBoolCommandBuffer = NULL;

	
	ViUInt8 responseBuffer[64];
	for(int i = 0; i < 64; i++)
		responseBuffer[i] = 0x00;


	// Send the direct command to the NXT.
	comm.SendDirectCommand( true, reinterpret_cast< ViByte* >( directCommandBuffer ), sizeof( directCommandBuffer ),
		reinterpret_cast< ViByte* >( responseBuffer ), sizeof( responseBuffer ));

	

	return 0;
}




