#include <asm-generic/socket.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <netinet/tcp.h>

#include "Socket.h"
#include "Logger.h"
#include "InetAddress.h"

Socket::~Socket()
{
    close(sockfd_);
}


void Socket::bindAddress(const InetAddress& localAddr)
{
    struct sockaddr_in addr;
    addr = localAddr.getSockaddr();
    int ret = ::bind(sockfd_, (struct sockaddr*)&addr, sizeof(sockaddr_in));
    if (ret < 0)
    {
        LOG_FATAL("bind sockfd: %d failed.\n", sockfd_);
    }
}   

void Socket::listen()
{
    int ret = ::listen(sockfd_, 128);
    if (ret < 0)
    {
        LOG_FATAL("listen sockfd: %d failed.\n", sockfd_);
    }    
}

int Socket::accept(InetAddress* peerAddr)
{
    struct sockaddr_in addr;
    bzero(&addr, sizeof addr);
    socklen_t len = sizeof(struct sockaddr_in);
    int connfd = ::accept(sockfd_, (struct sockaddr*)&addr, &len);
    if (connfd >= 0)
    {
        peerAddr->setSockAddr(addr);
    }
    return connfd;
}

void Socket::shutdownWrite()
{
    if (::shutdown(sockfd_, SHUT_WR) < 0)
    {
        LOG_ERROR("shutdownWrite error.\n");
    }
}

void Socket::setTcpNoDelay(bool on)
{
    int optval = on ? 1 : 0;
    ::setsockopt(sockfd_, IPPROTO_TCP, TCP_NODELAY, &optval, sizeof optval);
}

void Socket::setReuseAddr(bool on)
{
    int optval = on ? 1 : 0;
    ::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval);
}
void Socket::setReusePort(bool on)
{
    int optval = on ? 1 : 0;
    ::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof optval);
}

void Socket::setKeepAlive(bool on)
{
    int optval = on ? 1 : 0;
    ::setsockopt(sockfd_, SOL_SOCKET, SO_KEEPALIVE, &optval, sizeof optval);
}