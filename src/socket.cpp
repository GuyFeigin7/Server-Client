#include "socket.hpp"

#include <netinet/in.h> 
#include <stdexcept>
Socket::Socket(int fd) : socketFD(fd)
{}

Socket::~Socket()
{
    if (socketFD != -1)
    {
        close(socketFD);
    }
}

Socket::Socket(Socket &&other) noexcept : socketFD(other.Get())
{
    other.ResetFD(-1); 
}

Socket &Socket::operator=(Socket &&other) noexcept
{
    if (this != &other)
    {
        closeIfOpen();
        socketFD = other.Get();
        other.ResetFD(-1);
    }

    return *this;
}

void Socket::Bind(const sockaddr_in& addr) const
{
    if (bind(socketFD, (struct sockaddr*) &addr, sizeof(addr)) < 0)
    {
        throw std::runtime_error("Failed to bind socket.");
    } 
}

void Socket::Listen(int backlog) const
{
    if (listen(socketFD, backlog) < 0)
    {
        throw std::runtime_error("Failed to listen on socket.");
    }
}

int Socket::Get() const
{
    return socketFD;
}

void Socket::ResetFD(int newFD)
{
    socketFD = newFD;
}

void Socket::closeIfOpen()
{
    if (-1 != socketFD)
    {
        close(socketFD);
    }
}
