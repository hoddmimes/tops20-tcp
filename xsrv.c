#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>


int main( argc, argv ) 
int argc;
char **argv;
{
   int tSize, srvfd, tStatus, cltfd ;
   struct sockaddr_in       tAddr;
   char tBuffer[256];

   srvfd = socket(AF_INET, SOCK_STREAM, 0);
   if (srvfd < 0) {
     printf("Failed to allocate server socket\n");
     exit(0);
   }

  bzero( &tAddr, sizeof( struct sockaddr_in ));
  tAddr.sin_family = AF_INET;
  tAddr.sin_addr.s_addr = htonl( INADDR_ANY );
  tAddr.sin_port = htons( 5555 );

  tStatus = bind( srvfd, (struct sockaddr *) &tAddr, sizeof(tAddr));
  if (tStatus < 0) {
     printf("Failed to bind server socket\n");
     exit(0);
  }

  tStatus = listen( srvfd, 5 );
  if (tStatus < 0) {
     printf("Failed to listen server socket\n");
     exit(0);
  }

  while( 1 == 1 ) {

     cltfd = accept( srvfd, NULL, NULL );
     if (cltfd < 0) {
       printf("accept failed \n");
       exit(0);
     }
     printf("Inbound connection\n");
     while( cltfd != 0 ) {
        tSize = read( cltfd, tBuffer, sizeof( tBuffer));
        if (tSize <= 0) {
           printf("*** Read failure *** status: %d socket: %d\n", tSize, cltfd);
           cltfd = 0;
           break;	
        }
        tBuffer[ tSize ] = '\0';
        printf("[read] %s\n", tBuffer );

        tSize = write( cltfd, tBuffer, strlen( tBuffer));
        if (tSize < 0) {
           printf("*** Send failure ***\n");
           cltfd = 0;
           break;
         }           
     }
  }

}

