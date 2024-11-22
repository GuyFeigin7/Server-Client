#ifndef GF_CLIENT
#define GF_CLIENT

#include <netinet/in.h> // For sockaddr_in
#include <string>

#include "socket.hpp"

class Client
{
public:
    enum RequestType
    {
        SEND_REQUEST = 1,
        RECEIVE_REQUEST = 2
    };

    Client(const std::string& serverAddress_, int serverPort_, uid_t linuxID_);
    ~Client() = default;

    void SendData(const std::string& data);
    std::string ReceiveData(unsigned int size);
    const char GetStatus();
    
private:
    Socket clientSocket;
    struct sockaddr_in serverAddr;
    uid_t linuxID;

    void ConnectToServer();
    void CloseConnection();
};


#endif //GF_CLIENT