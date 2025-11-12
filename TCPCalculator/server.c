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

   struct sockaddr_in address ;

   address.sin_family = AF_INET;
   address.sin_port   = htons((uint16_t)atoi(argv[2]));

   if (inet_pton(AF_INET, argv[1], &address.sin_addr) <= 0) {
      perror("inet_pton");
      return(-1);
   }

   int bindstat = bind(sockFd, (struct sockaddr *)&address ,sizeof(address));

   if(-1 == bindstat){
      perror("bind : ");
      return (-1);
   }

   /********************************************************************************/
   write(STDOUT,"The Server is binding to ",sizeof("The Server is binding to "));
   write(STDOUT,argv[1],strlen(argv[1]));
   write(STDOUT,":",sizeof(":"));
   write(STDOUT,argv[2],strlen(argv[2]));
   write(STDOUT,"\n",sizeof("\n"));
   /********************************************************************************/

   int listenStat = listen(sockFd, BACKLOG);

   if(-1 == listenStat){
      perror("listen : ");
      return (-1);
   }

   write(STDOUT,"The Server is listening\n",sizeof("The Server is listening\n"));
   struct sockaddr_in client_addr;


   char buf[BUF_SIZE] = {'\0'};
   ssize_t n;
   int clientFd;

   while (1) {

      write(STDOUT,"waiting......\n",sizeof("waiting......\n"));

      clientFd = accept(sockFd,NULL,NULL);
      
      if (clientFd == -1){
         perror("accept : ");
         return (-1);
      }

      write(STDOUT,"Connected Successfully\n",sizeof("Connected Successfully\n"));

      n = read(clientFd, buf, BUF_SIZE);
      if(n == -1) {
         perror("read");
         break;
      }

      int num1 = atoi(buf);

      n = read(clientFd, buf, BUF_SIZE);
      if(n == -1) {
         perror("read");
         break;
      }

      int num2 = atoi(buf);

      char str[20];

      sprintf(str, "%d", num1+num2);   // Convert int → string
         

     n = write(clientFd, str, sizeof(str)); 
      if (n == -1) {
         perror("write");
      }

      close(clientFd);
   }   
}
