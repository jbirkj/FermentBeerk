// GPIO.c file

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include "gpio.h"

//#include "debug.h"

// should have come from wiringPi.h - don't know why it cannot be found
#define	INPUT			 0
#define	OUTPUT			 1


int SetGPIO(iState, iWiPiPinNum) {

	int outputPin = iWiPiPinNum;
	
#ifdef DEBUG
	printf("Using GPIO pin %d as test output\n",outputPin);
#endif
	pinMode (outputPin, OUTPUT) ;
	digitalWrite (outputPin, iState) ;

	
	return 1 ;
}



// Build command:  gcc IOToggle_Clib.c -o IOToggle_Clib 
// -I/usr/local/include -L/usr/local/lib -lwiringPi
/*
#include <wiringPi.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int main (int argc, char **argv)
{
  int outputPin = 1;
  printf ("IO Toggle test - C code\n") ;

  if (wiringPiSetup () == -1)
    exit (1) ;

  printf("Using GPIO pin %d as test output\n", outputPin);

  pinMode (outputPin, OUTPUT) ;

  for (;;)
  {
    digitalWrite (outputPin, 1) ;
    digitalWrite (outputPin, 0) ;
  }

  return 0 ;
}
*/


