/*
 * main.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <ncurses.h>
#include "PCAtest.h"
#include "DS2482.h"
#include <linux/i2c-dev.h>
#include "main.h"
#include "gpio.h"
#include "time.h"
#include "conversion.h"
#include <termios.h>
#include <unistd.h>
#include <string.h>
//#include <wiringPi/wiringPi.h>

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
/*
struct SetONdata {
	int iOnTime;
	int iLatestTime;
	int i_Tperiode; };
*/
/*
int conversion(void);
int SetON( struct SetONdata *D);
*/

char cROMCODE1[8],cROMCODE2[8], cROMCODE3[8], cROMCODE4[8], cROMCODE5[8];
char cWebLogGuid[100];

int main (void)
{
	// print infos
	printf("Raspberry Pi PCA9634 test Sample\n");
	printf("=======================================\n");
  
	struct tm *timeinfo;
	time_t rawtime;
	char strResponse[128];

	char cKeyStroke, skipKey, cReadings[8], cDeviceROMaddr[8];
	int tmpStat, i, iTemp=0, iTemp_1=0, iTargetTemp=0, iTargetTemp_1=0;
	int iTC=0, iTimeCount=0;

	//1=build into metal, 2=2nd w short wiremount, 3=3rd w long wiremount, 4=loose 18B20 
	//5=metal tip w silicone tube
	cROMCODE1[0]=0x28;	cROMCODE2[0]=0x28;	cROMCODE3[0]=0x28;	cROMCODE4[0]=0x28;	cROMCODE5[0]=0x28;
	cROMCODE1[1]=0xc3;	cROMCODE2[1]=0x0c;	cROMCODE3[1]=0xa6;	cROMCODE4[1]=0x2f;	cROMCODE5[1]=0xe8;
	cROMCODE1[2]=0xc2;	cROMCODE2[2]=0x1b;	cROMCODE3[2]=0xd8;	cROMCODE4[2]=0x90;	cROMCODE5[2]=0xd4;
	cROMCODE1[3]=0x9d;	cROMCODE2[3]=0xe0;	cROMCODE3[3]=0x9c;	cROMCODE4[3]=0x2d;	cROMCODE5[3]=0x45;
	cROMCODE1[4]=0x04;	cROMCODE2[4]=0x04;	cROMCODE3[4]=0x04;	cROMCODE4[4]=0x04;	cROMCODE5[4]=0x05;
	cROMCODE1[5]=0x00;	cROMCODE2[5]=0x00;	cROMCODE3[5]=0x00;	cROMCODE4[5]=0x00;	cROMCODE5[5]=0x0;
	cROMCODE1[6]=0x00;	cROMCODE2[6]=0x00;	cROMCODE3[6]=0x00;	cROMCODE4[6]=0x00;	cROMCODE5[6]=0x0;
	cROMCODE1[7]=0x9b;	cROMCODE2[7]=0xb8;	cROMCODE3[7]=0xfd;	cROMCODE4[7]=0xd9;	cROMCODE5[7]=0x83;


	const char text[] = "GET http://www.google.com/intl/en/about.html HTTP/1.1\n"
	"Host: www.google.com\n"
	"Accept: text/html\n"
	"Accept: video/mpg\n"
	"Accept: image/jpg\n"
	"User-Agent: Mozilla/5.0\n";
	char url[512];

	char syscmd[512];
	char returnCurl[512];
	
	// initialize buffer
	// address of PCA9634 device is 0x60, 0b01100000
	deviceParm PCA = {0x60, InitDataSet};
	FILE *ptr_file;

//	setup_io();
	if (wiringPiSetup() == -1) {
		return 0;
	}


	//char ControlDataSet[15];
	
	//InitI2Cdevice(PCA);
	printf("i=init, a = increase, s=decrease, q=quit\n");
	printf("z=start, x=stop\n");
	printf("t=DS2482 device detect(Reset and Init, y=OWReset()\n");
	printf("m=read temp from one 18b20, n=Read ROM code \n");
	printf("b=read temp from more 18b20s, v=set target \n");
	printf("o=enable GPIO, p=diable GPIO \n");
	printf("w=read multiple sensors, e=sent to web and read return (use popen) \n");
	
	
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
				case 'b' : // case to read temperature from multiple sensors
					if ( OWReset(27) ) {
						OWWriteByte( 0xCC ); // CC is Skip ROM command
						OWWriteByte( 0x44 ); // temp conversion started for all OW devices
						printf("Temperature reading started....\n");
						if ( OWReset(27) ) {
							OWWriteByte( 0x55 );// 55 address specific OW device
							for (i=0; i<8; i++) {
								OWWriteByte( cROMCODE1[i] );
							}
							OWWriteByte( 0xBE ); // reading scratch pad
							for (i=0; i<8; i++) {
								cReadings[i] = OWReadByte();
							}
							iTemp = (cReadings[1]<<8) | cReadings[0];
							iTemp /= 16;
							printf("read to %d degrees celcius\n", iTemp);
						}
						if ( OWReset(27) ) {
							OWWriteByte( 0x55 );// 55 address specific OW device
							for (i=0; i<8; i++) {
								OWWriteByte( cROMCODE2[i] );
							}
							OWWriteByte( 0xBE ); // reading scratch pad
							for (i=0; i<8; i++) {
								cReadings[i] = OWReadByte();
							}
							iTemp = (cReadings[1]<<8) | cReadings[0];
							iTemp /= 16;
							printf("read to %d degrees celcius\n", iTemp);
						}	
						if ( OWReset(27) ) {
							OWWriteByte( 0x55 );// 55 address specific OW device
							for (i=0; i<8; i++) {
								OWWriteByte( cROMCODE5[i] );
							}
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
				case 'e': 
					printf("Call G before this (e) to get GUID from file\n");
					WebLogWrite2( cWebLogGuid, 11,22,33 );
					break;
				case 'f': 
					printf("Ferment Log\n");

					ptr_file =fopen("GUID.txt","rt");
					if (!ptr_file)
						return 1;
					fgets( &cWebLogGuid[0], 100, ptr_file);
					printf("GUID read is: %s \n\r",cWebLogGuid);
					fclose(ptr_file);

					FermentLog();
					//WebLogWrite2( cWebLogGuid, 11,22,33 );
					break;

				case 'g': 		// get GUID for WebLog
					printf("Reading GUID for Brew@Birxlabs in guid.txt\n");
					
//					FILE *ptr_file;

					ptr_file =fopen("GUID.txt","rt");
					if (!ptr_file)
						return 1;

					fgets( &cWebLogGuid[0], 100, ptr_file);
					printf("GUID read is: %s \n\r",cWebLogGuid);

					fclose(ptr_file);
					
					break;
				case 'i':
					InitI2Cdevice(PCA);
					break;
				case 'm': // read temp from one sensors mounted
					if ( OWReset(27) ) {
						OWWriteByte( 0xCC ); // CC is Skip ROM command
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
				case 'n' : // get serial when only one 1w device is present
					if ( OWReset(27) ) {
						OWWriteByte( 0x33 );
						for (i=0; i<8; i++) {
							cDeviceROMaddr[i] = OWReadByte();
							printf("Byte %d: %x \n", i, cDeviceROMaddr[i]);
						}
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
				case 'q':
					return 0;
					break; 
				case 's':
					decreDutyCycle(PCA);
					break;
				case 't':
					DS2482_detect(27);
					//DS2482_reset();
					break;
				case 'v' : // enter temp target 
					printf("Reading GUID for Brew@Birxlabs in guid.txt\n");
					
//					FILE *ptr_file;

					ptr_file =fopen("GUID.txt","rt");
					if (!ptr_file)
						return 1;

					fgets( &cWebLogGuid[0], 100, ptr_file);
					printf("GUID read is: %s \n\r",cWebLogGuid);

					fclose(ptr_file);

					conversion();

					rawtime = time (NULL);
					timeinfo = localtime(&rawtime);
					strftime(strResponse,128,"%H:%M:%S %d-%b-%Y",timeinfo);
					
					printf("%s\n", strResponse);
					
					break;
				case 'w': 
				/*
					puts(text);
					if ( sscanf(text, "GET http://%511s", url) == 1 )
						{
						printf("url = \"%s\"\n", url);
					}
				*/
					// Build into curl command
					//sprintf(syscmd,"curl \"http://coredogs.com/content_media/lessons/servercore/fundamental-forms/process-simple-form-get.php?first_name=buffi&surname=summersdf\"");
					//sprintf(syscmd,"curl http://www.birxlabs.com/Brew/Api/Data/AddObservation -G -d guid=42b312c8-c2d0-4640-a617-513382083c4f -d t1=11 -d t2=22 -d t3=33 -d tt=44 -d msg");
					sprintf(syscmd,"curl http://www.birxlabs.com/brew/Api/Data/AddObservation -G  -d guid=1b04d06d-a1b4-4aff-8ff0-4aee63e06bed -d t1=1.1 -d t2=2.1 -d t3=3.1 -d tt=4.0 -d msg");
					// Execute--this will wait for command to complete before continuing. 
					system(syscmd);
					
					break;				case 'x':
					stopPCA_PWM0(PCA);
					break;
				case 'y':
					tmpStat = OWReset(27);
					printf("PPD : %x\n", tmpStat );
					break;
				case 'z': 
					startPCA_PWM0(PCA);
					break;
				default:
					printf("No funciton assigned to key\n");
					break;
				} //end switch-case loop
	} //end while(1) loop 
	return 0;
}

/*
int conversion() {
	
	int iTargetTemp=0, iTargetTemp_1=0, i, iTemp2=0, iTemp2_1=0, iChar=0;
	int i_LastSecond=0, i_Tperiode=10, iTempReadInterval=10, iIdleTime=0;
	int iPresentSecond = 0, iLastSecond = 0;;

	char cReading[8];
	long flg;
	struct termios term, oldterm;
	time_t t_tTimeStamp;

	struct SetONdata OnData;
	OnData.i_Tperiode = i_Tperiode;
	OnData.iOnTime = 0;
	
	FILE *pFile;
	char cFilename[] = "templog", cFilenameAdd[10];
	
	sprintf( cFilenameAdd, "%d", (int) time(0) );
	strcat( cFilename, cFilenameAdd );
	strcat( cFilename, ".txt" );
	pFile = fopen( cFilename, "w+");
	
	printf("Enter target temp: \n");
	scanf("%d", &iTargetTemp);
	printf("target temp set to %d \n", iTargetTemp);
	fprintf( pFile, "First timestamp: %d\n", (int) time(0) ); 
	
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
		iChar = getchar();
		
		if( iChar == 'q' ) {
			iTargetTemp =0;
			printf("\n\r...Target temp set to %d\n\r", iTargetTemp);
			SetGPIO(TRUE, 4); //turn OFF SSR
			printf("\n\r...SSR OFF\n\r");
		}
		//printf("\n...before check on timestamp %d, tim(0)=%d, periode %d\n\r", ((int) t_tTimeStamp % OnData.i_Tperiode), (int)time(0), OnData.i_Tperiode);

		iPresentSecond = (int) time(0);
		
		if( ( OnData.iLatestTime + iTempReadInterval) <= iPresentSecond ) {  // time to read temp and process
			
	
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
//				printf("...start processomg\n\r");
				iTemp2_1 = iTemp2;
				printf(" \n...New temp %d and target %d \n\r", iTemp2, iTargetTemp );
//				printf("...time since start: %d seconds\n\r", (OnData.iLatestTime-(int)t_tTimeStamp) );
				fprintf( pFile, "New temp: %d with target: %d at time %d seconds\n\r", iTemp2, iTargetTemp, iPresentSecond);	
				WebLogWrite(iTemp2, 00, iTargetTemp ); 

				
				if( (iTemp2) < iTargetTemp ) {
					if( (iTargetTemp-iTemp2) <= 2) OnData.iOnTime=30; //SetON(25, i_LastTimeSet, i_Tperiode);
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
			printf( "...SetON: %d. Total time: %d seconds \n\r" , OnData.iOnTime, (OnData.iLatestTime-(int)t_tTimeStamp) );
			fprintf( pFile, "...SetON: %d. Total time: %d seconds \n\r" , OnData.iOnTime, (OnData.iLatestTime-(int)t_tTimeStamp) );
			}
			OnData.iLatestTime = iPresentSecond;
						
		} //end if( t_tTimeStamp % i_Tperiode )
		else if ( iLastSecond < iPresentSecond ) {

//			printf("...before SetON(), %d, %d, %d\n\r", OnData.iOnTime, OnData.iLatestTime, OnData.i_Tperiode);
//			if ( !SetON(&OnData) )
//				printf("SetOn failed");
//			printf( "...SetON: %d %d %d\n\r" , OnData.iOnTime, OnData.iLatestTime, (int) time(0) );
	
			if( (iPresentSecond - OnData.iLatestTime)*10 < OnData.iOnTime ) {
				SetGPIO(FALSE, 4); //turn ON SSR
				 //printf("...SSR ON\n\r");
			}
			else {
				SetGPIO(TRUE, 4); //turn OFF SSR
				 //printf("...SSR OFF\n\r");
			}
			iLastSecond = iPresentSecond;
		}
		
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
    
    fclose( pFile );
    
    printf("...ending\n\r");
		
}	
*/


/*	
int SetON( struct SetONdata *D ) { // period = 2 minutes = 120seconds
	
	printf( "...SetON: %d %d %d\n\r" , D->iOnTime, D->iLatestTime, (int) time(0) );
	
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
*/
	
/*truct SetONdata {
	int iOnTime;
	int iLatestTime;
	int i_Tperiode; }; */
