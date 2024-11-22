#include "server.hpp"

#include <iostream>
#include <sys/socket.h> // For socket(), bind(), listen(), accept()
#include <netinet/in.h> // For sockaddr_in
#include <sys/select.h> 
#include <thread>


Server::Server(int port) : serverSocket(socket(AF_INET, SOCK_STREAM, 0)), isRunning(false)
{
    if (-1 == serverSocket.Get())
    {
        throw std::runtime_error("Failed to create server socket.");
    }

    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(port);

    serverSocket.Bind(serverAddress);
    serverSocket.Listen(10);

    std::cout << "Server is listening on port " << port << std::endl;
}

void Server::Start()
{
    isRunning = true;
    std::cout << "Server started, waiting for connections..." << std::endl;
    AcceptConnections();
}

void Server::Stop()
{
    isRunning = false;
    std::cout << "Server stopped." << std::endl;
}

void Server::AcceptConnections()
{
    while (isRunning)
    {
        sockaddr_in clientAddr{};
        socklen_t clientLen = sizeof(clientAddr);

        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(serverSocket.Get(), &readfds);

        
        struct timeval timeout = {1, 0};  

        int activity = select(serverSocket.Get() + 1, &readfds, nullptr, nullptr, &timeout);
        if (activity < 0)
        {
            if (isRunning)
            {
                std::cerr << "Error in select()." << std::endl;
            }
            continue;
        }

        if (FD_ISSET(serverSocket.Get(), &readfds))
        {
            int clientSocket = accept(serverSocket.Get(), (struct sockaddr*)&clientAddr, &clientLen);
            if (clientSocket < 0)
            {
                if (isRunning)
                {
                    std::cerr << "Error accepting client connection." << std::endl;
                }
                continue;
            }

            std::thread clientThread(&Server::HandleClient, this, clientSocket);
            clientThread.detach();
        }

        if (!isRunning)
        {
            break;
        }
    }
}

void Server::HandleClient(int clientSocket)
{
    try
    {   
        while (true)
        {
            char buffer[1024 + sizeof(unsigned int) + sizeof(uid_t)] = {0};

            int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
            if (bytesReceived == 0)
            {
                std::cout << "Client disconnected." << std::endl;
                break;
            }

            if (bytesReceived < 0)
            {
                throw std::runtime_error("Failed to received data from client.");
            }

            std::cout << "Client connected succesfully. Parsing data..." << std::endl;

            if (bytesReceived > 1024 + static_cast<int>(sizeof(uid_t) + sizeof(unsigned int)))
            {
                throw std::runtime_error("Data max size exceeded (1024 bytes).");
            }
            
            unsigned int requestType = *reinterpret_cast<unsigned int *>(buffer);
            uid_t linuxID = *reinterpret_cast<uid_t *>(buffer + sizeof(unsigned int));

            if (requestType == 1)
            {
                std::cout << "In Server::HandleClient -> requestType == 1" << std::endl;

                std::string str(buffer + sizeof(unsigned int) + sizeof(uid_t), bytesReceived - sizeof(int) - sizeof(uid_t));
                AddNewStr(linuxID, str);

                std::string successMessage = "0";
                ssize_t status = send(clientSocket, successMessage.c_str(), successMessage.size(), 0);
                if (status == -1)
                {
                    throw std::runtime_error("Failed to send acknowledgment to client.");
                }
            }
            else if (requestType == 2)
            {
                std::cout << "In Server::HandleClient -> requestType == 2" << std::endl;
                
                unsigned int size = *reinterpret_cast<int *>(buffer + sizeof(unsigned int) + sizeof(uid_t));
                SendStr(linuxID, size, clientSocket);
            }
            else
            {
                throw std::runtime_error("Invalid request type.");
            }
        }
    }
    catch(const std::exception& ex)
    {
        std::cerr << "Error handling client: " << ex.what() << std::endl;
        std::string errorMessage = "Error: " + std::string(ex.what());
        ssize_t status = send(clientSocket, errorMessage.c_str(), errorMessage.size(), 0);
        if (-1 == status)
        {
            std::cerr << "Client error response message failed." << std::endl;
        }
    }

    CleanUpClient(clientSocket);
}

void Server::CleanUpClient(int clientSocket)
{
    close(clientSocket);
    std::cout << "Client disconnected." << std::endl;
}

void Server::AddNewStr(uid_t linuxID, std::string &str)
{
    std::cout << "In Server::AddNewStr\n";
    std::cout << "Raw client data: " << str << std::endl;
 
    std::lock_guard<std::mutex> lock(userDataLock);
    if (userData.find(linuxID) == userData.end())
    {
        std::cout << "New client, creating new user data" << std::endl;

        auto newData = std::make_shared<UserData>(str);
        userData[linuxID] = newData;
    }
    else
    {
        std::cout << "Existing client, appending new data" << std::endl;

        userData[linuxID]->AppendStr(str);
    }
}

void Server::SendStr(uid_t linuxID, unsigned int size, int clientSocket)
{
    std::lock_guard<std::mutex> lock(userDataLock);

    std::cout << "In Server::SendStr\n";

    if (userData.find(linuxID) == userData.end())
    {
        std::string error("-1", size);
        std::cerr << "No user data exists. Sending error msg..." << std::endl;

        ssize_t totalSent = send(clientSocket, error.c_str(), error.size(), 0);
        if (-1 == totalSent)
        {
            throw std::runtime_error("Server failed to send invalid userID error message.");
        }
        return;
    }

    if (userData[linuxID]->GetStr().size() < size)
    {
        std::string error("-2", size);
        std::cerr << "Invalid size request. Sending error msg..." << std::endl;
        
        ssize_t totalSent = send(clientSocket, error.c_str(), error.size(), 0);
        if (-1 == totalSent)
        {
            throw std::runtime_error("Server failed to send invalid size request error message.");
        }
        return;
    }

    userData[linuxID]->Sort();
    std::string response = userData[linuxID]->GetStr().substr(0, size);
    std::cout << "response to be sent to client: " << response << "\n";

    ssize_t totalSent = 0;
    ssize_t bytesLeft = response.size();

    while (totalSent < bytesLeft)
    {
        ssize_t sent = send(clientSocket, response.c_str() + totalSent, bytesLeft - totalSent, 0);
        if (sent == -1)
        {
            throw std::runtime_error("Failed to send response to client.");
        }
        totalSent += sent;
        bytesLeft -= sent;
    }

    userData[linuxID]->RemoveChars(size);

    std::cout << "Data after removing requested chars: " << userData[linuxID]->GetStr() << std::endl;
}

