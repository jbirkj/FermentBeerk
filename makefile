#wiringPi documentation
#// Build command:  gcc IOToggle_Clib.c -o IOToggle_Clib 
#// -I/usr/local/include -L/usr/local/lib -lwiringPi

# C:\SysGCC\Raspberry\bin\arm-linux-gnueabihf-g++.exe -ggdb C:\Users\jbp.dk\projects\CrossComTest\CrossCOmTest.c -o CrossComTestOUTFILE

#This sample makefile has been setup for a project which contains the following files: main.h, ap-main.c, ap-main.h, ap-gen.c, ap-gen.h   Edit as necessary for your project
#Change output_file_name.a below to your desired executible filename

#Set all your object files (the object files of all the .c files in your project, e.g. main.o my_sub_functions.o )
#OBJ = ap-main.o ap-gen.o
OBJ = main.o DS2482.o PCAtest.o gpio.o logging.o conversion.o

#Set any dependant header files so that if they are edited they cause a complete re-compile (e.g. main.h some_subfunctions.h some_definitions_file.h ), or leave blank
#DEPS = main.h ap-main.h ap-gen.h
DEPS = main.h DS2482.h PCAtest.h gpio.h logging.h conversion.h

#Any special libraries you are using in your project (e.g. -lbcm2835 -lrt `pkg-config --libs gtk+-3.0` ), or leave blank
#LIBS = -lbcm2835 -lrt
#LIBS = -L/usr/local/lib -L/home/pi/wiringPi -lwiringPi -lpthread
LIBS = -L/usr/local/lib -lwiringPi -lpthread

#Set any compiler flags you want to use (e.g. -I/usr/include/somefolder `pkg-config --cflags gtk+-3.0` ), or leave blank
#CFLAGS =-c -Wall
CFLAGS =

#includes by JBirk
INCL =-I/usr/local/include -I/home/pi/wiringPi/wiringPi
#INCL =-I/usr/local/include 

#defines
#DEFS =-DDEBUG

#-I/usr/local/include -L/usr/local/lib -lwiringPi

#Set the compiler you are using ( gcc for C or g++ for C++ )
#CC = gcc
CC = C:\SysGCC\Raspberry\bin\arm-linux-gnueabihf-gcc

#Set the filename extensiton of your C files (e.g. .c or .cpp )
EXTENSION = .c

.phony: JBeerK_BrewControl_2_140905.a

#define a rule that applies to all files ending in the .o suffix, 
#which says that the .o file depends upon the .c version of the file 
#and all the .h files included in the DEPS macro.  Compile each object file
%.o: %$(EXTENSION) $(DEPS)
#	$(CC) -c -o $@ $< $(CFLAGS) $(DEFS)
	$(CC) -c -o $@ $< $(CFLAGS)

#Combine them into the output file
#Set your desired exe output file name here
#output_file_name.a: $(OBJ)
#	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

JBeerK_BrewControl_2_140905.a: $(OBJ)
#	$(CC) $(CFLAGS) -o $@ $^ $(INCL) $(LIBS) $(DEFS)
	$(CC) $(CFLAGS) -o $@ $^ $(INCL) $(LIBS)


#Cleanup
.PHONY: clean

clean:
	rm -f *.o *~ core *~ 

