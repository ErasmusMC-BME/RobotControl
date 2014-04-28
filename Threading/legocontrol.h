#include "NXT++.h"
#include <stdio.h>
#include <iostream>
#include <windows.h>
#include <cmath>
#include <iostream>
#include <fstream> 
#define rotationaxis_factor 0.16667 // ratio between motor and rotation axis
#define imageplane_rotation 200.0	// amount of degrees the image plane of the probe can rotate
#define distance_zaxis 15.0			// amount of cm the vertical axis can move
#define rotationaxis_max 540		// corresponds with 90 degrees on the rotation axis
#define rotationaxis_min -540		// corresponds with -90 degrees on the rotation axis


const int MAILBOX_A = 1;	//! Position Mailbox for Motor port A on the NXT
const int MAILBOX_B = 2;	//! Position Mailbox for Motor port B on the NXT
const int MAILBOX_C = 3;	//! Position Mailbox for Motor port C on the NXT
const int MAILBOX_INIT   = 4; //! Intialization Mailbox for Motors
const int MAILBOX_START  = 5; //! Start Mailbox for Motors
const int MAILBOX_RESET  = 6; //! Start Mailbox for Motors
const int MAILBOX_RECIEVE = 8;


// Image plane movement functions
//	int image_min, image_max;
class Lego
{
private:
	Comm::NXTComm comm;
	
	// PID functions
	// Controls the specified motor with a PID control
	void PID(float z, float P, float I, float D, int minpower, int maxpower, const int Port);
	// Corrects the power out of the PID with its minimum and maximum
	int normalise_power(float power, int minpower, int maxpower);
	// Save the selected data in a .txt file
	void save_data(char filename[256], std::vector<float> plot);
	std::ofstream outStream;

	// Vertical axis movement functions
	int zmax, zmin;
	float zfactor;
	// Checks if vertical coordinate is in range
	int limit_check_z(int z);
	// Transforms vertical coordinate to motor counts
	int normalise_zcoord(float z);
	
	// Rotation axis movement functions
	// Checks if the rotation angle is in range
	int limit_check_angle(int angle);
	// Transforms the rotation angle to motor counts
	int normalise_angle(float angle);
		
	// Image plane movement functions
	int image_min, image_max;
	float imagefactor;
	// Checks if the angle of the image plane is in range
	int limit_check_imageplane(int angle);
	// Transforms the angle of the image plane to motor counts
	int normalise_imageangle(float angle);
			
	// Calibration sub functions
	// Calibrates the minimum
	int calibration_min(const int Port, int power);
	// Calibrates the maximum
	int calibration_max(const int Port, int power);
	// Calibrates the rotation axis
	void calibration_rotationaxis(const int Port, int power);
		
	public:
	// Make connection with the NXT
	int init();
	// Perform the calibration
	int calibration();
	// Moves the robot absolute to given position
	void move(float z, float angle, float image_angle);
	// Moves the robot relative to given position
	void move_relative(float z, float angle, float image_angle);
	// Get the image angle
	float get_image_angle();
	// Get the height of the probe
	float get_probe_height();
	// Get the rotation angle
	float get_rotation_angle();
	void TextMessageSend(std::string message, int inbox);
	std::string TextMessageRecieve(int mailbox, bool remove);
	void WordSend(int Value, int inbox);
	int WordRecieve(int mailbox, bool remove);
	void BoolSend(bool Value, int inbox);
	bool BoolRecieve(int mailbox, bool remove);

};