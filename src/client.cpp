#include "client.hpp"

#include <stdexcept>
#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <vector>

Client::Client(const std::string &serverAddress_, int serverPort_, uid_t linuxID_) : clientSocket(socket(AF_INET, SOCK_STREAM, 0))
{ 
    if (clientSocket.Get() == -1)
    {
        throw std::runtime_error("Failed to create client socket.");
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(serverPort_);
    if (inet_pton(AF_INET, serverAddress_.c_str(), &serverAddr.sin_addr) <= 0)
    {
        throw std::runtime_error("Invalid server address.");
    }

    ConnectToServer();
}

void Client::SendData(const std::string &data)
{
    std::cout << "Client sending data: " << data << std::endl;

    std::string buffer(""); 
    RequestType reqType = SEND_REQUEST;

    buffer.append(reinterpret_cast<const char*>(&reqType), sizeof(reqType));
    buffer.append(reinterpret_cast<const char*>(&linuxID), sizeof(linuxID));
    buffer += data;

    ssize_t status = send(clientSocket.Get(), buffer.c_str(), buffer.size(), 0);
    if (-1 == status)
    {
        throw std::runtime_error("Failed to send data from client.");
    }

    char replyStatus = GetStatus();
    if (replyStatus != '0')
    {
        throw std::runtime_error("Server reply failed.");
    }

    std::cout << "Client sent data successfully." << std::endl;
}

std::string Client::ReceiveData(unsigned int size)
{
    std::cout << "Sending request to server..." << std::endl;

    std::string reqBuffer(""); 
    RequestType reqType = RECEIVE_REQUEST;

    reqBuffer.append(reinterpret_cast<const char*>(&reqType), sizeof(reqType));
    reqBuffer.append(reinterpret_cast<const char*>(&linuxID), sizeof(linuxID));
    reqBuffer.append(reinterpret_cast<const char*>(&size), sizeof(size));
    

    ssize_t sent = send(clientSocket.Get(), reqBuffer.c_str(), reqBuffer.size(), 0);
    if (sent == -1) 
    {
        throw std::runtime_error("Failed to send data to server.");
    }
    if (sent != static_cast<ssize_t>(reqBuffer.size()))
    {
        throw std::runtime_error("Invalid amount of data sent to server.");
    }

    std::vector<char> respBuffer(size, 0);

    unsigned int bytesReceived = 0;
    
    while (bytesReceived < size)
    {
        int chunkSize = recv(clientSocket.Get(), respBuffer.data() + bytesReceived, size - bytesReceived, 0);
        if (chunkSize == -1)
        {
            throw std::runtime_error("Failed to receive data from server.");
        }
        if (chunkSize == 0)
        {
            std::cerr << "Server closed the connection." << std::endl;
            throw std::runtime_error("Connection closed by server.");
        }
        bytesReceived += chunkSize;
    }

    std::string response(respBuffer.data(), bytesReceived);
    if (response.substr(0, 2) == "-1")
    {
        throw std::runtime_error("No data exists for this linuxID.");
    }
    if (response.substr(0, 2) == "-2")
    {
        throw std::runtime_error("Invalid size requested.");
    }

    std::cout << "Client received data successfully." << std::endl;

    return response;
}

const char Client::GetStatus()
{
    char buffer[1];

    ssize_t status = recv(clientSocket.Get(), buffer, sizeof(buffer), 0);
    if (-1 == status)
    {
        throw std::runtime_error("Failed to receive status from server.");
    }

    return buffer[0];
}

void Client::ConnectToServer()
{
    if (connect(clientSocket.Get(), (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
    {
        throw std::runtime_error("Failed to connect to server.");
    }

    std::cout << "Client successfully connected to server." << std::endl;
}

void Client::CloseConnection()
{
    close(clientSocket.Get());
    
    std::cout << "Connection closed." << std::endl;
}
