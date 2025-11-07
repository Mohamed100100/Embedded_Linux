
#include <stdio.h>

#include "myWrite.h"

int main(){

   ssize_t size = myPrintf("hello world \n");

   printf("size = %li\n",size);

}

