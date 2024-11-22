#ifndef GF_SERVER
#define GF_SERVER

#include "socket.hpp"
#include "user_data.hpp"

#include <unordered_map>
#include <unistd.h> // for uid_t
#include <memory> // for shared_ptr
#include <mutex> 
#include <string>
#include <atomic>

class Server
{
public:
    Server(int port);
    ~Server() = default;

    void Start();
    void Stop();

private:
    Socket serverSocket;
    std::unordered_map<uid_t, std::shared_ptr<UserData>> userData;
    std::mutex userDataLock;
    std::atomic<bool> isRunning;

    void AcceptConnections();
    void HandleClient(int clientSocket);
    void CleanUpClient(int clientSocket);
    void AddNewStr(uid_t linuxID, std::string& str);
    void SendStr(uid_t linuxID, unsigned int size, int clientSocket);
}; 

#endif //GF_SERVERs