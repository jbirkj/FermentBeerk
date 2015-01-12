//conversion.c
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

struct SetONdata {
	int iOnTime;
	int iLatestTime;
	int i_Tperiode; };

int SetON( struct SetONdata *D);
//int ReadTempN( int i18B20ID );	// read temperature of the ID of 18B20
//int ReadTemp();					// read if only one 18B20 is mounted

extern char cROMCODE1[8],cROMCODE2[8], cROMCODE3[8], cROMCODE4[8], cROMCODE5[8];
extern char cWebLogGuid[40];

int FermentLog()
{

	int iChar=0, i=0, iTemperatureFerment=0, iTemperatureRoom=0, iPeriode=60, iLastT=0, iPresent=0;
	char cRead[8];
	
	iLastT = (int) time(0);

	printf("Enter log interval (seconds) \n");
	scanf("%d", &iPeriode);
	printf("log interval set to: %d \n", iPeriode);
//	fprintf( pFile, "First timestamp: %d\n", (int) time(0) ); 
	
	while( iPeriode != 0 ) {
		
		iChar = GetCh();
		
		if( iChar == 'q' ) {
			iPeriode =0;
		}
		
		iPresent = (int) time(0);
		if ( iLastT+iPeriode == iPresent ) {

			if ( OWReset(27) ) {
				OWWriteByte( 0xCC ); // CC is Skip ROM command
				OWWriteByte( 0x44 ); // temp conversion started for all OW devices
//				printf("Temperature reading started @ t=%d....\n\r", iPresentSecond );
				if ( OWReset(27) ) {		// ferment temp, metal tip with silicone
					OWWriteByte( 0x55 );// 55 address specific OW device
					for (i=0; i<8; i++) {
						OWWriteByte( cROMCODE5[i] );		//ROMCODE5 = metal tip w silicone
					}
					OWWriteByte( 0xBE ); // reading scratch pad
					for (i=0; i<8; i++) {
						cRead[i] = OWReadByte();
					}
					iTemperatureFerment = (cRead[1]<<8) | cRead[0];
					iTemperatureFerment /= 16;
				}
				if ( OWReset(27) ) {		// room temp, loose 18b20
					OWWriteByte( 0x55 );// 55 address specific OW device
					for (i=0; i<8; i++) {
						OWWriteByte( cROMCODE2[i] );		//ROMCODE5 = metal tip w silicone
					}
					OWWriteByte( 0xBE ); // reading scratch pad
					for (i=0; i<8; i++) {
						cRead[i] = OWReadByte();
					}
					iTemperatureRoom = (cRead[1]<<8) | cRead[0];
					iTemperatureRoom /= 16;
				}
			}

				WebLogWrite2( &cWebLogGuid[0], iTemperatureFerment, iTemperatureRoom, 0, 0 );
				iLastT = iPresent;
		}
	}
	return 1;
	
}

int conversion() {
	
	int iTargetTemp=0, iTargetTemp_1=0, i, iTemp2=0, iTemp2_1=0, iChar=0, iTemp=0, iTemp3=0, iTemp3_1=0, iTemp4=0;
	int i_LastSecond=0, i_Tperiode=10, iTempReadInterval=10, iIdleTime=0;
	int iPresentSecond = 0, iLastSecond = 0;;

	char cReading[8];
//	long flg;
//	struct termios term, oldterm;
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

	time(&t_tTimeStamp);
	OnData.iLatestTime = (int) t_tTimeStamp;
	printf("...timestamp %d\n\r", (int) t_tTimeStamp );
	
	while(iTargetTemp != 0) {
		iChar = GetCh();
		
		if( iChar == 'q' ) {
			iTargetTemp =0;
			printf("\n\r...Target temp set to %d\n\r", iTargetTemp);
			SetGPIO(TRUE, 4); //turn OFF SSR
			printf("\n\r...SSR OFF\n\r");
		}
		
		if( iChar == 'v' ) {
			printf("Enter target temp: \n");
			scanf("%d", &iTargetTemp);
			printf("target temp set to %d \n", iTargetTemp);			
			
		}
		
		//printf("\n...before check on timestamp %d, tim(0)=%d, periode %d\n\r", ((int) t_tTimeStamp % OnData.i_Tperiode), (int)time(0), OnData.i_Tperiode);

		iPresentSecond = (int) time(0);
		
		if( ( OnData.iLatestTime + iTempReadInterval) <= iPresentSecond ) {  // time to read temp and process
			
			if ( OWReset(27) ) {
				OWWriteByte( 0xCC ); // CC is Skip ROM command
				OWWriteByte( 0x44 ); // temp conversion started for all OW devices
//				printf("Temperature reading started @ t=%d....\n\r", iPresentSecond );
				if ( OWReset(27) ) {
					OWWriteByte( 0x55 );// 55 address specific OW device
					for (i=0; i<8; i++) {
						OWWriteByte( cROMCODE1[i] );		//ROMCODE1 = JohnGuestInsert
					}
					OWWriteByte( 0xBE ); // reading scratch pad
					for (i=0; i<8; i++) {
						cReading[i] = OWReadByte();
					}
					iTemp2 = (cReading[1]<<8) | cReading[0];
					iTemp2 /= 16;
//					printf("iTemp2 read to %d degrees celcius\r\n", iTemp2);
				}
				if ( OWReset(27) ) {
					OWWriteByte( 0x55 );// 55 address specific OW device
					for (i=0; i<8; i++) {
						OWWriteByte( cROMCODE5[i] );
					}
					OWWriteByte( 0xBE ); // reading scratch pad
					for (i=0; i<8; i++) {
						cReading[i] = OWReadByte();
					}
					iTemp3 = (cReading[1]<<8) | cReading[0];
					iTemp3 /= 16;
//					printf("iTemp3 read to %d degrees celcius\r\n", iTemp3);
				}						
				if ( OWReset(27) ) {
					OWWriteByte( 0x55 );// 55 address specific OW device
					for (i=0; i<8; i++) {
						OWWriteByte( cROMCODE2[i] );
					}
					OWWriteByte( 0xBE ); // reading scratch pad
					for (i=0; i<8; i++) {
						cReading[i] = OWReadByte();
					}
					iTemp4 = (cReading[1]<<8) | cReading[0];
					iTemp4 /= 16;
//					printf("iTemp4 read to %d degrees celcius\r\n", iTemp4);
				}						
			} else {
				printf("OWReset failed\n");
			}


			if( (iTemp2 != iTemp2_1) ) {
//				printf("...start processomg\n\r");
				iTemp2_1 = iTemp2;
				printf(" \n\r...New temp %d and target %d, Tcontrol: %d \n\r", iTemp2, iTargetTemp, iTemp3 );
//				printf("...time since start: %d seconds\n\r", (OnData.iLatestTime-(int)t_tTimeStamp) );
				fprintf( pFile, "New temp: %d with target: %d Tcontrol: %d at time %d seconds\n\r", iTemp2, iTargetTemp, iTemp3, iPresentSecond);	
				WebLogWrite2( &cWebLogGuid[0], iTemp2, iTemp3, iTemp4, iTargetTemp ); 

				// old was <5:100%, >5<2:50%, >2=0:30%  
				if( (iTemp2) < iTargetTemp ) {
					if( (iTargetTemp-iTemp2) <= 2) 
						{ OnData.iOnTime=30; } 
					else if ( (iTargetTemp-iTemp2) > 5 ) 
						{ OnData.iOnTime=100; }
					else if ( 2 < (iTargetTemp-iTemp2) <= 5) 
						{ OnData.iOnTime=80; }
					else 
						{ OnData.iOnTime=100; }
				}
				else if( (iTemp2) > iTargetTemp ) {
					
					if( iTemp2-iTargetTemp > 3 ) OnData.iOnTime=0; //SetON(0, i_LastTimeSet, i_Tperiode);
					else OnData.iOnTime=0;  //SetON(10, i_LastTimeSet, i_Tperiode);
				}
				else { //temp = target
					OnData.iOnTime=0;  //SetON(30, i_LastTimeSet, i_Tperiode);
				}	 
				printf( "...SetON: %d. Total time: %d seconds \n\r" , OnData.iOnTime, (OnData.iLatestTime-(int)t_tTimeStamp) );
				fprintf( pFile, "...SetON: %d. Total time: %d seconds \n\r" , OnData.iOnTime, (OnData.iLatestTime-(int)t_tTimeStamp) );
			}
			else if (iTemp3 != iTemp3_1) {
				iTemp3_1 = iTemp3;
				printf("Update from boil sensor");
				WebLogWrite2( &cWebLogGuid[0], iTemp2, iTemp3, iTemp4, iTargetTemp );
			}
			OnData.iLatestTime = iPresentSecond;
						
		} //end if( t_tTimeStamp % i_Tperiode )
		else if ( iLastSecond < iPresentSecond ) {

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
/*
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
*/    
    fclose( pFile );
    
    printf("...ending\n\r");
		
}	

int GetCh() {

	int ch;
	long flg;
	struct termios term, oldterm;
		
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

	ch = getchar();
	
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
//    printf("in GetCh\n\r");	//not sure why but Paulsen did it in example
    	
	return ch;
}


	
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
/*
int ReadTempN( char *c18B20ID ) {

	if ( OWReset(27) ) {
	OWWriteByte( 0xCC ); // CC is Skip ROM command
	OWWriteByte( 0x44 ); // temp conversion started for all OW devices
	printf("Temperature reading started....\n");
	if ( OWReset(27) ) {
		OWWriteByte( 0x55 );// 55 address specific OW device
		for (i=0; i<8; i++) {
			OWWriteByte( c18B20ID[i] );
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
			OWWriteByte( cROMCODE3[i] );
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
	
	
	
	}
*/
