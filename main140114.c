/*
 * main.c
 */

#include <stdio.h>
#include <fcntl.h>
#include <ncurses.h>
#include "PCAtest.h"
#include "DS2482.h"
#include <linux/i2c-dev.h>
#include "main.h"
#include "gpio.h"
#include "time.h"
#include <termios.h>
#include <unistd.h>

//#include "debug.h"

//#include <linux/i2c.h>
//#include <wiringPiI2C.h>

//#define DEBUG 0
//#define 

char InitDataSet[15]={0b10000000, //3MSB is autoinc address	, LSB is register start address
					0b00000000, //00h, MODE1 normal mode
					0b00010110, //01h, MODE2 reg
					0x7F, 		//02h, PWM0
					0x7F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //03h-09hPWM1-7
					0x00, //0Ah GRPPWM
					0x00, //0BhG RPFREQ
					0b00000000, //0Ch LEDOUT0, LED out 0 disabled (enabled 0x02)
					0b00000000 	//0Dh LEDOUT1 LED4-7 OFF
					};

struct SetONdata {
	int iOnTime;
	int iLatestTime;
	int i_Tperiode; };

int conversion(void);
int SetON( struct SetONdata *D);
int main (void)
{
	// print infos
	printf("Raspberry Pi PCA9634 test Sample\n");
	printf("=======================================\n");
  
	struct tm *timeinfo;
	time_t rawtime;
	char strResponse[128];

	char cKeyStroke, skipKey, cReadings[8];
	int tmpStat, i, iTemp=0, iTemp_1=0, iTargetTemp=0, iTargetTemp_1=0;
	int iTC=0, iTimeCount=0;
  
	// initialize buffer
	// address of PCA9634 device is 0x60, 0b01100000
	deviceParm PCA = {0x60, InitDataSet};

	//char ControlDataSet[15];
	
	//InitI2Cdevice(PCA);
	printf("i=init, a = increase, s=decrease, q=quit\n");
	printf("z=start, x=stop\n");
	printf("t=DS2482 device detect(Reset and Init, y=OWReset()\n");
	printf("m=write Start Conversion, n=readDate \n");
	printf("b=read temp, v=set target \n");
	printf("o=enable GPIO, p=diable GPIO \n");
	while(1) {
		
		if(iTimeCount++ == 1000 ){
			printf("tæller %d", iTC++);
			iTimeCount=0;
		}
		
		cKeyStroke = getchar();
		//printf("keys pressed %x\n", cKeyStroke );
		skipKey = getchar();
		switch (cKeyStroke) {
				case 'a': 
					incDutyCycle(PCA);
					break;
				case 's':
					decreDutyCycle(PCA);
					break;
				case 'z': 
					startPCA_PWM0(PCA);
					break;
				case 'x':
					stopPCA_PWM0(PCA);
					break;
				case 'q':
					return 0;
					break; 
				case 'i':
					InitI2Cdevice(PCA);
					break;
				case 't':
					DS2482_detect(27);
					//DS2482_reset();
					break;
				case 'y':
					tmpStat = OWReset(27);
					printf("PPD : %x\n", tmpStat );
					break;
				case 'm':
					if ( OWReset(27) ) {
						OWWriteByte( 0xCC );
						OWWriteByte( 0x44 );
					} else {
						printf("OWReset failed\n");
					}
					
					break;
				case 'n' :
					DS2482_detect(27);
					if (OWReset(27) ) {
						if (OWWriteByte( 0xCC )) printf("OWWriteByte 0xCC SUCCESS\n");
						else printf("OWWriteByte 0xCC FAILED\n");
						if (OWWriteByte( 0xBE )) printf("OWWriteByte 0xBE SUCCESS\n");
						else printf("OWWriteByte 0xBE FAILED\n");
//						OWWriteByte( 0xBE );
						for (i=0; i<8; i++) {
							cReadings[i] = OWReadByte();
						}
						for (i=0; i<8; i++) 
						{ printf("count %d reading: %xH\n", i, cReadings[i]); }
						iTemp = (cReadings[1]<<8) | cReadings[0];
						iTemp /= 16;
						printf("Temperature read is %d degree celcius", iTemp);
					} else {
						printf("OWReset failed\n");
					}
					break;
				case 'b' :
					if ( OWReset(27) ) {
						OWWriteByte( 0xCC );
						OWWriteByte( 0x44 ); //temo conversion started
						printf("Temperature reading started....\n");
						if ( OWReset(27) ) {
							OWWriteByte( 0xCC );
							OWWriteByte( 0xBE ); // reading scratch pad
							for (i=0; i<8; i++) {
								cReadings[i] = OWReadByte();
							}
							iTemp = (cReadings[1]<<8) | cReadings[0];
							iTemp /= 16;
							printf("read to %d degrees celcius\n", iTemp);
						}
					} else {
						printf("OWReset failed\n");
					}
					break;
				case 'o' : // enable GPIO
					if (SetGPIO(TRUE, 4))
						break;
					else
						printf("enable GPIO failed\n");
					break;
				case 'p' : // disable GPIO
					if (SetGPIO(FALSE, 4))
						break;
					else
						printf("disable GPIO failed\n");
					break;
				case 'v' : // enter temp target 
					conversion();

					rawtime = time (NULL);
					timeinfo = localtime(&rawtime);
					strftime(strResponse,128,"%H:%M:%S %d-%b-%Y",timeinfo);
					
					printf("%s\n", strResponse);
					
					break;
				default:
					printf("No funciton assigned to key\n");
					break;
				} //end switch-case loop
	} //end while(1) loop 
	return 0;
}

int conversion() {
	
	int iTargetTemp=0, iTargetTemp_1=0, i, iTemp2=0, iTemp2_1=0, iChar=0;
	int i_LastTimeSet=0, i_Tperiode=10, iTempReadInterval=10, iIdleTime=0;

	char cReading[8];
	long flg;
	struct termios term, oldterm;
	time_t t_tTimeStamp;

	struct SetONdata OnData;
	OnData.i_Tperiode = i_Tperiode;
	OnData.iOnTime = 0;
	
	printf("Enter target temp: \n");
	scanf("%d", &iTargetTemp);
	printf("target temp set to %d \n", iTargetTemp);
	
	if ((flg = fcntl(STDIN_FILENO, F_GETFL)) == (long)-1) {
		perror("fcntl(STDIN_FILENO, F_GETFL)");
		return -1;
	}
	flg |= O_NDELAY;
	if (fcntl(STDIN_FILENO, F_SETFL, flg) == -1) {
		perror("fcntl(STDIN_FILENO, F_SETFL)");
		return -1;
	}
	if (isatty(STDIN_FILENO)) {
		if (tcgetattr(STDIN_FILENO, &term)==-1) {
		  perror("tcgetattr(STDIN_FILENO, F_SETFL)");
		  return -1;
		}
    }
    oldterm=term;
    cfmakeraw(&term);
    if (tcsetattr(STDIN_FILENO, TCSANOW, &term)==-1) {
      perror("tcgetattr(STDIN_FILENO, F_SETFL)");
      return -1;
    }
	time(&t_tTimeStamp);
	OnData.iLatestTime = (int) t_tTimeStamp;
	printf("...timestamp %d\n\r", (int) t_tTimeStamp );
	
	while(iTargetTemp != 0) {
//		printf("...just enter while...\n");
//		iChar = getchar(); // dummy, just need to be there
		iChar = getchar();
/*		if( iChar != EOF ) {
			printf("Enter target temp: type 0 to exit conversion \n");
			scanf("%d", &iTargetTemp);
			printf("New target temp set to %d \n", iTargetTemp);
			//iTargetTemp =0;
		}
*/		
		if( iChar == 'q' ) {
//			printf("Enter target temp: type 0 to exit conversion \n");
//			scanf("%d", &iTargetTemp);
//			printf("New target temp set to %d \n", iTargetTemp);
			printf("...Target temp set to 0\n\r");
			iTargetTemp =0;
		}
		printf("\n...before check on timestamp %d, tim(0)=%d, periode %d\n\r", ((int) t_tTimeStamp % OnData.i_Tperiode), (int)time(0), OnData.i_Tperiode);
//		if( ((int) t_tTimeStamp % OnData.i_Tperiode) == 0 ){
//		if( ((int) t_tTimeStamp + iTempReadInterval) >= (int) time(0)   ){
			OnData.iLatestTime = (int) time(0);
			printf("...time since start: %d seconds\n\r", (OnData.iLatestTime-(int)t_tTimeStamp) );
		
			if ( OWReset(27) ) {  // read current temperature
				OWWriteByte( 0xCC );
				OWWriteByte( 0x44 ); //temo conversion started
				//printf("Temperature reading started....\n");
				if ( OWReset(27) ) {
					OWWriteByte( 0xCC );
					OWWriteByte( 0xBE ); // reading scratch pad
					for (i=0; i<8; i++) {
						cReading[i] = OWReadByte();
					}
					iTemp2 = (cReading[1]<<8) | cReading[0];
					iTemp2 /= 16;
					//printf("read to %d degrees celcius\r", iTemp2);
				}
			} // end read current temperature

			if( (iTemp2 != iTemp2_1) ) {
				printf("...start processomg\n\r");
				iTemp2_1 = iTemp2;
				printf(" \n...New temp %d and target %d \n\r", iTemp2, iTargetTemp );
				
				if( (iTemp2) < iTargetTemp ) {
					if( (iTargetTemp-iTemp2) <= 2) OnData.iOnTime=25; //SetON(25, i_LastTimeSet, i_Tperiode);
					else if ( (iTargetTemp-iTemp2) > 5) OnData.iOnTime=100; //SetON(100, i_LastTimeSet, i_Tperiode); 
					else OnData.iOnTime=50;  //SetON(50, i_LastTimeSet, i_Tperiode);	
				}
				else if( (iTemp2) > iTargetTemp ) {
					
					if( iTemp2-iTargetTemp > 3 ) OnData.iOnTime=0; //SetON(0, i_LastTimeSet, i_Tperiode);
					else OnData.iOnTime=10;  //SetON(10, i_LastTimeSet, i_Tperiode);
				}
				else { //temp = target
					OnData.iOnTime=30;  //SetON(30, i_LastTimeSet, i_Tperiode);
				}	 
			}			
//		} //end if( t_tTimeStamp % i_Tperiode )
		printf("...before SetON(), %d, %d, %d\n\r", OnData.iOnTime, OnData.iLatestTime, OnData.i_Tperiode);
		if ( !SetON(&OnData) )
			printf("SetOn failed");
	printf("...check time and usleep\n\r");
	
	iIdleTime = ((int) time(0))- OnData.iLatestTime;
	if ( iIdleTime == 0 )  {
		usleep( iTempReadInterval*1000000);
		iIdleTime = 10;
	}
	else if ( iIdleTime <= iTempReadInterval )
			usleep(iIdleTime*1000000 );
	else
		printf("...processing takes more than ", iTempReadInterval);
	printf("...just run usleep %d seconds \n\r", iIdleTime);


	} //while(iTargetTemp != 0)	

	if ((flg = fcntl(STDIN_FILENO, F_GETFL)) == (long)-1) {
		perror("fcntl(STDIN_FILENO, F_GETFL)");
		return -1;
	}
	flg &= !O_NDELAY;
	if (fcntl(STDIN_FILENO, F_SETFL, flg) == -1) {
		perror("fcntl(STDIN_FILENO, F_SETFL)");
		return -1;
	}
    tcsetattr(STDIN_FILENO, TCSANOW, &oldterm);
    printf("\n\r");	//not sure why but Paulsen did it in example
    
    printf("...ending\n\r");
		
}	
	
int SetON( struct SetONdata *D /*PercentageON, t_lastON, tPer*/ ) { // period = 2 minutes = 120seconds
	
	//int tPer = 60;
	
	//printf( "\nSetON: %d %d %d\n" , PercentageON, t_lastON, (int) time(0) );
	printf( "...SetON: %d %d %d\n\r" , D->iOnTime, D->iLatestTime, (int) time(0) );
	
//	if( ( ((int)time(0))-t_lastON ) < (tPer*PercentageON/100) )	
	if( ( ((int)time(0))-D->iLatestTime ) <= (D->i_Tperiode * D->iOnTime/100) )	{
		SetGPIO(TRUE, 4); //turn off SSR
		 printf("...SSR OFF\n\r");
	}
	else {
		SetGPIO(FALSE, 4); //turn on SSR
		 printf("...SSR ON\n\r");
	}
	return 1;

}	
/*truct SetONdata {
	int iOnTime;
	int iLatestTime;
	int i_Tperiode; }; */
