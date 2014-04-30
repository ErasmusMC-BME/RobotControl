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
	if(NXT::OpenBT(&comm)) //initialize the NXT and continue if it succeeds
	{
		std::string name="receiverFromPC1";
		_bLegoFound=true;
		NXT::StartProgram(&comm,name);
		std::cout << "NXT Connection made" << std::endl;
		return;
	}
	else
	{
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
 
	_endthread();
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
	directLongCommandBuffer  = new ViUInt8[8];
	directLongCommandBuffer[0] = 0x09;
	directLongCommandBuffer[1] = inbox-1; // Mailbox number -1 (e.g. for mailbox 1 use Ox00 
	directLongCommandBuffer[2] = 0x05;	// this is the datalength: 1 byte + a 1 byte terminator 
	directLongCommandBuffer[3] = LOBYTE(Value);	// the LSB 
	directLongCommandBuffer[4] = HIBYTE(Value);
	directLongCommandBuffer[5] = LOUPPERBYTE(Value);
	directLongCommandBuffer[6] = HIUPPERBYTE(Value);// the MSB 
	directLongCommandBuffer[7] = NULL;	// terminator
	comm.SendDirectCommand( false, reinterpret_cast< ViByte* >( directLongCommandBuffer ), 8,NULL,0);

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
	ViUInt8 directCommandBuffer[] = { 0x13, mailbox+9, 0x00, remove };
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




