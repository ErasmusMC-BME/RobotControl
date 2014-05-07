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
	comm.Close();
}
template  <class Tin,class Tout>  void LegoThread<Tin,Tout>::Close( void )
{
	comm.Close();
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
	//if(NXT::Open(&comm)) //initialize the NXT and continue if it succeeds
	//{
	//	std::string name="receiverFromPC1\n";
	//	_bLegoFound=true;
	//	NXT::StartProgram(&comm,name);
	//	std::cout << "NXT Connection made with USB " << std::endl;
	//	return;
	//}
	//else
	{
		if(NXT::OpenBT(&comm)) //initialize the NXT and continue if it succeeds
		{
			std::string name="receiverFromPC1";
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

	// This is the desired entr`y-point-function but to get
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
				zz=record.z*Zfactor/10;  // corresponds to 1 CM (-7.5.. +7.5)
				aa=record.y*Anglefactor; // corresponds to -45 deg (-90.. +90)
				bb=record.x*Imagefactor; // corresponds to -45 deg  (-90.. +90)
				WaitForNxtDone(true); //empty buffer
				WordSend(zz,MAILBOX_A);//Z Plane + <---> -
				WordSend(aa,MAILBOX_B);//Angle 0 <---> -
				WordSend(bb,MAILBOX_C);//Image Plane  + <---> -
				//BoolRecieve(9,false);
				BoolSend(bValue,MAILBOX_START);//Start Motors
				//BoolRecieve(9,true);
				//BoolRecieve(10,false);
				//BoolRecieve(9,false);
				WaitForNxtDone(false);
			//	WaitForRotationIdle();
			//	TextMessageRecieve(MAILBOX_RECIEVE,false);


 	} while (1);
	_endthread();
}


// not in yet !!! have to test for timeout
template  <class Tin,class Tout> void LegoThread<Tin,Tout>::WaitForNxtDone(bool bFlag)
{
	if(bFlag)  // clear buffer, use before sending rotation command to be shure no message is pending
	{
		do 
		{
		} while (BoolRecieve(9,true)); //messagebox 9 trou to clear message after reading
	}
	else
	{
		do 
		{
		} while (!BoolRecieve(9,true));
	}

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
		WaitForNxtDone(true);
		BoolSend(bValue,MAILBOX_INIT);//init PID; calibrate motors
    WaitForNxtDone(false);
		WordSend(0,MAILBOX_A);//Z Plane + <---> -
		WordSend(0,MAILBOX_B);//Angle 0 <---> -
		WordSend(0,MAILBOX_C);//Image Plane  + <---> -
		BoolSend(bValue,MAILBOX_START);//Start Motor
		WaitForNxtDone(false);
	}
}





/*
MESSAGEWRITE
Byte 0: 0x00  or 0x80
Byte 1: 0x09
Byte 2: Inbox number (0 - 9) 
Byte 3: Message  size
Byte 4 - N: Message  data, where N = Message  size + 3

Message  data is treated as a string; it must include null termination to be accepted.  
Accordingly, message size must include the null termination byte. 
Message  size must be capped at 59 for all message packets to be legal  on USB!

Return package: 
Byte 0: 0x02        
Byte 1: 0x09
Byte 2: Status Byte

Note!    MAXIMUM COMMAND LENGTH
Currently, total  direct command telegram size is limited to 64 bytes, including the telegram type byte (Byte 0). 

!!!! lowlevel Byte 0: 0x02 is not included in the SendDirectCommand so the responseBuffer size should be 63 bytes !!!!!
As specified in the LEGO®  MINDSTORMS® NXT Communication protocol document, 
Bluetooth® packets have an additional two bytes  for size tacked onto the front, but these  are not included in this limit.

*/



template  <class Tin,class Tout> void LegoThread<Tin,Tout>::TextMessageSend(std::string message, int inbox)
{
	
	int length = message.size();
	const char *charmessage = message.c_str();
	ViUInt8* directTextCommandBuffer = NULL;
	directTextCommandBuffer  = new ViUInt8[3+length+1];
	directTextCommandBuffer[0] = 0x09;
	directTextCommandBuffer[1] = inbox-1;	// Mailbox number -1 (e.g. for mailbox 1 use Ox00 
	directTextCommandBuffer[2] = length+1;	// Byte 3: Message size 
	for(int i = 3; i < 3+length; i++)		// Byte 4 - N: Message data, where N = Message size + 3 
		directTextCommandBuffer[i] = charmessage[i-3];
	directTextCommandBuffer[length+3] = '\0';
	comm.SendDirectCommand( false, reinterpret_cast< ViByte* >( directTextCommandBuffer ),sizeof( directTextCommandBuffer ),NULL,0);

}


#define LOUPPERBYTE(w)           ((BYTE)((((DWORD_PTR)(w)) >> 16) & 0xff))
#define HIUPPERBYTE(w)           ((BYTE)((((DWORD_PTR)(w)) >> 24) & 0xff))

template  <class Tin,class Tout> void LegoThread<Tin,Tout>::WordSend(int Value, int inbox)
{

	ViUInt8 directBoolSetInputCommandBuffer[] = { 0x05, 0, 0,'\0' };
	comm.SendDirectCommand( false, reinterpret_cast< ViByte* >( directBoolSetInputCommandBuffer ),5,NULL,0);
	ViUInt8* directLongCommandBuffer = NULL;

	directLongCommandBuffer  = new ViUInt8[8];
	directLongCommandBuffer[0] = 0x09;
	directLongCommandBuffer[1] = inbox-1; // Mailbox number -1 (e.g. for mailbox 1 use Ox00 
	directLongCommandBuffer[2] = 0x05;	// this is the datalength: 1 byte + a 1 byte terminator 
	directLongCommandBuffer[3] = LOBYTE(Value);	// the LSB 
	directLongCommandBuffer[4] = HIBYTE(Value);
	directLongCommandBuffer[5] = LOUPPERBYTE(Value);
	directLongCommandBuffer[6] = HIUPPERBYTE(Value);// the MSB 
	directLongCommandBuffer[7] = '\0';	// terminator
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
	directBoolCommandBuffer[4] = '\0';// terminator

	comm.SendDirectCommand( false, reinterpret_cast< ViByte* >( directBoolCommandBuffer ),5,NULL,0);
}
/*
MESSAGEREAD
Byte 0: 0x00  or 0x80
Byte 1: 0x13
Byte 2: Remote  Inbox number (10 - 19) 
Byte 3: Local  Inbox number (0 - 9)
Byte 4: Remove? (Boolean; TRUE (non-zero)  value clears  message from  Remote  Inbox)

Return package: 
Byte 0: 0x02       
Byte 1: 0x13
Byte 2: Status Byte
Byte 3: Local  Inbox number (0 - 9) 
Byte 4: Message  size
Byte 5 - 63: Message  data  (padded)

Message  data is treated as a string; it must include null termination.
Accordingly, message size includes the null termination byte.  Furthermore, 
return  packages have a fixed  size, so the message data field  will be padded with null bytes.

Note that the remote  Inbox number may specify a value of 0-19, 
while all other  mailbox  numbers should remain  below  9.  
This is due to the master-slave relationship between connected NXT bricks.  
Slave devices may not initiate  communication transactions with their masters, 
so they store outgoing
messages in the upper  10 mailboxes (indices 10-19).   
Use the MessageRead command from the master device  to retrieve  these  messages.
*/


template  <class Tin,class Tout> std::string LegoThread<Tin,Tout>::TextMessageRecieve(int mailbox, bool remove)
{
//BYTE loByte = 17;
//BYTE hiByte = 45;
	ViUInt8 directCommandBuffer[] = { 0x13, mailbox+9, mailbox-1, remove };
	ViUInt8 responseBuffer[63];
	for(int i = 0; i < 63; i++)
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
}

template  <class Tin,class Tout> bool LegoThread<Tin,Tout>::BoolRecieve(int mailbox, bool remove)
{
	ViUInt8 directBoolSetInputCommandBuffer[] = { 0x05, 0, 0,'\0' };
	comm.SendDirectCommand( false, reinterpret_cast< ViByte* >( directBoolSetInputCommandBuffer ),5,NULL,0);

	ViUInt8 directCommandBuffer[] = { 0x13, mailbox+9, mailbox-1, remove,'\0' };
	ViUInt8 responseBuffer[63] ; //NOTE !!! Low Level Return package: 64 Bytes lowlevel Byte 0: 0x02 is not included here so this should be 63 bytes 
	responseBuffer[62]='\0';
	for(int i = 0; i < 62; i++)
		responseBuffer[i] = 0x02;
	// Send the direct command to the NXT.
	comm.SendDirectCommand( true, reinterpret_cast< ViByte* >( directCommandBuffer ), sizeof( directCommandBuffer ),
	reinterpret_cast< ViByte* >( responseBuffer ), sizeof( responseBuffer ));
	if(responseBuffer[4]==1)
	{
		return true;
	}
	else
		return false;
}

// Testcode Gerard
template  <class Tin,class Tout> int LegoThread<Tin,Tout>::LSGetStatus(int port)
{
	ViUInt8 directCommandBuffer[] = {0x0E, port};
	ViUInt8 responseBuffer[] = {1,1,1};


	comm.SendDirectCommand( true, reinterpret_cast< ViByte* >( directCommandBuffer ), sizeof( directCommandBuffer),
		   reinterpret_cast< ViByte* >( responseBuffer ), sizeof( responseBuffer ));

	return (int)responseBuffer[2];
}

template  <class Tin,class Tout> int LegoThread<Tin,Tout>::GetRotationCount(int port)
{
	ViUInt8 directCommandBuffer[] = { 0x06, port };
	ViInt8 responseBuffer[] = { 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 };

	// Send the direct command to the NXT.
	comm.SendDirectCommand( true, reinterpret_cast< ViByte* >( directCommandBuffer ), sizeof( directCommandBuffer ),
		reinterpret_cast< ViByte* >( responseBuffer ), sizeof( responseBuffer ));

	int i = responseBuffer[20];
	if(i < 0)
		i = 256 + i;
	if(responseBuffer[22] == -1)
		responseBuffer[22] = 0;
	if(responseBuffer[23] == -1)
		responseBuffer[23] = 0;

	int tacho = responseBuffer[23]*16777216+responseBuffer[22]*65536+responseBuffer[21]*256+i;

	return tacho;
}
// End  Testcode Gerard
//Code i found on the internet https://github.com/esteve/nxtpp/blob/master/src/nxt%2B%2B.cpp

/* DOESN'T WORK YET - DON'T TRY - JUST TEST CODE
void MessageSend(std::string message, int inbox)
{
		//ViUInt8 directCommandBuffer[] = { 0x0A, port, relative };
		//Comm::SendDirectCommand( false, reinterpret_cast< ViByte* >( directCommandBuffer ), sizeof( directCommandBuffer ), NULL, 0);
	int length = message.size();
	const char *charmessage = message.c_str();
	//ViUInt8 *directCommandBuffer  = (ViUInt8*) malloc(3+length);
	ViUInt8* directCommandBuffer = NULL;
	directCommandBuffer  = new ViUInt8[3+length+1];
	directCommandBuffer[0] = 0x09;
	directCommandBuffer[1] = inbox-1;
	directCommandBuffer[2] = length+1;
	for(int i = 3; i < 3+length; i++)
		directCommandBuffer[i] = charmessage[i-3];
	directCommandBuffer[length+3] = NULL;
	for(int j = 0; j < 4+length; j++)
		std::cout << (int)directCommandBuffer[j] << "\n";
	//ViUInt8 directCommandBuffer[] = { 0x01 };
	Comm::SendDirectCommand( false, reinterpret_cast< ViByte* >( directCommandBuffer ), sizeof( directCommandBuffer ), NULL, 0);
}


std::string MessageRecieve(int mailbox, bool remove)
{
	ViUInt8 directCommandBuffer[] = { 0x13, mailbox, 0x00, remove };
	ViUInt8 responseBuffer[64];
	for(int i = 0; i < 64; i++)
		responseBuffer[i] = 0x00;


	// Send the direct command to the NXT.
	Comm::SendDirectCommand( true, reinterpret_cast< ViByte* >( directCommandBuffer ), sizeof( directCommandBuffer ),
		reinterpret_cast< ViByte* >( responseBuffer ), sizeof( responseBuffer ));
	for(int j = 0; j < 64; j++)
		std::cout << (int)responseBuffer[j] << "\n";


	return "";
}
*/
