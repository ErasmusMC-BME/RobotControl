#include "legocontrol.h"
#include <cmath>

int Lego::init()
{
	bool bNXTFound = false;
	
	if(NXT::Open(&comm)) //initialize the NXT and continue if it succeds USB 

	//if(NXT::OpenBT(&comm)) //initialize the NXT and continue if it succeeds
	{
		bNXTFound=true;
		std::cout << "Connection made" << std::endl;
		return 1;
	}
	else
	{
		std::cout << "Could not make connection" << std::endl;
		return 0;
	}
	
}

int Lego::limit_check_z(int z)
{
	if(z > zmax)
	{
		z = zmax;
		std::cout << "z is out of boundaries and is set to maximum" << std::endl;
	}
	if(z < zmin)
	{
		z = zmin;
		std::cout << "z is out of boundaries and is set to minimum" << std::endl;
	}
	else
	{
		z = z;
	}
	return z;
}

int Lego::limit_check_imageplane(int angle)
{
	if(angle > image_max)
	{
		angle = image_max;
		std::cout << "angle is out of boundaries and is set to maximum" << std::endl;
	}
	if(angle < image_min)
	{
		angle = image_min;
		std::cout << "angle is out of boundaries and is set to minimum" << std::endl;
	}
	else
	{
		angle = angle;
	}
	return angle;
}

int Lego::limit_check_angle(int angle)
{
	if(angle > rotationaxis_max)
	{
		angle = rotationaxis_max;
		std::cout << "angle is out of boundaries and is set to maximum" << std::endl;
	}
	if(angle < rotationaxis_min)
	{
		angle = rotationaxis_min;
		std::cout << "angle is out of boundaries and is set to minimum" << std::endl;
	}
	else
	{
		angle = angle;
	}
	return angle;
}

int Lego::normalise_zcoord(float z)
{
	z = (z * zfactor) + 0.5;
	return z;
}

int Lego::normalise_imageangle(float angle)
{
	angle = (angle * imagefactor) + 0.5;
	return angle;
}

int Lego::normalise_angle(float angle)
{
	if(angle >= 0)
	{
		angle = (angle / rotationaxis_factor) + 0.5;
		angle = (int)angle;
	}
	if(angle < 0)
	{
		angle = (angle / rotationaxis_factor) - 0.5;
		angle = (int)angle;
	}
	return angle;
}

int Lego::calibration()
{	
	// z-axis calibration
	
	zmax = calibration_max(OUT_A, 30);
	zmin = calibration_min(OUT_A, -30);

	std::cout << zmin << std::endl;
	std::cout << zmax << std::endl;

	zfactor = (zmax - zmin)/ distance_zaxis;
	
	//rotation axis calibration
	calibration_rotationaxis(OUT_B, 75);
	
	// image plane calibration
	image_min = calibration_min(OUT_C, -68);
	image_max = calibration_max(OUT_C, 68);

	std::cout << image_min << std::endl;
	std::cout << image_max << std::endl;

	imagefactor = (image_max - image_min)/ imageplane_rotation;

	//check if calibration is done right
	if (zmin && image_min == 0)
	{
		std::cout << "calibration is done" << std::endl;
		return 1;
	}

	else
	{
		std::cout << "Calibration did not succeed" << std::endl;
		return 0;
	}
	return 1;
}

int Lego::calibration_min(const int Port, int power)
{
	if(power >= 0)
	{
		power = -power;
	}
	ViUInt8 setreverse[] = { 0x04, Port, power, 0x01 | 0x04, 0x00, 0, 0x20, 0, 0, 0, 0 };
	comm.SendDirectCommand(false, reinterpret_cast< ViByte* >(setreverse), sizeof(setreverse), NULL, 0);
	//NXT::Motor::SetReverse(&comm, Port, power);

	int rot_count = NXT::Motor::GetRotationCount(&comm, Port);
	std::cout << "Motor is calibrating" << std::endl;
	Wait(200);
	while(NXT::Motor::GetRotationCount(&comm, Port) != rot_count)
	{
		rot_count = NXT::Motor::GetRotationCount(&comm, Port);
		//Wait(50);
	}
	NXT::Motor::Stop(&comm, Port, true);
	Wait(500);
	NXT::Motor::ResetRotationCount(&comm, Port, false);
	int min_value = NXT::Motor::GetRotationCount(&comm, Port);
	return min_value;
}

int Lego::calibration_max(const int Port, int power)
{
	if(power <= 0)
	{
		power = -power;
	}
	ViUInt8 setforward[] = { 0x04, Port, power, 0x01 | 0x04, 0x00, 0, 0x20, 0, 0, 0, 0 };
	comm.SendDirectCommand(false, reinterpret_cast< ViByte* >(setforward), sizeof(setforward), NULL, 0);
	//NXT::Motor::SetForward(&comm, Port, power);

	int rot_count = NXT::Motor::GetRotationCount(&comm, Port);
	std::cout << "Motor is calibrating" << std::endl;
	Wait(200);
	while(NXT::Motor::GetRotationCount(&comm, Port) != rot_count)
	{
		rot_count = NXT::Motor::GetRotationCount(&comm, Port);
		//Wait(50);
	}
	NXT::Motor::Stop(&comm, Port, true);
	Wait(500);
	int max_value = NXT::Motor::GetRotationCount(&comm, Port);
	return max_value;
}

void Lego::calibration_rotationaxis(const int Port, int power)
{
	ViUInt8 setforward[] = { 0x04, Port, power, 0x01 | 0x04, 0x00, 0, 0x20, 0, 0, 0, 0 };
	comm.SendDirectCommand(false, reinterpret_cast< ViByte* >(setforward), sizeof(setforward), NULL, 0);
	//NXT::Motor::SetForward(&comm, Port, power);

	NXT::Sensor::SetTouch(&comm, IN_1);
	std::cout << "Motor is calibrating" << std::endl;
	Wait(200);
	while(1)
	{
		if(NXT::Sensor::GetValue(&comm, IN_1) == true)
		{
			NXT::Motor::Stop(&comm, Port, true);
			Wait(500);
			NXT::Motor::ResetRotationCount(&comm, Port, false);
			break;
		}
	}
	std::cout << "Motor rotation axis is stopped" << std::endl;
	int value = NXT::Motor::GetRotationCount(&comm, Port);
	std::cout << "position: " << value << std::endl;
}

void Lego::move(float z, float angle, float image_angle)
{
	// move vertical axis
	z = normalise_zcoord(z);
	z = limit_check_z(z);
	float P = 0.035;
	float I = 0.000015;
	float D = 2.0;
	PID(z, P, I, D, 25, 100, OUT_A);

	// move rotation axis
	angle = normalise_angle(angle);
	angle = limit_check_angle(angle);
	P = 0.08;
	I = 0.00007;
	D = 0.0;
	PID(angle, P, I, D, 68, 100, OUT_B);

	// move image plane
	image_angle = normalise_imageangle(image_angle);
	image_angle = limit_check_imageplane(image_angle);
	P = 0.17;
	I = 0.0;
	D = 0,0;
	PID(image_angle, P, I, D, 62, 100, OUT_C);
}

void Lego::move_relative(float z, float angle, float image_angle)
{
	// move vertical axis
	z = normalise_zcoord(z);
	std::cout << NXT::Motor::GetRotationCount(&comm, OUT_A) << std::endl;
	z = NXT::Motor::GetRotationCount(&comm, OUT_A) + z;
	std::cout << z << std::endl;
	z = limit_check_z(z);
	float P = 0.035;
	float I = 0.000015;
	float D = 2.0;
	PID(z, P, I, D, 20, 100, OUT_A);

	// move rotation axis
	angle = normalise_angle(angle);
	std::cout << NXT::Motor::GetRotationCount(&comm, OUT_B) << std::endl;
	angle = NXT::Motor::GetRotationCount(&comm, OUT_B) + angle;
	std::cout << angle << std::endl;
	angle = limit_check_angle(angle);
	P = 0.08;
	I = 0.00007;
	D = 0.0;
	PID(angle, P, I, D, 68, 100, OUT_B);

	// move image plane
	image_angle = normalise_imageangle(image_angle);
	std::cout << NXT::Motor::GetRotationCount(&comm, OUT_C) << std::endl;
	image_angle = NXT::Motor::GetRotationCount(&comm, OUT_C) + image_angle;
	std::cout << image_angle << std::endl;
	image_angle = limit_check_imageplane(image_angle);
	P = 0.17;
	I = 0.00;
	D = 0.0;
	PID(image_angle, P, I, D, 62, 100, OUT_C);
}

void Lego::PID(float z, float P, float I, float D, int minpower, int maxpower, const int Port)
{
	int error = 10;
	int error_previous = 0;
	int delta_error;
	int angle_current;
	float differential;
	float proportional;
	float integral = 0;
	clock_t new_time;
	clock_t new_time2;
	clock_t delta_time = 70;
	clock_t delta_time2;
	float power = 1;
	int vectorLength = 600;
	std::vector<float> errorV( vectorLength, 0.0 );
	std::vector<float> error_previousV( vectorLength, 0.0 );
	std::vector<float> timeV( vectorLength, 0.0 );
	std::vector<float> pw1V( vectorLength, 0 );
	std::vector<float> pw2V( vectorLength, 0 );
	std::vector<float> angleV( vectorLength, 0 );
	std::vector<float> PV( vectorLength, 0 );
	std::vector<float> IV( vectorLength, 0 );
	std::vector<float> DV( vectorLength, 0 );
	std::vector<float> deltatimeV( vectorLength, 0 );
	int count = 0;
	int count2 = 0;
	
	while ( error < -10 || error > 10 || error != error_previous)
	{
		new_time = clock();
		timeV[count] = new_time;
		error_previous = error;
		error_previousV[count] = error_previous;
		angle_current = NXT::Motor::GetRotationCount(&comm, Port);
		delta_time2 = clock() - new_time;
		deltatimeV[count2] = delta_time2;
		count2++;
		angleV[count] = angle_current;
		error = z - angle_current;
		errorV[count] = error;
		delta_error = error - error_previous;
		differential = D * (delta_error/float(delta_time));
		DV[count] = differential;
		integral += I * error * delta_time;
		IV[count] = integral;
		proportional = P * error;
		PV[count] = proportional;
		power = proportional + integral + differential;
		pw1V[count] = power;
		power = normalise_power(power, minpower, maxpower);
		pw2V[count] = power;
		//new_time2 = clock();
		ViUInt8 setforward[] = { 0x04, Port, power, 0x01, 0x00, 0, 0x20, 0, 0, 0, 0 };//| 0x04
		comm.SendDirectCommand(false, reinterpret_cast< ViByte* >(setforward), sizeof(setforward), NULL, 0);
		//delta_time2 = clock() - new_time2;
		//deltatimeV[count2] = delta_time2;
		//count2++;
		std::cerr << "P: " << power << std::endl;
		std::cout << "Pos: " << angle_current << std::endl;
		//std::cout << "D: " << differential << std::endl;
		//std::cout << "I: " << integral << std::endl;
		std::cout << "E: " << error << std::endl;
		std::cout << "EP: " << error_previous << std::endl;
		/*std::cout << delta_time << std::endl;*/
		delta_time = clock() - new_time;
		count++;
	}
	ViUInt8 stop[] = { 0x04, Port, 0, 0x01 | 0x02, 0x00, 0, 0x20, 0, 0, 0, 0 };
	comm.SendDirectCommand( false, reinterpret_cast< ViByte* >( stop ), sizeof( stop ), NULL, 0);
	int i;
	for(i = 0; i < 100; i++)
	{
		new_time = clock();
		timeV[count] = new_time;
		angle_current = NXT::Motor::GetRotationCount(&comm, Port);
		angleV[count] = angle_current;
		error = z - angle_current;
		errorV[count] = error;
		count++;
	}
	Wait(2000);
	if(Port == OUT_A)
	{
	save_data("OUT_A_proportional.txt", PV);
	save_data("OUT_A_differential.txt", DV);
	save_data("OUT_A_integral.txt", IV);
	save_data("OUT_A_deltatime.txt", deltatimeV);
	save_data("OUT_A_error.txt", errorV);
	save_data("OUT_A_time.txt", timeV);
	save_data("OUT_A_pw1.txt", pw1V);
	save_data("OUT_A_pw2.txt", pw2V);
	save_data("OUT_A_angle.txt", angleV);
	save_data("OUT_A_error_previous.txt", error_previousV);
	}
	if(Port == OUT_B)
	{
	save_data("OUT_B_proportional.txt", PV);
	save_data("OUT_B_differential.txt", DV);
	save_data("OUT_B_integral.txt", IV);
	save_data("OUT_B_deltatime.txt", deltatimeV);
	save_data("OUT_B_error.txt", errorV);
	save_data("OUT_B_time.txt", timeV);
	save_data("OUT_B_pw1.txt", pw1V);
	save_data("OUT_B_pw2.txt", pw2V);
	save_data("OUT_B_angle.txt", angleV);
	save_data("OUT_B_error_previous.txt", error_previousV);
	}
	if(Port == OUT_C)
	{
	save_data("OUT_C_proportional.txt", PV);
	save_data("OUT_C_differential.txt", DV);
	save_data("OUT_C_integral.txt", IV);
	save_data("OUT_C_deltatime.txt", deltatimeV);
	save_data("OUT_C_error.txt", errorV);
	save_data("OUT_C_time.txt", timeV);
	save_data("OUT_C_pw1.txt", pw1V);
	save_data("OUT_C_pw2.txt", pw2V);
	save_data("OUT_C_angle.txt", angleV);
	save_data("OUT_C_error_previous.txt", error_previousV);
	}
	std::cout << "position: " << NXT::Motor::GetRotationCount(&comm, Port) << std::endl;
}

int Lego::normalise_power(float power, int minpower, int maxpower)
{	
	if(power >= 0)
	{
		power = power + 0.5;
		power = (int)power;
	}
	if(power < 0)
	{
		power = power - 0.5;
		power = (int)power;
	}
	if(power >= 1)
	{
		power += minpower;
	}
	if(power <= -1)
	{
		power -= minpower;
	}
	if(power > maxpower)
	{
		power = maxpower;
	}
	if(power < -maxpower)
	{
		power = -maxpower;
	}
	return power;
}

void Lego::save_data(char filename[256], std::vector<float> plot)
{
	/*std::string filename;
	std::cout << "Give the name of the file: " << std::endl;
	std::cin >> filename;*/
	outStream.open( filename, std::ios::out );
	for (int itr = 0; itr < plot.size(); itr++ )
	{
		outStream << plot[itr] << std::endl;
	}
	outStream.close();
}

float Lego::get_probe_height()
{
	int angle = NXT::Motor::GetRotationCount(&comm, OUT_A);
	float hight = angle / zfactor;
	return hight;
}

float Lego::get_rotation_angle()
{
	int angle = NXT::Motor::GetRotationCount(&comm, OUT_B);
	float rotation_angle = angle * rotationaxis_factor;
	return rotation_angle;
}

float Lego::get_image_angle()
{
	int angle = NXT::Motor::GetRotationCount(&comm, OUT_C);
	float image_angle = angle / imagefactor;
	return image_angle;
}
void Lego::TextMessageSend(std::string message, int inbox)
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

void Lego::WordSend(int Value, int inbox)
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
void Lego::BoolSend(bool bValue, int inbox)
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

std::string Lego::TextMessageRecieve(int mailbox, bool remove)
{
BYTE loByte = 17;
BYTE hiByte = 45;
	ViUInt8 directCommandBuffer[] = { 0x13, mailbox+9, 0x00, remove };
	ViUInt8 responseBuffer[64];
	for(int i = 0; i < 64; i++)
		responseBuffer[i] = 0x00;


	// Send the direct command to the NXT.
	comm.SendDirectCommand( true, reinterpret_cast< ViByte* >( directCommandBuffer ), sizeof( directCommandBuffer ),
		reinterpret_cast< ViByte* >( responseBuffer ), sizeof( responseBuffer ));
	for(int j = 0; j < 64; j++)
		std::cout << (int)responseBuffer[j] << "\n";


WORD wordVal = MAKEWORD(loByte, hiByte);
	return NULL;
}
int Lego::WordRecieve(int mailbox, bool remove)
{
	return 0;
}
bool Lego::BoolRecieve(int mailbox, bool remove)
{
	return 0;
}


