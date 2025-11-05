#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>


#include "./LIB/errtype.h"
#include "./LIB/stdtypes.h"

#define CAPSLOCK_PATH	("/sys/class/leds/input4::capslock/brightness")

void printUsageMessage();
ErrType_t CapslockControl(uint8 *CapsState);

int main(int argc,char * argv[]){

 
   switch (argc){
   // in case of one only argument :(
   case 1  : 
	   printUsageMessage(); 
	   break; 
   // in case of 2 arguments :)	   
   case 2  : 
   	ErrType_t status = CapslockControl(argv[1]);
   	if(E_OK != status){
      	   printUsageMessage();		   
   	}else{
      	   printf("it works successfully.\n"); 
      	   printf("Look at the CapsLock Led :)\n"); 
   	}
   	break;
   // more than two arguments 
   default : 
	printf("Too Much arguments !!\n");
	printUsageMessage();
	break;
   }
}

void printUsageMessage(){
   printf("Usage:\n");
   printf("  sudo CapslockControl.exe 1   -> Turn ON Caps Lock LED\n");
   printf("  sudo CapslockControl.exe 0   -> Turn OFF Caps Lock LED\n");
}

ErrType_t CapslockControl(uint8 *CapsState){

   ErrType_t status = E_NOT_OK;

   // check on null pointer
   if(NULL == CapsState){
      status = E_NULL_PTR;

   // incase of argumrnt is correct
   }else{
      // check if the string is either 0 or 1
      if((0 == strcmp(CapsState,"0"))||0 == strcmp(CapsState,"1")){

	 // if yes start to open the file 
         uint32 CapsLockFd = open(CAPSLOCK_PATH,O_RDWR);

	 // check on open successully
	 if(-1 != CapsLockFd){
	    write(CapsLockFd, CapsState , 1);
   	    status = E_OK;
	 // if not open successfully
	 }else{
	    printf("Erro while opening the file : %s\n",strerror(errno));
	    status = E_UNKNOWN_ERROR;
	 }
      // invalid argument
      }else{
	 printf("Error, invalid arguments !\n");
         status = E_INVALID_PARAM;
      }
   }

   return (status);
}
