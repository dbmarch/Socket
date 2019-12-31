#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h> 

#include "Socket.h"


//*****************************************************
// Socket::Socket
//*****************************************************
Socket::Socket(int domain, int type) :
  mDomain(domain),
  mType (type)
{
  printf ("%s\n", __func__);
}


//*****************************************************
// Socket::Socket
//*****************************************************
Socket::Socket (const Socket& sock) 
{
  sockId = sock.sockId;

  mDomain = sock.mDomain;

  mType = sock.mType;
}

//*****************************************************
// Socket::~Socket
//*****************************************************
Socket::~Socket()
{
  printf ("%s\n", __func__);
}

//*****************************************************
// Socket::Open
//*****************************************************
int Socket::Open(void)
{	
  printf ("%s\n", __func__);
  sockId = socket(mDomain, mType, 0);
  if (sockId < 0) 
    return sockId;

  int optVal = 1;
  if (setsockopt(sockId, SOL_SOCKET, SO_REUSEADDR, &optVal, sizeof (int)) < 0 ) //You can reuse the address and the port
  {
    printf ("set socket option failed %s\n", strerror(errno));
  }

  return 0;
}


//*****************************************************
// Socket::Close
//*****************************************************
int Socket::Close(void)
{	
  return 0;
}


//*****************************************************
// Socket::Shutdown
//*****************************************************
int Socket::Shutdown(int how)
{	
  return shutdown(sockId, how);
}


//*****************************************************
// Socket::Recv
//*****************************************************
int Socket::Recv (uint8_t * buf , size_t len)
{	
  return recv (sockId, buf, len, 0);
}

//*****************************************************
// Socket::Send
//*****************************************************
int Socket::Send (uint8_t * buf , size_t len)
{
   int status = send(sockId, buf, len, 0);
    if (status < 0) {
        printf("recv error: %s\n", strerror(errno));
    }
  return status;
}

