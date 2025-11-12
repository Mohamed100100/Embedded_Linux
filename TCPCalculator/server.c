#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>

#define STDOUT  (1)
#define BACKLOG (4)
#define BUF_SIZE (100)

int main(int argc , char *argv[]) {
    if (argc != 3) {
        write(STDOUT, "Usage: ./server <IP> <PORT>\n", 29);
        return -1;
    }

    int sockFd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockFd == -1) {
        perror("socket");
        return -1;
    }


   /*************************** Socket Created successfully ************************/
    write(STDOUT,"The Socket is created Successfully\n",sizeof("The Socket is created Successfully\n"));
   /********************************************************************************/

    struct sockaddr_in address;
    address.sin_family  = AF_INET;
    address.sin_port    = htons((uint16_t)atoi(argv[2]));
    if (inet_pton(AF_INET, argv[1], &address.sin_addr) <= 0) {
        perror("inet_pton");
        return -1;
    }

    if (bind(sockFd, (struct sockaddr *)&address, sizeof(address)) == -1) {
        perror("bind");
        return -1;
    }
   /*************************** Binding successfully *******************************/ 
    write(STDOUT,"The Server is binding to ",sizeof("The Server is binding to ")); 
    write(STDOUT,argv[1],strlen(argv[1])); 
    write(STDOUT,":",sizeof(":")); 
    write(STDOUT,argv[2],strlen(argv[2])); 
    write(STDOUT,"\n",sizeof("\n")); 
   /********************************************************************************/

    listen(sockFd, BACKLOG);

   /*************************** Listening successfully ******************************/
    write(STDOUT, "Server listening...\n", sizeof("Server listening...\n"));
   /*********************************************************************************/
    char buf[BUF_SIZE];
    int clientFd;
    ssize_t n;
    char str[2];
    int num1;
    int num2;

    while (1) {
        clientFd = accept(sockFd, NULL, NULL);
        if (clientFd == -1) {
            perror("accept");
            continue;
        }

        write(STDOUT, "Client connected\n", sizeof("Client connected\n"));

	// receive the first number
        memset(buf, 0, BUF_SIZE);
        n = read(clientFd, buf, BUF_SIZE);
        if (n <= 0){ 
	   perror("read");
	   close(clientFd);
           continue;
	}

	// convert first number from string to int
        num1 = atoi(buf);

	// receive the second number
        memset(buf, 0, BUF_SIZE);
        n = read(clientFd, buf, BUF_SIZE);
        if (n <= 0){ 
	   perror("read");
           close(clientFd);
	   continue;
	}

	// convert second number from string to int
        num2 = atoi(buf);

	// convert the result to string
	sprintf(str,"%d",num1+num2);

	// send the result to the client
        write(clientFd, str, strlen(str));

	// after finish close the client socket
        close(clientFd);
    }

    close(sockFd);
    return 0;
}

