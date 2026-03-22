
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

// Define a unique key for the shared memory segment.
#define SHM_KEY 		(100100)
// Define the size of the shared memory block in bytes.
#define SHARED_MEMORY_SIZE 	(1024)



int main() {

    // Pointer that will point to the attached shared memory region
    char *buf = NULL;

    //Step 1: Create (or get) a shared memory segment
    // shmget() creates a shared memory segment identified by SHM_KEY.
    // If it already exists, it returns the same segment ID.
    // Permissions: 0666 (read & write for all users)
    int shmid = shmget(SHM_KEY, SHARED_MEMORY_SIZE, IPC_CREAT | 0666);
    if (shmid == -1) {
        write(2,"Error with shmget function\n",sizeof("Error with shmget function\n"));
    }else{
    
        // Step 2: Attach the shared memory segment to the process’s address space
        // shmat() returns a pointer to the start of the shared memory.
        buf = (char *)shmat(shmid, NULL, 0);
        if (buf == (void *)-1) {
           write(2,"Error with shmat function\n",sizeof("Error with shmat function\n"));
        }else{
 
    	   printf("[Sender] Shared memory attached at address: %p\n", buf);
     	   while (1) {

	      // Read input from standard input (keyboard)
              // and store it in shared memory starting at (buf + 1)
              // Reserve buf[0] as a flag (0 = empty, 1 = new data)
              read(0,(buf+1),SHARED_MEMORY_SIZE-1);
	      
	      // Set the first byte as flag = 1 to indicate new data available
	      buf[0] = 1;
	   }

	}
    }
    shmdt(buf);
    return 0;
}

