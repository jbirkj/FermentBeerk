// PCAtest.h

typedef struct _deviceParm {
	int deviceI2CAddress;
	char *parms;
} deviceParm;

void Init(deviceParm);
int InitI2Cdevice(deviceParm);
int decreDutyCycle(deviceParm );
int startPCA_PWM0(deviceParm );
int stopPCA_PWM0(deviceParm );
int incDutyCycle(deviceParm );

/* initial for LED control
char InitDataSet[15]={0b10000000, //3MSB is autoinc address	, LSB is register start address
					0b00000000, //00h, MODE1 normal mode
					0b00000001, //01h, MODE2 reg
					0x7F, 		//02h, PWM0
					0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //03h-09hPWM1-7
					0x00, //0Ah GRPPWM
					0x00, //0BhGRPFREQ
					0b00000010, //0Ch LEDOUT0, LED out 0 enabled
					0b00000000 	//0Dh LEDOUT1 LED4-7 OFF
					};
*/
