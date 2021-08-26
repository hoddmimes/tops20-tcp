#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <jsys.h>
#include <sys/usydat.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/tcp.h>



/**
 This method wait for an inbound connection on port specified.
 It does not support machines having multiple host addresses 
 it just takes the default one. It's possible to specify the 
 interface on which the listener should be declared.
 Require a change to the tDeclare string.
**/

int waitForConn( pPort ) 
int   pPort; /* port to accept connections on */
{
  int tSts,jfn, fd;
  int acs[5];
  char  tDeclStr[80];
  struct _ufile *uf;


  fd = socket(AF_INET, SOCK_STREAM, 0);
  if (fd < 0) {
    printf("Failed to allocate socket\n");
    exit(0);
  }


  if ((pPort <= 255) || (pPort >= 32768)) {
        sprintf( tDeclStr,"TCP:%d\026#;CONNECT:PASSIVE", pPort );
  } else {
        sprintf( tDeclStr,"TCP:%d#;CONNECT:PASSIVE", pPort );
  }


  acs[1] = GJ_SHT;
  acs[2] = (int) (tDeclStr - 1);

  tSts = jsys(GTJFN, acs );
  if (tSts <= 0) {
    printf("GTJFN failure status: %d \n", tSts );
    exit(0);
  }

  jfn = acs[1]; 

  /* Open file and wait for connection */
  acs[2] = 0100400300000; /* 8bit interactive rd/wr */
  tSts = jsys(OPENF, acs );
  if (tSts <= 0) {
     printf("OPENF failure status: %d \n", tSts );
     exit(0);
  }

  bzero( acs, sizeof( acs ));
  acs[1] = jfn;

  tSts = jsys( GDSTS, acs );
  if (tSts <= 0) {
     printf("GDSTS failure status: %d \n", tSts );
     exit(0);
  }


  USYS_BEG();
  
  if (!(uf = USYS_VAR_REF(uffd[ fd ]))) {
    printf("*** failed to locate socket ***\n");
    exit(0);
  }

  uf->uf_ch = jfn;
  _openuf( fd, uf, O_RDWR | O_BINARY | O_BSIZE_8 ); 



  printf("inbound connection\n");
  USYS_RET(fd);
}




int main( argc, argv )
int argc;
char **argv;

{
  int tSize, tSocket;
  char tBuffer[256];

  while( 1 == 1) {
   tSocket = waitForConn( 5555 );
   while (tSocket != 0) {
    tSize = read( tSocket, tBuffer, sizeof( tBuffer));
    if (tSize <= 0) {
      printf("*** Read failure *** status: %d socket: %d\n", tSize, tSocket);
      tSocket = 0;
      break;	
    }
    tBuffer[ tSize ] = '\0';
    printf("[read] %s\n", tBuffer );

    tSize = write( tSocket, tBuffer, strlen( tBuffer));
    if (tSize < 0) {
      printf("*** Send failure ***\n");
      tSocket = 0;
      break;
     }
   }
}


