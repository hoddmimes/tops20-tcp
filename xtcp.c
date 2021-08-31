/*      tcplib.h 1.0 2021-08-28 Havsvidden */


/**
 * This module implements bind, listen, accept
 * These function are missing in the KCC compiler.
 * The implementation is a bear minimum to be somewhat complaint.
 * the listen interface do not maintain a backlog for example.
 */

#include <c-env.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/file.h>
#include <sys/usydat.h>
#include <sys/usysio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <jsys.h>

#define I(byte, position) (((unsigned) (byte) & 0377) << ((position) * 8))

int     bind( fd, sockaddr, addr_len )
int fd;
const struct sockaddr *sockaddr;
int addr_len;
{
  const struct sockaddr_in *sin = (struct sockaddr_in *) sockaddr;
  char *p = (char *) &sin->sin_addr.S_un.S_addr;
  unsigned long tAddr = I(p[0],3) | I(p[0],2) | I(p[0],1) | I(p[0],0);

  struct _ufile *uf;
  char *srvStr;

  USYS_BEG();

  if (!(uf = USYS_VAR_REF(uffd[fd])))
    USYS_RETERR(ENOTSOCK);		/* err.. */

  uf->uf_ch = _PRIOU;  /* See CLOSE.C */
 
  uf->uf_buf = _iobget();
  srvStr = (char *) uf->uf_buf;

  if (tAddr == 0) {
    if ((sin->sin_port <= 255) || (sin->sin_port >= 32768)) {
        sprintf( srvStr,"TCP:%d\026#;CONNECT:PASSIVE", sin->sin_port );
    } else {
        sprintf( srvStr,"TCP:%d#;CONNECT:PASSIVE", sin->sin_port );
    }        
  } else {
    if ((sin->sin_port <= 255) || (sin->sin_port >= 32768)) {
        sprintf( srvStr,"TCP:.%lo-%d\026#;CONNECT:PASSIVE", tAddr,sin->sin_port );
    } else {
        sprintf( srvStr,"TCP:.%lo-%d#;CONNECT:PASSIVE", tAddr,sin->sin_port );
    }        
  }

  USYS_RET(0);
}



int     listen( fd,  backlog ) 
int fd;
int backlog;
{
  struct _ufile *uf;
 
 USYS_BEG();

  if (!(uf = USYS_VAR_REF(uffd[fd])))
    USYS_RETERR(ENOTSOCK);		/* err.. */

  USYS_RET(0);
}


static void addrToStr( int pAddr, char *pAddrStr ) {
 char a[4];
 int i;
 for( i = 0; i < 4; i++ ) {
  a[i] = (pAddr >> (i * 8)) &0xFF;
 }
 sprintf(pAddrStr,"%d.%d.%d.%d", a[3],a[2],a[1],a[0]);
}


int     accept( fd, sockaddr, adrlen )   
int fd;
struct sockaddr *sockaddr;
int *adrlen;
{
  const struct sockaddr_in *sin;
  unsigned long rmthst;
  int cfd, tSts, jfn, acs[5];
  struct _ufile *uf;
  char *srvStr;
  char rmthststr[80];


  USYS_BEG();

  if (!(uf = USYS_VAR_REF(uffd[fd]))) {
    printf("[accept] invalid server socket\n");
    USYS_RETERR(-1);		/* err.. */
  }

  srvStr = (char *) uf->uf_buf;

  USYS_END();

  acs[1] = GJ_SHT;
  acs[2] = (int) (srvStr - 1);

  tSts = jsys(GTJFN, acs );
  if (tSts <= 0) {
    printf("[accept] GTJFN failure status: %d \n", tSts );
    return -1;
  }

  jfn = acs[1]; 

  /* Open file and wait for connection */
  acs[2] = 0100400300000; /* 8bit interactive rd/wr */
  tSts = jsys(OPENF, acs );
  if (tSts <= 0) {
     printf("[accept] OPENF failure status: %d \n", tSts );
     return -1;
  }


  bzero( acs, sizeof( acs ));
  acs[1] = jfn;

  /* Get remote host */
  tSts = jsys( GDSTS, acs );
  if (tSts <=  0) {
     printf("[accept] GDSTS failure status: %d \n", tSts );
     return -1;
  }
  rmthst = acs[3];
  /* printf("remote host: %X\n", rmthst ); */

  /* Translate rmtadr to string */
  bzero( acs, sizeof( acs ));
  acs[1] = _GTHNS;
  acs[2] = (int) (rmthststr - 1);
  acs[3] = rmthst;
  tSts = jsys( GTHST, acs );
  if (tSts <= 0) {
    addrToStr( rmthst, rmthststr );
    }

  if (sockaddr != NULL) {
    sin = (struct sockaddr_in *) sockaddr;   
    sin->sin_addr.S_un.S_addr = rmthst;
    sin->sin_port = 0;
    sin->sin_family = AF_INET;
    if (adrlen != NULL) {
     *adrlen = sizeof( struct sockaddr_in );
    }
  }

  cfd = socket(AF_INET, SOCK_STREAM, 0);
  if (cfd < 0) {
    printf("[accept] Failed to allocate client socket\n");
    return -1;
  }

  USYS_BEG();
  
  if (!(uf = USYS_VAR_REF(uffd[ cfd ]))) {
    printf("[accept] failed to locate client socket\n");
    USYS_RETERR(-1);		/* err.. */
  }

  uf->uf_ch = jfn;
  _openuf( fd, uf, O_RDWR | O_BINARY | O_BSIZE_8 ); 



  printf("inbound connection from %s\n", rmthststr );
  USYS_RET(cfd);

}     

