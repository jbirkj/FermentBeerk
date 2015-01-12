/*
=================================================================================
 Name        : PCA9634test.c
 Version     : 0.1
================================================================================
 */

#include <stdio.h>
#include <fcntl.h>
#include <ncurses.h>
#include "PCAtest.h"
#include "DS2482.h"
#include <linux/i2c-dev.h>
#include "main.h"
//#include "debug.h"


int incDutyCycle(deviceParm dP) {
	
	printf("Increase duty cycle \n");
	char buffer[3];
	int readBytes;
	int deviceHandle;
	
	buffer[0]=0b10000010; 	//auto inc and address 2h
	
	// open device on /dev/i2c-1 note Rev.B board use addr 1
	if ((deviceHandle = open("/dev/i2c-1", O_RDWR)) < 0) {
		printf("Error: Couldn't open device! %d\n", deviceHandle);
		return 1;
	}
	// connect to PCA9634 as i2c slave
	if (ioctl(deviceHandle, I2C_SLAVE, dP.deviceI2CAddress) < 0) {
		printf("Error: Couldn't find device on address!\n");
		return 1;
	}  

	readBytes = write(deviceHandle,&buffer[0], 1);
	if (readBytes != 1) {
		printf("Error: read count does not match.Received no ACK-Bit, couldn't established connection!\n");
	}
	else {
		readBytes = read(deviceHandle, &buffer[1], 2);
		if (readBytes != 2){	
		printf("Error: read count does not match.Received no ACK-Bit, couldn't established connection!\n");
		}
		//printf("PCA9634 regs read\n");
		// get data
		char PWM0 = buffer[1];		// 1-23
		char PWM1 = buffer[2];		// 1-23
			
		// and print results
		printf("existing PWM0: %x and PWMM1: %x\n", PWM0, PWM1);
			
	}

	// begin transmission and request acknowledgement
	buffer[1] += 0x10;
	buffer[2] += 0x10;
	readBytes = write(deviceHandle,&buffer[0], 3);
	if (readBytes != 3)
	{
		printf("Error: write count does not match.Received no ACK-Bit, couldn't established connection!\n");
	}
	else {
		printf("PWM0 updated to %x and PWM1 to %x\n\n", buffer[1],buffer[2]);
		}

	// close connection and return
	close(deviceHandle);
	return 0;
	}

int stopPCA_PWM0(deviceParm dP) {
	
	//printf("Disabling PWM0...\n");
	char buffer[3];
	int readBytes;
	int deviceHandle;
	
	buffer[0]=0x0C; //LEDOUT0 address
	buffer[1]=0b00000000;	//disable PWMn outputs 0-3
	buffer[2]=0b00000000;	//disable all PWMn outputs 4-7
	
	// open device on /dev/i2c-1 note Rev.B board use addr 1
	if ((deviceHandle = open("/dev/i2c-1", O_RDWR)) < 0) {
		printf("Error: Couldn't open device! %d\n", deviceHandle);
		return 1;
	}
	// connect to PCA9634 as i2c slave
	if (ioctl(deviceHandle, I2C_SLAVE, dP.deviceI2CAddress) < 0) {
		printf("Error: Couldn't find device on address!\n");
		return 1;
	}  
	// begin transmission and request acknowledgement
	readBytes = write(deviceHandle,&buffer[0], 3);
	if (readBytes != 3)
	{
		printf("Error: write count does not match.Received no ACK-Bit, couldn't established connection!\n");
	}
	else {
		printf("All PWM outputs 0-7 disabled\n");
		}

		// close connection and return
	close(deviceHandle);
	return 0;

	
}

int startPCA_PWM0(deviceParm dP) {
	
	//printf("Enabling  PWM0...\n");
	char buffer[2];
	int readBytes;
	int deviceHandle;
	
	buffer[0]=0x0C; //no addr increment and LEDOUT0 address
	buffer[1]=0b00001010;	//enable bit for PWM0 and PWM1
	
	// open device on /dev/i2c-1 note Rev.B board use addr 1
	if ((deviceHandle = open("/dev/i2c-1", O_RDWR)) < 0) {
		printf("Error: Couldn't open device! %d\n", deviceHandle);
		return 1;
	}
	// connect to PCA9634 as i2c slave
	if (ioctl(deviceHandle, I2C_SLAVE, dP.deviceI2CAddress) < 0) {
		printf("Error: Couldn't find device on address!\n");
		return 1;
	}  

	// begin transmission and request acknowledgement
	readBytes = write(deviceHandle,&buffer[0], 2);
	if (readBytes != 2)
	{
		printf("Error: write count does not match.Received no ACK-Bit, couldn't established connection!\n");
	}
	else {
		printf("PWM0 and PWM1 started\n");
		}

		// close connection and return
	close(deviceHandle);
	return 0;

	
}

int decreDutyCycle(deviceParm dP) {
	
	printf("Decrease duty cycle \n");
	
	char buffer[3];
	int readBytes;
	int deviceHandle;
	
	buffer[0]=0b10000010;	//addr auto inc and PWM0 address	
	// open device on /dev/i2c-1 note Rev.B board use addr 1
	if ((deviceHandle = open("/dev/i2c-1", O_RDWR)) < 0) {
		printf("Error: Couldn't open device! %d\n", deviceHandle);
		return 1;
	}
	// connect to PCA9634 as i2c slave
	if (ioctl(deviceHandle, I2C_SLAVE, dP.deviceI2CAddress) < 0) {
		printf("Error: Couldn't find device on address!\n");
		return 1;
	}  
	
	// read to check state prior to setting new value
	readBytes = write(deviceHandle,&buffer[0], 1);
	if (readBytes != 1) {
		printf("Error: read count does not match.Received no ACK-Bit, couldn't established connection!\n");
	}
	else {
		readBytes = read(deviceHandle, &buffer[1], 2);
		if (readBytes != 2){	
		printf("Error: read count does not match.Received no ACK-Bit, couldn't established connection!\n");
		}
		//printf("PCA9634 regs read\n");
		// get data
			
		// and print results
		char PWM0 = buffer[1];		// 1-23
		char PWM1 = buffer[2];		// 1-23
		printf("existing PWM0: %x\n", PWM0);
		printf("existing PWM1: %x\n", PWM1);
			
	}

	// begin transmission and request acknowledgement
	buffer[1] -= 0x10;
	buffer[2] -= 0x10;
	readBytes = write(deviceHandle,&buffer[0], 3);
	if (readBytes != 3)
	{
		printf("Error: write count does not match.Received no ACK-Bit, couldn't established connection!\n");
	} 
	else {
		printf("PWM0 and PWM1 decreased to %x and %x\n\n", buffer[1], buffer[2]);
		}

		// close connection and return
	close(deviceHandle);
	return 0;
	}

int InitI2Cdevice(deviceParm dP) {
	
	char buffer[15];
	int readBytes;
	int deviceHandle;
	
	

	// open device on /dev/i2c-1 note Rev.B board use addr 1
	if ((deviceHandle = open("/dev/i2c-1", O_RDWR)) < 0) {
		printf("Error: Couldn't open device! %d\n", deviceHandle);
		return 1;
	}

	// connect to PCA9634 as i2c slave
	if (ioctl(deviceHandle, I2C_SLAVE, dP.deviceI2CAddress) < 0) {
		printf("Error: Couldn't find device on address!\n");
		return 1;
	}  
  
	// begin transmission and request acknowledgement
	readBytes = write(deviceHandle,&dP.parms[0], 15);
	if (readBytes != 15)
	{
		printf("Error: write count does not match.Received no ACK-Bit, couldn't established connection!\n");
	}
	else {
		printf("PCA9634 regs initialized. LED0 enabled\n");
		}

	readBytes = write(deviceHandle,&dP.parms[0], 1);
	if (readBytes != 1)
	{
		printf("Error: read count does not match.Received no ACK-Bit, couldn't established connection!\n");
	}
	else {
		readBytes = read(deviceHandle, &buffer[0], 15);
		if (readBytes != 15){	
		printf("Error: read count does not match.Received no ACK-Bit, couldn't established connection!\n");
		}
		printf("PCA9634 regs read\n");
		// get data
			char MODE1 = buffer[0];	// 0-59
			char MODE2 = buffer[1];	// 0-59
			char PWM0 = buffer[2];		// 1-23
			char PWM1 = buffer[3];		// 1-23
			char LEDOUT0 = buffer[0x0C];	// 1-7
			
			// and print results
			printf("\nMODE1: %x\n", MODE1);
			printf("MODE2: %x\n", MODE2);
			printf("PWM0: %x\n", PWM0);
			printf("PWM1: %x\n", PWM1);
			printf("LEDOUT0: %x\n", LEDOUT0);
			
		}
	// close connection and return
	close(deviceHandle);
	
	return 0;
}

/*
unsigned char I2C_address_DS2482;
int c1WS, cSPU, cPPM, cAPU;
int short_detected;


int DS2482_detect(unsigned char addr)
{
//--------------------------------------------------------------------------
// DS2428 Detect routine that sets the I2C address and then performs a
// device reset followed by writing the configuration byte to default values:
//   1-Wire speed (c1WS) = standard (0)
//   Strong pullup (cSPU) = off (0)
//   Presence pulse masking (cPPM) = off (0)
//   Active pullup (cAPU) = on (CONFIG_APU = 0x01)
//
// Returns: TRUE if device was detected and written
//          FALSE device not detected or failure to write configuration byte
//

   // set global address
   I2C_address_DS2482 = addr;

   // reset the DS2482 ON selected address
   if ( DS2482_reset() )
		printf("DS2482 reset SUCCESS\n");
   else
		printf("DS2482 reset FAILED\n");

   // default configuration
   c1WS = 0;
   cSPU = 0;
   cPPM = 0;
   cAPU = 1;


   // write the default configuration setup
   if (!DS2482_write_config(c1WS | cSPU | cPPM | cAPU))
      return FALSE;

   return TRUE;
}


int DS2482_reset()
{
//--------------------------------------------------------------------------
// Perform a device reset on the DS2482
//
// Returns: TRUE if device was reset
//          FALSE device not detected or failure to perform reset
//

   unsigned char buffer[2];
   int deviceHandle, readBytes;

   // Device Reset
   //   S AD,0 [A] DRST [A] Sr AD,1 [A] [SS] A\ P
   //  [] indicates from slave
   //  SS status byte to read to verify state

//   I2C_start();
	// open device on /dev/i2c-1 note Rev.B board use addr 1
	if ((deviceHandle = open("/dev/i2c-1", O_RDWR)) < 0) {
		printf("Error: Couldn't open device! %d\n", deviceHandle);
		return 1;
	}
	// connect to PCA9634 as i2c slave
	if (ioctl(deviceHandle, I2C_SLAVE, I2C_address_DS2482) < 0) {
		printf("Error: Couldn't find device on address!\n");
		return 1;
	}  


//   I2C_write(I2C_address | I2C_WRITE, EXPECT_ACK);
//   I2C_write(CMD_DRST, EXPECT_ACK);
//   I2C_rep_start();
//   I2C_write(I2C_address | I2C_READ, EXPECT_ACK);
//   status = I2C_read(NACK);
//   I2C_stop();


   
	buffer[0] = 0xF0;
	
	// begin transmission and request acknowledgement
	readBytes = write(deviceHandle,&buffer[0], 1);
	if (readBytes != 1)	{
		printf("Error: write count does not match.Received no ACK-Bit, couldn't established connection!\n");
	}
	else {
		printf("DS2482 Device Reset set\n");
		readBytes = read(deviceHandle, &buffer[1], 1);
		if (readBytes != 1){	
			printf("Error: read count does not match.Received no ACK-Bit, couldn't established connection!\n");
		}
	printf("Register read: %d \n", buffer[1]);

	}
		// close connection and return
	close(deviceHandle);
	
   // check for failure due to incorrect read back of status
   return ((buffer[1] & 0xF7) == 0x10);
}



int DS2482_write_config(unsigned char config)
{
//--------------------------------------------------------------------------
// Write the configuration register in the DS2482. The configuration
// options are provided in the lower nibble of the provided config byte.
// The uppper nibble in bitwise inverted when written to the DS2482.
//
// Returns:  TRUE: config written and response correct
//           FALSE: response incorrect
//

   unsigned char buffer[2];
   int deviceHandle, readBytes;

   // Write configuration (Case A)
   //   S AD,0 [A] WCFG [A] CF [A] Sr AD,1 [A] [CF] A\ P
   //  [] indicates from slave
   //  CF configuration byte to write

   
//   I2C_start();
//   I2C_write(I2C_address | I2C_WRITE, EXPECT_ACK);
//   I2C_write(CMD_WCFG, EXPECT_ACK);
   
//   I2C_write(config | (~config << 4), EXPECT_ACK);
//   I2C_rep_start();
   
//   I2C_write(I2C_address | I2C_READ, EXPECT_ACK);
//   read_config = I2C_read(NACK);
   
//   I2C_stop();



	buffer [0] = 0xD2;
	buffer[1]= config | (~config << 4);
	
	// open device on /dev/i2c-1 note Rev.B board use addr 1
	if ((deviceHandle = open("/dev/i2c-1", O_RDWR)) < 0) {
		printf("Error17: Couldn't open device! %d\n", deviceHandle);
		return 1;
	}
	// connect to PCA9634 as i2c slave
	if (ioctl(deviceHandle, I2C_SLAVE, I2C_address_DS2482) < 0) {
		printf("Error18: Couldn't find device on address!\n");
		return 1;
	}  
   
	// begin transmission and request acknowledgement
	readBytes = write(deviceHandle,&buffer[0], 2);
	if (readBytes != 2)	{
		printf("Error19: write count does not match.Received no ACK-Bit, couldn't established connection!\n");
	}


	
   // check for failure due to incorrect read back of status

	buffer[0] = 0xE1;
	buffer[0] = 0xC3;
	
	// begin transmission and request acknowledgement
	readBytes = write(deviceHandle,&buffer[0], 1);
	if (readBytes != 1)	{
		printf("Error20: write count does not match.Received no ACK-Bit, couldn't established connection!\n");
	}
	else {
		readBytes = read(deviceHandle, &buffer[1], 1);
		if (readBytes != 1){	
			printf("Error21: read count does not match.Received no ACK-Bit, couldn't established connection!\n");
		}
	printf("Config write: %d ,config read: %d \n", config, buffer[1]);

	}

//   if (config != read_config)
//   {
//      // handle error
//      // ...
//      DS2482_reset();

//      return FALSE;
//   }

	// close connection and return
	close(deviceHandle);
	
   return TRUE;
}


int OWReset(void)
{
//--------------------------------------------------------------------------
// Reset all of the devices on the 1-Wire Net and return the result.
//
// Returns: TRUE(1):  presence pulse(s) detected, device(s) reset
//          FALSE(0): no presence pulses detected
//

   int poll_count = 0, POLL_LIMIT = 50;
   unsigned char buffer[2];
   int deviceHandle, readBytes;
   
	I2C_address_DS2482 = 27;

   // 1-Wire reset (Case B)
   //   S AD,0 [A] 1WRS [A] Sr AD,1 [A] [Status] A [Status] A\ P
   //                                   \--------/
   //                       Repeat until 1WB bit has changed to 0
   //  [] indicates from slave

//   I2C_start();
//   I2C_write(I2C_address | I2C_WRITE, EXPECT_ACK);
//   I2C_write(CMD_1WRS, EXPECT_ACK);
//   I2C_rep_start();
//   I2C_write(I2C_address | I2C_READ, EXPECT_ACK);

	// open device on /dev/i2c-1 note Rev.B board use addr 1
	if ((deviceHandle = open("/dev/i2c-1", O_RDWR)) < 0) {
		printf("Error32: Couldn't open device! %d\n", deviceHandle);
		return 1;
	}
	// connect to PCA9634 as i2c slave
	if (ioctl(deviceHandle, I2C_SLAVE, I2C_address_DS2482) < 0) {
		printf("Error33: Couldn't find device on address!\n");
		return 1;
	}  
   
   buffer[0] = 0xB4;	//B4 = One wire reset
   
	// begin transmission and request acknowledgement
	readBytes = write(deviceHandle,&buffer[0], 1);
	if (readBytes != 1)	{
		printf("Error34: write count does not match.Received no ACK-Bit, couldn't established connection!\n");
	}

   // loop checking 1WB bit for completion of 1-Wire operation
   // abort if poll limit reached
//   status = I2C_read(ACK);
	
	buffer[0] = 0xE1;	//E1 = set read pointer
	buffer[1] = 0xF0;	//F0 status register
	poll_count = 0;

	readBytes = write(deviceHandle,&buffer[0], 2);
	if (readBytes != 2)	{
		printf("Error35: write count does not match.Received no ACK-Bit, couldn't established connection!\n");
	}

	readBytes = read(deviceHandle, &buffer[0], 1);
		if (readBytes != 1){	
			printf("Error36: read count does not match.Received no ACK-Bit, couldn't established connection!\n");
		}

   do
   {
      //status = I2C_read(status & STATUS_1WB);
      readBytes = read(deviceHandle, &buffer[0], 1);
   }
   while ((buffer[0] & STATUS_1WB) && (poll_count++ < POLL_LIMIT));
	printf("Error38: INFO: 1W STATUS = %d\n", buffer[0]);
	printf("Error39: INFO: poll count = %d\n", poll_count);
 
	close(deviceHandle);    //I2C_stop();

   // check for failure due to poll limit reached
   if (poll_count >= POLL_LIMIT)
   {
      printf("Error37: poll count for 1W readiness reached the limit\n");
      DS2482_reset();
      return FALSE;
   }

   // check for short condition
   if (buffer[0]  & STATUS_SD)
      short_detected = TRUE;
   else
      short_detected = FALSE;

   // check for presence detect
   if (buffer[0]  & STATUS_PPD)
      return TRUE;
   else
      return FALSE;
}
 

*/
