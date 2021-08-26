#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>



int main( argc, argv )
int argc;
char **argv;

{
   char *tAddrStr = "192.168.42.139";
   unsigned long tAddr = inet_addr( tAddrStr );

  struct sockaddr_in      tDestAddr;
  int tSocket;
  int tStatus,tSize;
  int tMsgCount = 1;
  char tXtaBuffer[132];
  char tRcvBuffer[132];

  tSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (tSocket < 0) {
    printf("Failed to allocate socket\n");
    exit(0);
  }

  tDestAddr.sin_family        = AF_INET;
  tDestAddr.sin_port          = htons(55555);
  tDestAddr.sin_addr.s_addr   = (unsigned int) tAddr;

  tStatus = connect( tSocket, (struct sockaddr *) &tDestAddr, sizeof( tDestAddr));
  if (tStatus < 0)
  {
	  fprintf(stdout, "failed to connect");
	  exit(0);
  }

   printf( "Successfully connected\n");

   while( 1 == 1) {
    sprintf( tXtaBuffer,"Message number %d", tMsgCount++);
    printf("write message\n");
    tSize = write( tSocket, tXtaBuffer, strlen( tXtaBuffer));
    printf("sent message %d\n", tSize);
    if (tSize < 0) {
      printf("*** Send failure ***\n");
      exit(0);
     }

    tSize = read( tSocket, tRcvBuffer, sizeof( tRcvBuffer));
    if (tSize <= 0) {
      printf("*** Read failure ***\n");
      exit(0);
     }
    
    tRcvBuffer[ tSize ] = 0;
    printf("[read] %s\n", tRcvBuffer );
    sleep(2);
  }

   printf( "Bye \n");
   exit(0);
}
