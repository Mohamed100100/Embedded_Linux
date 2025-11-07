
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

// Define a unique key to identify the shared memory segment
#define SHM_KEY                 (100100)

// Define the size of the shared memory segment (1 KB here)
#define SHARED_MEMORY_SIZE      (1024)


int main() {

    // Pointer that will point to the shared memory once attached
    char *buf = NULL;

    // Create (or access existing) shared memory segment
    int shmid = shmget(SHM_KEY, SHARED_MEMORY_SIZE, IPC_CREAT | 0666);
    if (shmid == -1) {
        write(2,"Error with shmget function\n",sizeof("Error with shmget function\n"));
    }else{

        // Attach the shared memory to the process’s address space
        buf = (char *)shmat(shmid, NULL, 0);
        if (buf == (void *)-1) {
           write(2,"Error with shmat function\n",sizeof("Error with shmat function\n"));
        }else{
                            
           printf("[Reciever] Shared memory attached at address: %p\n", buf);
           while (1) {

	      // buf[0] acts as a flag:
              // 0 >> no new message
              // 1 >> new message available
              if(1 == buf[0]){          

		 // Write the message (starting from buf+1) to stdout
                 write(1,(buf+1),SHARED_MEMORY_SIZE-1);

		 // Clear the message area (reset memory to '\0')
		 memset(buf+1,'\0',SHARED_MEMORY_SIZE-1);

		 // Reset the flag to 0 (message consumed)
                 buf[0] = 0;
	      }else{
	         // donothing
	      }
           }

        }
    }
    shmdt(buf);
    return 0;
}

