#include <string>
#include <string.h>

#include "InetAddress.h"

 InetAddress::InetAddress(uint16_t port, std::string ip)
 {
    bzero(&addr_, sizeof(addr_));
    addr_.sin_family = AF_INET;
    addr_.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &addr_.sin_addr.s_addr);
 }

std::string InetAddress::toIp() const
{
    char ip[32] = {0};
    inet_ntop(AF_INET, &addr_.sin_addr.s_addr, ip, sizeof(ip));
    return ip;
}

std::string InetAddress::toIpPort() const
{
    char buf[64] = {0};
    char ip[32] = {0};
    inet_ntop(AF_INET, &addr_.sin_addr.s_addr, ip, sizeof(ip)); 
    const uint16_t port = ntohs(addr_.sin_port);
    sprintf(buf, "%s : %u", ip, port);
    return buf;
}

uint16_t InetAddress::toPort() const
{
    return ntohs(addr_.sin_port);
}

const sockaddr_in InetAddress::getSockaddr() const
{
    return addr_;
}

#if 0
#include <iostream>

int main()
{
    InetAddress add(3306);
    std::cout << add.toIpPort() << std::endl;
    std::cout << add.toPort() << std::endl;
    std::cout << add.toIp() << std::endl;

    return 0;
}

#endif