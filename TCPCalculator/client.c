#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>

#define STDOUT_FILENO 1
#define STDIN_FILENO  0
#define BUF_SIZE 100

int main(int argc , char *argv[]) {
    if (argc != 3) {
        write(STDOUT_FILENO, "Usage: ./client <IP> <PORT>\n", 29);
        return -1;
    }

    int sockFd = socket(AF_INET, SOCK_STREAM ,0);
    if (sockFd == -1) {
        perror("socket");
        return -1;
    }

   /*************************** Socket Created successfully ************************/
    write(STDOUT_FILENO,"The Socket is created Successfully\n",sizeof("The Socket is created Successfully\n"));
   /********************************************************************************/

    struct sockaddr_in serverAddress;
    serverAddress.sin_family  = AF_INET;
    serverAddress.sin_port    = htons((uint16_t)atoi(argv[2]));
    if (inet_pton(AF_INET, argv[1], &serverAddress.sin_addr) <= 0) {
        perror("inet_pton");
        close(sockFd);
        return -1;
    }

    
    //while(1){
    if (connect(sockFd, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1) {
        perror("connect");
        return -1;
    }

    
   /*************************** Binding successfully *******************************/
    write(STDOUT_FILENO,"Connected Successfully to ",sizeof("Connected Successfully to "));
    write(STDOUT_FILENO,argv[1],strlen(argv[1]));
    write(STDOUT_FILENO,":",sizeof(":"));
    write(STDOUT_FILENO,argv[2],strlen(argv[2]));
    write(STDOUT_FILENO,"\n",sizeof("\n"));
   /********************************************************************************/

    char buf[BUF_SIZE];
    ssize_t bytesRead;
	
    // get the first num from the user (terminal)        
    write(STDOUT_FILENO, "Enter first number: ", sizeof("Enter first number: "));
    memset(buf, 0, BUF_SIZE);
    bytesRead = read(STDIN_FILENO, buf, BUF_SIZE);
    if (bytesRead <= 0) {
       perror("read");
       close(sockFd);
       return(-1);
    }

    buf[strcspn(buf, "\n")] = '\0';

    // send the number1 to the server
    write(sockFd, buf, strlen(buf));

    // get the second num from the user (terminal) 
    write(STDOUT_FILENO, "Enter second number: ", sizeof("Enter second number: "));
    memset(buf, 0, BUF_SIZE);
    bytesRead = read(STDIN_FILENO, buf, BUF_SIZE);
    if (bytesRead <= 0) {
       perror("read");
       close(sockFd);	   
       return(-1);
    }

    buf[strcspn(buf, "\n")] = '\0';

    // send the number2 to the server
    write(sockFd, buf, strlen(buf));

    // read the result from the server
    memset(buf, 0, BUF_SIZE);
    bytesRead = read(sockFd, buf, BUF_SIZE);
    if (bytesRead <= 0){
	perror("read");
	close(sockFd); 
	return(-1);
    }
    // print the result at the terminal
    write(STDOUT_FILENO, "Sum = ", sizeof("Sum = "));
    write(STDOUT_FILENO, buf, bytesRead);
    write(STDOUT_FILENO, "\n", sizeof("\n"));
    close(sockFd);

    return 0;
}

