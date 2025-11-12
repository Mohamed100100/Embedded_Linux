#include <sys/types.h>          
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <stdlib.h>   
#include <string.h>
#define STDOUT		(1)
#define BACKLOG		(4)
#define BUF_SIZE	(100)

int main(int argc , char *argv[]){

   if(argc != 3){
      write(1," f",sizeof(" f"));
      return (-1);
   }

   
   int sockFd = socket(AF_INET, SOCK_STREAM ,0);

   if(-1 == sockFd){
      perror("socket : "); 
      return (-1);
   }

   write(STDOUT,"The Socket is created Successfully\n",sizeof("The Socket is created Successfully\n"));

   struct sockaddr_in serverAddress ;

   serverAddress.sin_family = AF_INET;
   serverAddress.sin_port = htons((uint16_t)atoi(argv[2]));

   if (inet_pton(AF_INET, argv[1], &serverAddress.sin_addr) <= 0) {
       perror("inet_pton : ");
       close(sockFd);
       return (-1);
   }


   int connectStat = connect(sockFd,(struct sockaddr *)&serverAddress,sizeof(serverAddress));

   if(-1 == connectStat){
      perror("connect : ");
      return(-1);
   }


    char buf[BUF_SIZE];
    ssize_t bytesRead;
    // Loop: read from terminal and send to server
    while(1) {
        write(STDOUT_FILENO, "Enter the first number : ", sizeof("Enter the first number : "));

        // Read from STDIN
        bytesRead = read(STDIN_FILENO, buf, sizeof(buf));
        if(bytesRead == -1) {
            perror("read");
            break;
        }

        // Send to server
        if(write(sockFd, buf, bytesRead) == -1) {
            perror("write");
            break;
        }

	write(STDOUT_FILENO, "Enter the second number : ", sizeof("Enter the second number : "));

        // Read from STDIN
        bytesRead = read(STDIN_FILENO, buf, sizeof(buf));
        if(bytesRead == -1) {
            perror("read");
            break;
        }

        // Send to server
        if(write(sockFd, buf, bytesRead) == -1) {
            perror("write");
            break;
        }


	// Read from STDIN
        bytesRead = read(sockFd, buf, sizeof(buf));
        if(bytesRead == -1) {
            perror("read");
            break;
        }


	write(STDOUT_FILENO,"The Result : ",sizeof("The Result : "));

        // Send to server
        if(write(STDOUT_FILENO, buf, bytesRead) == -1) {
            perror("write");
            break;
        }


    }

    close(sockFd);
    return 0;

}
