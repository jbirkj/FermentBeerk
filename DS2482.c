/*
 * untitled.c
 * 1-wire speed (c1WS = standard 0)
 
 * 
 */
#include <stdio.h>
#include <fcntl.h>
#include <ncurses.h>
#include <linux/i2c-dev.h>
#include "DS2482.h"
//#include "debug.h"

//int DS2482_reset();
//int DS2482_detect(unsigned char addr);

// DS2482 state

unsigned char I2C_address_DS2482;
unsigned char c1WS, cSPU, cPPM, cAPU;
int short_detected, PPD;

int STATUS_1WB = 0b00000001;
int STATUS_PPD = 0b00000010;
int STATUS_SD  = 0b00000100;
int STATUS_RST = 0b00010000;

unsigned char ucSTATUS_1WB = 0x01;


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

	unsigned char tmp;
   // set global address
   I2C_address_DS2482 = addr;

#ifdef DEBUG
   // reset the DS2482 ON selected address
   if ( DS2482_reset(I2C_address_DS2482) )
		printf("DS2482 reset SUCCESS\n");
   else
		printf("DS2482 reset FAILED\n");
#endif
   // default configuration
   c1WS = 0b10000000;		// 1WS is bit 3 (with bit 7 inverted)
   cSPU = 0b01000000;		// SPU is bit 2 (with bit 6 inverted)
   cPPM = 0b00100000;		// bit =0 (with bit 5 = 1)
   cAPU = 0b00000001;		// APU is bit 0 (with bit 4 inverted)
   tmp  = 0b11100001;

	//tmp = c1WS | cSPU | cPPM | cAPU;
   // write the default configuration setup
   if (!DS2482_write_config( tmp ) ) {
	   printf("DS2482 write config failed!\n", tmp);
      return FALSE;
  }
   return TRUE;
}


int DS2482_reset(unsigned char ucI2CAddr)
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
	if (ioctl(deviceHandle, I2C_SLAVE, ucI2CAddr) < 0) {
		printf("Error: Couldn't find device on address!\n");
		return 1;
	}  


//   I2C_write(I2C_address | I2C_WRITE, EXPECT_ACK);
//   I2C_write(CMD_DRST, EXPECT_ACK);
//   I2C_rep_start();
//   I2C_write(I2C_address | I2C_READ, EXPECT_ACK);
//   status = I2C_read(NACK);
//   I2C_stop();


   
	buffer[0] = 0xF0;		// F0h = Device Reset
							// F0 set read pointer to Status Reg for verification
	
	// begin transmission and request acknowledgement
	readBytes = write(deviceHandle,&buffer[0], 1);
	if (readBytes != 1)	{
		printf("Error: write count does not match.Received no ACK-Bit, couldn't established connection!\n");
	}
	else {
#ifdef DEBUG
		printf("DS2482 Device Reset set\n");
#endif
		readBytes = read(deviceHandle, &buffer[1], 1);
		if (readBytes != 1){	
			printf("Error: read count does not match.Received no ACK-Bit, couldn't established connection!\n");
		}
#ifdef DEBUG
	printf("Status Reg read: %xH \n", buffer[1]);
#endif

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
	//buffer[1]= config | (~config << 4);
	buffer[1]= config;
	
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
	//write config set read pointer to config reg as prep for data validation

	
   // check for failure due to incorrect read back of status

	readBytes = read(deviceHandle, &buffer[1], 1);
	if (readBytes != 1){	
		printf("Error21: read count does not match.Received no ACK-Bit, couldn't established connection!\n");
	}
#ifdef DEBUG
	printf("Config write: %xH ,config read: %xH \n", config, buffer[1]);
#endif
	// close connection and return
	close(deviceHandle);

	if ( (config & 0x0F) == buffer[1] )
		return TRUE;
	else
		return FALSE;
	
   //return TRUE;
}


int OWReset(unsigned char addrI2C)
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
   
	I2C_address_DS2482 = addrI2C;

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
		return -1;
	}
	// connect to PCA9634 as i2c slave
	if (ioctl(deviceHandle, I2C_SLAVE, I2C_address_DS2482) < 0) {
		printf("Error33: Couldn't find device on address!\n");
		return -1;
	}  
   
	readBytes = read(deviceHandle, buffer, 1);
#ifdef DEBUG
	printf("Error35a: write buffer[0]= %xH\n", buffer[0]);
#endif
	buffer[0] = 0xb4;	//B4 = One wire reset
   
	// begin transmission and request acknowledgement
	readBytes = write(deviceHandle,buffer, 1);
	if (readBytes != 1)	{
		printf("Error34: write count does not match.Received no ACK-Bit, couldn't established connection!\n");
	}

   // loop checking 1WB bit for completion of 1-Wire operation
   // abort if poll limit reached
//   status = I2C_read(ACK);

#ifdef DEBUG
// note: read pointer automatically set to status reg past reset	
	printf("Error35a: write buffer[0]= %xH\n", buffer[0]);
#endif
	readBytes = read(deviceHandle, &buffer[0], 1);
#ifdef DEBUG
	printf("Error35b: INFO: read again buffer[0]= %xH\n", buffer[0]);
#endif
   do
   {
      //status = I2C_read(status & STATUS_1WB);
      readBytes = read(deviceHandle, &buffer[0], 1);
#ifdef DEBUG
      printf("Error35: INFO: in DO loop buffer[0]= %xH\n", buffer[0]);
#endif
		if (readBytes != 1){	
			printf("Error36: read count does not match.Received no ACK-Bit, couldn't established connection!\n");
		}
   }
   while ((buffer[0] & STATUS_1WB) && (poll_count++ < POLL_LIMIT));
   
#ifdef DEBUG
	printf("Error38: INFO: 1W STATUS = %d\n", buffer[0]);
	printf("Error39: INFO: poll count = %d\n", poll_count);
#endif
 
	close(deviceHandle);    //I2C_stop();

   // check for failure due to poll limit reached
   if (poll_count >= POLL_LIMIT)
   {
      printf("Error37: poll count for 1W readiness reached the limit\n");
      DS2482_reset(addrI2C);
      return FALSE;
   }

   // check for short condition
   if (buffer[0]  & STATUS_SD) {
      short_detected = TRUE;
#ifdef DEBUG
      printf("Error37a: short detected past 1W Reset\n");
#endif
   } else
      short_detected = FALSE;

   // check for presence detect
   if (buffer[0]  & STATUS_PPD) {
      PPD = TRUE;
#ifdef DEBUG
      printf("Error37b: Presence Puls Detected past 1W Reset\n");
#endif
   } else
      PPD = FALSE;
      
   return PPD;
}
 
int OWWriteByte( char cWriteByte ) {
// Send 8 bits of communication to the 1-Wire Net and verify that the
// 8 bits read from the 1-Wire Net are the same (write operation).
// The parameter 'sendbyte' least significant 8 bits are used.
//
// 'sendbyte' - 8 bits to send (least significant byte)
//
// Returns:  TRUE: bytes written and echo was the same
//           FALSE: echo was not the same
//
//Approach
// 1. check if idle (1WB=0)
// 2. Start to write byte (cWriteByte)
// 3. check for idle before returning (1WB=0)
//
   unsigned char status;
   int poll_count = 0, POLL_LIMIT=100;
   I2C_address_DS2482 = 27;
   unsigned char buffer[2];
   int deviceHandle, readBytes;
   
   // 1-Wire Write Byte (Case B)
   //   S AD,0 [A] 1WWB [A] DD [A] Sr AD,1 [A] [Status] A [Status] A\ P
   //                                          \--------/
   //                             Repeat until 1WB bit has changed to 0
   //  [] indicates from slave
   //  DD data to write

//   I2C_start();
//   I2C_write(I2C_address | I2C_WRITE, EXPECT_ACK);
//   I2C_write(CMD_1WWB, EXPECT_ACK);
//   I2C_write(sendbyte, EXPECT_ACK);
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
   
	buffer[0] = 0xE1;	//E1 = set read pointer
	buffer[1] = 0xF0;	//F0 = Status register
   
	// begin transmission and request acknowledgement
	readBytes = write(deviceHandle,&buffer[0], 2);
	if (readBytes != 2)	{
		printf("Error40: write count does not match.Received no ACK-Bit, couldn't established connection!\n");
	}
	// no busy polling needed. Set Read Pointer point right immediately.
	
	//Now check if 1WB=0 to make sure 1WWrite command can be executed.
	poll_count = 0;
	readBytes = read(deviceHandle, &buffer[0], 1);
	do
	{ //status = I2C_read(status & STATUS_1WB);
	  readBytes = read(deviceHandle, &buffer[0], 1);
#ifdef DEBUG
	  printf("Error35: INFO: in DO loop count %d, buffer[0]= %xH\n",poll_count, buffer[0]);
#endif
	  if (readBytes != 1){	
		printf("Error36: read count does not match.Received no ACK-Bit, couldn't established connection!\n");
		return 0;
	  }
	}
	while ((buffer[0] & STATUS_1WB) && (poll_count++ < POLL_LIMIT));

/*	while (TRUE){
		poll_count++;

		readBytes = read(deviceHandle, &buffer[0], 1);
			if (readBytes != 1){	
				printf("Error41: read count does not match.Received no ACK-Bit, couldn't established connection!\n");
			}

		if (buffer[0] && 0x01) {   //1W busy bit
			if (poll_count>100) {
				printf("Error42: 1WWB count too long FAIL\n");
				return -1;		// too wait too long
			}
		} else {
				break;	// bus is idle
			}
		}
*/		
	buffer[0] = 0xA5;	//A5 = 1Wire Write Byte
	buffer[1] = cWriteByte;	//byte data to write 
   
	// begin transmission and request acknowledgement
	readBytes = write(deviceHandle,&buffer[0], 2);
	if (readBytes != 2)	{
		printf("Error45: write count does not match.!\n");
		return -1;
	}

	// for 1WWritebyte, busy polling is needed
	// 1WW set read pointer to status reg for busy polling
	poll_count = 0;
	readBytes = read(deviceHandle, &buffer[0], 1);
	do
	{ //status = I2C_read(status & STATUS_1WB);
	  readBytes = read(deviceHandle, &buffer[0], 1);
#ifdef DEBUG
	  printf("Error35A: INFO: in DO loop count %d,  buffer[0]= %xH\n",poll_count, buffer[0]);
#endif
	  if (readBytes != 1){	
		printf("Error35AA: read count does not match.Received no ACK-Bit, couldn't established connection!\n");
		return -1;
	  }
	  if (poll_count == POLL_LIMIT ) {
		printf("Error35BB: poll limit reached !\n");
		return -1;
	  }
	}
	while ((buffer[0] & STATUS_1WB) && (poll_count++ < POLL_LIMIT));


/*	while (TRUE){	// check for write finished / idle
		poll_count++;

		readBytes = read(deviceHandle, &buffer[0], 1);
			if (readBytes != 1){	
				printf("Error46: read count does not match.Received no ACK-Bit, couldn't established connection!\n");
			}

		if (buffer[0] && 0x01) {   //1W busy bit
			if (poll_count>100) {
				printf("Error47: 1WWB count too long FAIL\n");
				return -1;		// too wait too long
			}
		} else {
				break;	// bus is idle
			}
		}
*/
	close(deviceHandle);    //I2C_stop();
	return 1; // write successfull
	
}

int OWReadByte() {
// 1. check for idle (1WB=0)
// 2. set read pointer to Read Data Register
// 3. do 2482 read function

   unsigned char status;
   int poll_count = 0, POLL_LIMIT = 100;
   I2C_address_DS2482 = 27;
   unsigned char buffer[3];
   int deviceHandle, readBytes;
   
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
 /*  
	buffer[0] = 0xE1;	//E1 = set read pointer
	buffer[1] = 0xF0;	//F0 = Status register
   
	// begin transmission and request acknowledgement
	readBytes = write(deviceHandle,&buffer[0], 2);
	if (readBytes != 2)	{
		printf("Error50: !\n");
	}
	// no busy polling needed. Set Read Pointer point right immediately.
	// any now check if 1WB=0 before executing read command
	do
	{ //status = I2C_read(status & STATUS_1WB);
	  readBytes = read(deviceHandle, &buffer[0], 1);
	  printf("Error51: INFO: in DO loop buffer[0]= %d\n", buffer[0]);
	  if (readBytes != 1){	
		printf("Error52: read count does not match.Received no ACK-Bit, couldn't established connection!\n");
		return 0;
	  }
	}
	while ((buffer[0] & STATUS_1WB) && (poll_count++ < POLL_LIMIT));

	if (poll_count == POLL_LIMIT ) {
		printf("Error53: poll limit reached !\n");
		return -1;
	}
*/

/*	while (TRUE){
		poll_count++;

		readBytes = read(deviceHandle, &buffer[0], 1);
			if (readBytes != 1){	
				printf("Error51: !\n");
			}

		if (buffer[0] && 0x01) {   //1W busy bit
			if (poll_count>100) {
				printf("Error52: 1WWB count too long FAIL\n");
				return -1;		// too wait too long
			}
		} else {
				break;	// bus is idle
			}
		}
*/
	
	// Set DS2482 1W read mode
	buffer[0] = 0x96;	//96 = set 1W read byte
   
	// begin transmission and request acknowledgement
	readBytes = write(deviceHandle,&buffer[0], 1);
	if (readBytes != 1)	{
		printf("Error53: !\n");
	}
	while (TRUE){
		poll_count++;

		readBytes = read(deviceHandle, &buffer[0], 1);
			if (readBytes != 1){	
				printf("Error54: !\n");
			}

		if (buffer[0] & 0x01) {   //1W busy bit
			if (poll_count>100) {
				printf("Error55: 1WWB count too long FAIL\n");
				return -1;		// too wait too long
			}
		} else {
				break;	// bus is idle
			}
		}	

	// read data
	buffer[0] = 0xE1;	//E1 = set read pointer
	buffer[1] = 0xE1;	//E1 = read data register

	// begin transmission and request acknowledgement
	readBytes = write(deviceHandle,&buffer[0], 2);
	if (readBytes != 2)	{
		printf("Error56: !\n");
	}
	//no polling needed for Set Read Pointer
	// Now read actual data
	readBytes = read(deviceHandle,&buffer[0], 1);
	if (readBytes != 1)	{
		printf("Error57: write count does not match.Received no ACK-Bit, couldn't established connection!\n");
	}

	// set pointer to status reg for busy polling
	buffer[1] = 0xE1;	//E1 = set read pointer
	buffer[2] = 0xF0;	//F0 = Status register
	// begin transmission and request acknowledgement
	readBytes = write(deviceHandle,&buffer[1], 2);
	if (readBytes != 2)	{
		printf("Error58: write count does not match.Received no ACK-Bit, couldn't established connection!\n");
	}
	
	poll_count = 0;
	readBytes = read(deviceHandle, &buffer[1], 1);
	do
	{ //status = I2C_read(status & STATUS_1WB);
	  readBytes = read(deviceHandle, &buffer[1], 1);
#ifdef DEBUG
	  printf("Error59: INFO: in DO loop count %d, buffer[0]= %xH\n",poll_count, buffer[1]);
#endif
	  if (readBytes != 1){	
		printf("Error59a: read count does not match.Received no ACK-Bit, couldn't established connection!\n");
		return 0;
	  }
	}
	while ((buffer[1] & STATUS_1WB) && (poll_count++ < POLL_LIMIT));

	close(deviceHandle);    //I2C_stop();
	
	return buffer[0]; // return data as has been read


	
}
