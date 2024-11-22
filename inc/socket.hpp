#ifndef GF_SOCKET
#define GF_SOCKET

#include <unistd.h> // for close()
#include <arpa/inet.h>

class Socket 
{ 
public:
    explicit Socket(int fd = -1);
    ~Socket();
    Socket(const Socket& other) = delete;
    Socket& operator=(const Socket& other) = delete;
    Socket(Socket&& other) noexcept;
    Socket& operator=(Socket&& other) noexcept;

    void Bind(const sockaddr_in& addr) const;
    void Listen(int backlog) const;

    int Get() const; 

protected:
    void ResetFD(int newFD = -1);

private:
    int socketFD;

    void closeIfOpen();
};

#endif //GF_SOCKET