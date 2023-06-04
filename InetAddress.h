#ifndef _INETADDRESS_H_ 
#define _INETADDRESS_H_

#include <netinet/in.h>
#include <string>
#include <arpa/inet.h>

class InetAddress
{
public:
    explicit InetAddress(uint16_t port, std::string ip = "127.0.0.1");
    explicit InetAddress(const sockaddr_in& addr)
        : addr_(addr)
    {}

    std::string toIp() const;
    std::string toIpPort() const;
    uint16_t toPort() const;

    const sockaddr_in getSockaddr() const;
    void setSockAddr(const sockaddr_in& addr) { addr_ = addr; }

private:
    struct sockaddr_in addr_;
};

#endif