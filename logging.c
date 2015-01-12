// Logging.c

#include "Logging.h"
#include <stdio.h>
#include <stdlib.h>



int WebLogWrite( char *cWLG, int t1, int t2, int tt ) {
	
	char syscmd2[512];
//	printf("curl http://www.birxlabs.com/Brew/Api/Data/AddObservation -G -d guid=%.36s -d t1=%d -d t2=%d -d t3=0 -d tt=%d -d msg=\n\r", cWLG, t1, t2, tt);
	sprintf(syscmd2,"curl http://www.birxlabs.com/Brew/Api/Data/AddObservation -G -d guid=%.36s -d t1=%d -d t2=%d -d t3=0 -d tt=%d -d msg=\n\r", cWLG, t1, t2, tt);
//http://www.birxlabs.com/Brew/Api/Data/AddObservation -G -d guid=%s -d t1=%d -d t2=%d -d -d tt=%d -d msg", cWLG, t1, t2, tt)
//http://www.birxlabs.com/brewApi/Data/AddObservation?guid=d9c56e03-43e8-4c46-b4b5-3524548397cc&t1=1.1&t2=2.1&t3=3.1&tt=4.0&msg=

	system(syscmd2);

	return 1;
	}

int WebLogWrite2( char *cWLG, int t1, int t2, int t3, int tt ) {
	
	char cSyscmd[512], cReturn[512];
	FILE *pfConsole;
	int res=0;
	
//	sprintf(cSyscmd, "curl http://www.birxlabs.com/Brew/Api/Data/AddObservation -G -d guid=%.36s -d t1=%d -d t2=%d -d t3=%d -d tt=%d -d msg=\n\r", cWLG, t1, t2, t3, tt);
	sprintf(cSyscmd, "curl http://54.214.87.135/Brew/Api/Data/AddObservation -G -d guid=%.36s -d t1=%d -d t2=%d -d t3=%d -d tt=%d -d msg=\n\r", cWLG, t1, t2, t3, tt);
//	sprintf(cSyscmd, "ps");
	printf( "%s \r\n",cSyscmd );
	
	pfConsole = popen( cSyscmd, "r" );
	
	if( pfConsole != NULL){
		res = fscanf( pfConsole, "%s", cReturn );
		if(res <= 0)
			printf("failed reading return string from popen \n");
		printf("Web log script returned: %s\r\n", cReturn );	 	
		pclose(pfConsole);
//		printf("pfConsole closed\r\n");
	} else {
		printf("failed reading from pipe popen()  \r\n");
	}
//	printf("curl http://www.birxlabs.com/Brew/Api/Data/AddObservation -G -d guid=%.36s -d t1=%d -d t2=%d -d t3=0 -d tt=%d -d msg=\n\r", cWLG, t1, t2, tt);
	//sprintf(syscmd2,"curl http://www.birxlabs.com/Brew/Api/Data/AddObservation -G -d guid=%.36s -d t1=%d -d t2=%d -d t3=0 -d tt=%d -d msg=\n\r", cWLG, t1, t2, tt);
//http://www.birxlabs.com/Brew/Api/Data/AddObservation -G -d guid=%s -d t1=%d -d t2=%d -d -d tt=%d -d msg", cWLG, t1, t2, tt)
//http://www.birxlabs.com/brewApi/Data/AddObservation?guid=d9c56e03-43e8-4c46-b4b5-3524548397cc&t1=1.1&t2=2.1&t3=3.1&tt=4.0&msg=

	return 1;
	}
