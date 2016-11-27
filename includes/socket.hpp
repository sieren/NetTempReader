#include <fcntl.h>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

static const int BUFFSIZE = 2000;

class ServiceSocket
{
public:
  ServiceSocket(std::uint32_t port) :
    mPort(port)
  {
    initializeSocket();
  }

  ~ServiceSocket()
  {
    close(mFd);
  }

  void initializeSocket()
  {

    mFd = socket(AF_INET, SOCK_DGRAM, 0);
    if (mFd == 0)
    {
      std::cerr << "Could not create Socket" << std::endl;
      throw;
    }
    struct sockaddr_in sockAddr;
    memset((char *)&sockAddr, 0, sizeof(sockAddr));
    sockAddr.sin_family = AF_INET;
    sockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    sockAddr.sin_port = htons(mPort);
    int optval = 1;
    setsockopt(mFd, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));
    int sigval = 1;
    setsockopt (mFd, SOL_SOCKET, SO_NOSIGPIPE,
      reinterpret_cast <const char *> (&sigval), sizeof (sigval));


    if(bind(mFd, (struct sockaddr *)&sockAddr, sizeof(sockAddr)) < 0)
    {
      std::cerr << "Could not bind Socket" << '\n';
      throw;
    };
  }

  bool hasReceived()
  {
    fd_set fds;
    struct timeval tv;
    
    FD_ZERO (&fds);
    FD_SET (mFd, &fds);
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    bool recv = true;
    recv &= select (mFd + 1, &fds, 0, 0, NULL) > 0;
    recv &= FD_ISSET (mFd, &fds) != 0;

    return recv;
  }

  bool receive(std::uint32_t& ip, std::uint32_t& port, std::vector<uint8_t>& data)
  {
    data.resize(65535);
    struct sockaddr_in mRemoteAddr;
    memset (&mRemoteAddr, 0, sizeof (mRemoteAddr));
    socklen_t remoteAddrLen = sizeof(mRemoteAddr);
    ssize_t bytesRecv = 0;
    bytesRecv = recvfrom(mFd, &data[0], data.size(), 0,
      reinterpret_cast < ::sockaddr *> (&mRemoteAddr), &remoteAddrLen);

    // Nothing received
    if (bytesRecv <= 0)
    {
      return false;
    }
    data.resize(bytesRecv);
    ip = ntohl (mRemoteAddr.sin_addr.s_addr);
    port = ntohs (mRemoteAddr.sin_port);
    return true;
  }
  
private:
  int mFd = -1;
  std::uint32_t mPort = 0;
};
